// Copyright 2021 Stream.IO, Inc. All Rights Reserved.

#include "StreamChatClientComponent.h"

#include "Algo/Transform.h"
#include "Blueprint/CallbackAction.h"
#include "Channel/ChatChannel.h"
#include "Channel/Filter.h"
#include "ChatApi.h"
#include "ConstantTokenProvider.h"
#include "Event/Client/ConnectionRecoveredEvent.h"
#include "Event/User/UserPresenceChangedEvent.h"
#include "IChatSocket.h"
#include "Request/Message/MessageRequestDto.h"
#include "Response/Channel/ChannelStateResponseDto.h"
#include "Response/Channel/ChannelsResponseDto.h"
#include "StreamChatSettings.h"
#include "TokenManager.h"
#include "User/UserManager.h"
#include "Util.h"
#include "Jwt/Public/Jwt.h"

UStreamChatClientComponent::UStreamChatClientComponent() : TokenManager(MakeShared<FTokenManager>())
{
    PrimaryComponentTick.bCanEverTick = false;
}

// Called when the game starts
void UStreamChatClientComponent::BeginPlay()
{
    Super::BeginPlay();

    // Initialize in BeginPlay to ensure properties like ApiKey are loaded from BP, TODO I don't like this
    Api = FChatApi::Create(ApiKey, GetDefault<UStreamChatSettings>()->Host, TokenManager);
}

void UStreamChatClientComponent::ConnectUserInternal(const FUser& User, const TFunction<void(const FUserRef&)> Callback)
{
    UUserManager::Get()->SetCurrentUser(User);
    Socket = IChatSocket::Create(TokenManager.ToSharedRef(), ApiKey, GetDefault<UStreamChatSettings>()->Host, Util::Convert<FUserObjectDto>(User));
    Socket->Connect(
        [Callback](const FOwnUserDto& OwnUser)
        {
            const FUserRef Ref = UUserManager::Get()->UpsertUser(OwnUser);
            if (Callback)
            {
                Callback(Ref);
            }
        });

    On<FConnectionRecoveredEvent>(this, &UStreamChatClientComponent::OnConnectionRecovered);
    On<FUserPresenceChangedEvent>(this, &UStreamChatClientComponent::OnUserPresenceChanged);
}

UChatChannel* UStreamChatClientComponent::CreateChannelObject(const FChannelStateResponseFieldsDto& Dto)
{
    return UChatChannel::Create(this, Api.ToSharedRef(), Socket.ToSharedRef(), Dto);
}

void UStreamChatClientComponent::OnConnectionRecovered(const FConnectionRecoveredEvent&)
{
    // Fetch data for known channels
    TArray<FString> Cids;
    Algo::Transform(Channels, Cids, [](const UChatChannel* Channel) { return Channel->State.Cid; });
    QueryChannels({}, FFilter::In(TEXT("cid"), Cids));
}

void UStreamChatClientComponent::OnUserPresenceChanged(const FUserPresenceChangedEvent& Event)
{
    UUserManager::Get()->UpsertUser(Event.User);
}

void UStreamChatClientComponent::ConnectUser(
    const FUser& User,
    const FString& Token,
    const UObject* WorldContextObject,
    const FLatentActionInfo LatentInfo,
    FUserRef& OutUser)
{
    TCallbackAction<FUserRef>::CreateLatentAction(WorldContextObject, LatentInfo, OutUser, [&](auto Callback) { ConnectUser(User, Token, Callback); });
}

void UStreamChatClientComponent::QueryChannels(
    const FFilter Filter,
    const TArray<FChannelSortOption>& SortOptions,
    const UObject* WorldContextObject,
    const FLatentActionInfo LatentInfo,
    TArray<UChatChannel*>& OutChannels)
{
    const TOptional<FFilter> OptionalFilter = Filter.IsValid() ? Filter : TOptional<FFilter>{};
    TCallbackAction<TArray<UChatChannel*>>::CreateLatentAction(
        WorldContextObject, LatentInfo, OutChannels, [&](auto Callback) { QueryChannels(Callback, OptionalFilter, SortOptions); });
}

void UStreamChatClientComponent::WatchChannel(
    const FString& Type,
    const TArray<FString>& Members,
    const FString& Id,
    const UObject* WorldContextObject,
    const FLatentActionInfo LatentInfo,
    UChatChannel*& OutChannel)
{
    const TOptional<FString> OptionalId = Id.IsEmpty() ? TOptional<FString>{} : Id;
    const TOptional<TArray<FString>> OptionalMembers = Members.Num() == 0 ? TOptional<TArray<FString>>{} : Members;
    TCallbackAction<UChatChannel*, UChatChannel*>::CreateLatentAction(
        WorldContextObject, LatentInfo, OutChannel, [&](auto Callback) { WatchChannel(Callback, Type, OptionalId, OptionalMembers); });
}

void UStreamChatClientComponent::ConnectUser(const FUser& User, TUniquePtr<ITokenProvider> TokenProvider, const TFunction<void(const FUserRef&)> Callback)
{
    TokenManager->SetTokenProvider(MoveTemp(TokenProvider), User.Id);
    ConnectUserInternal(User, Callback);
}

void UStreamChatClientComponent::ConnectUser(const FUser& User, const FString& Token, const TFunction<void(const FUserRef&)> Callback)
{
    TokenManager->SetTokenProvider(MakeUnique<FConstantTokenProvider>(Token), User.Id);
    ConnectUserInternal(User, Callback);
}

void UStreamChatClientComponent::DisconnectUser()
{
    if (Socket)
    {
        Socket->Events().UnsubscribeAll(this);
        Socket->Disconnect();
    }
    TokenManager->Reset();
}

void UStreamChatClientComponent::QueryChannels(
    TFunction<void(const TArray<UChatChannel*>&)> Callback,
    const TOptional<FFilter> Filter,
    const TArray<FChannelSortOption>& SortOptions)
{
    // TODO Can we return something from ConnectUser() that is required for this function to prevent ordering ambiguity?
    check(Socket->IsConnected());

    TOptional<TSharedRef<FJsonObject>> FilterJson;
    if (Filter.IsSet())
    {
        FilterJson.Emplace(Filter.GetValue().ToJsonObject());
    }

    Api->QueryChannels(
        [WeakThis = TWeakObjectPtr<UStreamChatClientComponent>(this), Callback](const FChannelsResponseDto& Response)
        {
            if (!WeakThis.IsValid())
            {
                return;
            }

            TArray<UChatChannel*> NewChannels;
            Algo::Transform(
                Response.Channels,
                NewChannels,
                [WeakThis](const FChannelStateResponseFieldsDto& ResponseChannel) { return WeakThis->CreateChannelObject(ResponseChannel); });
            WeakThis->Channels = NewChannels;

            if (Callback)
            {
                Callback(NewChannels);
            }
        },
        Socket->GetConnectionId(),
        EChannelFlags::State | EChannelFlags::Watch | EChannelFlags::Presence,
        FilterJson,
        Util::Convert<FSortParamRequestDto>(SortOptions));
}

void UStreamChatClientComponent::WatchChannel(
    TFunction<void(UChatChannel*)> Callback,
    const FString& Type,
    const TOptional<FString>& Id,
    const TOptional<TArray<FString>>& Members)
{
    // TODO Can we return something from ConnectUser() that is required for this function to prevent ordering ambiguity?
    check(Socket->IsConnected());

    Api->QueryChannel(
        [WeakThis = TWeakObjectPtr<UStreamChatClientComponent>(this), Callback](const FChannelStateResponseDto& Dto)
        {
            if (!WeakThis.IsValid())
            {
                return;
            }
            UChatChannel* Channel = WeakThis->CreateChannelObject(Dto);

            WeakThis->Channels.Add(Channel);
            if (Callback)
            {
                Callback(Channel);
            }
        },
        Type,
        Socket->GetConnectionId(),
        EChannelFlags::State | EChannelFlags::Watch,
        {Members.Get({})},
        Id);
}

FString UStreamChatClientComponent::DevToken(const FString& UserId)
{
    return Jwt::Development(UserId);
}

void UStreamChatClientComponent::UpdateMessage(const FString& Id, const FString& Text) const
{
    // TODO Can we return something from ConnectUser() that is required for this function to prevent ordering ambiguity?
    check(Socket->IsConnected());

    Api->UpdateMessage(FMessageRequestDto{Id, Text});
}

void UStreamChatClientComponent::DeleteMessage(const FString& Id) const
{
    // TODO Can we return something from ConnectUser() that is required for this function to prevent ordering ambiguity?
    check(Socket->IsConnected());

    Api->DeleteMessage(Id);
}

const TArray<UChatChannel*>& UStreamChatClientComponent::GetChannels() const
{
    return Channels;
}
