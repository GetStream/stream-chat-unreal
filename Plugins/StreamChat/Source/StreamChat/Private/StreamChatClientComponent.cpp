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
#include "Jwt/Public/Jwt.h"
#include "Request/Message/MessageRequestDto.h"
#include "Request/User/UserObjectRequestDto.h"
#include "Response/Channel/ChannelStateResponseDto.h"
#include "Response/Channel/ChannelsResponseDto.h"
#include "Response/Message/SearchResponseDto.h"
#include "Response/User/GuestResponseDto.h"
#include "Response/User/UsersResponseDto.h"
#include "StreamChatSettings.h"
#include "Token.h"
#include "TokenManager.h"
#include "User/UserManager.h"
#include "Util.h"

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
    const FUserRef UserRef = UUserManager::Get()->UpsertUser(User);
    Socket = IChatSocket::Create(TokenManager.ToSharedRef(), ApiKey, GetDefault<UStreamChatSettings>()->Host, FUserObjectDto(User));
    Socket->Connect(
        [Callback](const FOwnUserDto& OwnUser)
        {
            const FUserRef OwnUserRef = UUserManager::Get()->UpsertUser(OwnUser);
            UUserManager::Get()->SetCurrentUser(OwnUserRef);
            if (Callback)
            {
                Callback(OwnUserRef);
            }
        });

    On<FConnectionRecoveredEvent>(this, &UStreamChatClientComponent::OnConnectionRecovered);
    On<FUserPresenceChangedEvent>(this, &UStreamChatClientComponent::OnUserPresenceChanged);
    On<FMessageNewEvent>(this, &UStreamChatClientComponent::OnNewMessage);
}

UChatChannel* UStreamChatClientComponent::CreateChannelObject(const FChannelStateResponseFieldsDto& Dto)
{
    return UChatChannel::Create(this, Api.ToSharedRef(), Socket.ToSharedRef(), Dto);
}

void UStreamChatClientComponent::OnConnectionRecovered(const FConnectionRecoveredEvent&)
{
    // Fetch data for known channels
    TArray<FString> Cids;
    Algo::Transform(Channels, Cids, [](const UChatChannel* Channel) { return Channel->Properties.Cid; });
    QueryChannels({}, FFilter::In(TEXT("cid"), Cids));
}

void UStreamChatClientComponent::OnUserPresenceChanged(const FUserPresenceChangedEvent& Event)
{
    UUserManager::Get()->UpsertUser(Event.User);
}

void UStreamChatClientComponent::OnNewMessage(const FMessageNewEvent& Event)
{
    // Maintain order by most recent message
    if (const int32 Index = Channels.IndexOfByPredicate([&Event](const UChatChannel* Channel) { return Channel->Properties.Id == Event.ChannelId; });
        Index != 0)
    {
        // We could sort the entire array, but moving the channel to the first position should be enough
        UChatChannel* Channel = Channels[Index];
        Channels.RemoveAt(Index, 1, false);
        Channels.Insert(Channel, 0);
        ChannelsUpdated.Broadcast(Channels);
    }
}

void UStreamChatClientComponent::SetChannels(const TArray<UChatChannel*>& InChannels)
{
    Channels = InChannels;
    ChannelsUpdated.Broadcast(Channels);
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

void UStreamChatClientComponent::ConnectAnonymousUser(const UObject* WorldContextObject, const FLatentActionInfo LatentInfo, FUserRef& OutUser)
{
    TCallbackAction<FUserRef>::CreateLatentAction(WorldContextObject, LatentInfo, OutUser, [&](auto Callback) { ConnectAnonymousUser(Callback); });
}

void UStreamChatClientComponent::ConnectGuestUser(const FUser& User, const UObject* WorldContextObject, const FLatentActionInfo LatentInfo, FUserRef& OutUser)
{
    TCallbackAction<FUserRef>::CreateLatentAction(WorldContextObject, LatentInfo, OutUser, [&](auto Callback) { ConnectGuestUser(User, Callback); });
}

void UStreamChatClientComponent::QueryChannels(
    const FFilter Filter,
    const TArray<FChannelSortOption>& SortOptions,
    const UObject* WorldContextObject,
    const FLatentActionInfo LatentInfo,
    TArray<UChatChannel*>& OutChannels,
    const int32 Flags)
{
    const TOptional<FFilter> OptionalFilter = Filter.IsValid() ? Filter : TOptional<FFilter>{};
    TCallbackAction<TArray<UChatChannel*>>::CreateLatentAction(
        WorldContextObject,
        LatentInfo,
        OutChannels,
        [&](auto Callback) { QueryChannels(Callback, OptionalFilter, SortOptions, static_cast<EChannelFlags>(Flags)); });
}

void UStreamChatClientComponent::WatchChannel(
    const FChannelProperties& ChannelProperties,
    const UObject* WorldContextObject,
    const FLatentActionInfo LatentInfo,
    UChatChannel*& OutChannel)
{
    TCallbackAction<UChatChannel*, UChatChannel*>::CreateLatentAction(
        WorldContextObject, LatentInfo, OutChannel, [&](auto Callback) { WatchChannel(ChannelProperties, Callback); });
}

void UStreamChatClientComponent::ConnectUser(const FUser& User, TUniquePtr<ITokenProvider> TokenProvider, const TFunction<void(const FUserRef&)> Callback)
{
    if (UUserManager::Get()->GetCurrentUser().IsValid())
    {
        DisconnectUser();
    }

    TokenManager->SetTokenProvider(MoveTemp(TokenProvider), User.Id);
    ConnectUserInternal(User, Callback);
}

void UStreamChatClientComponent::ConnectUser(const FUser& User, const FString& Token, const TFunction<void(const FUserRef&)> Callback)
{
    if (UUserManager::Get()->GetCurrentUser().IsValid())
    {
        DisconnectUser();
    }

    TokenManager->SetTokenProvider(MakeUnique<FConstantTokenProvider>(FToken::Jwt(Token)), User.Id);
    ConnectUserInternal(User, Callback);
}

void UStreamChatClientComponent::ConnectAnonymousUser(const TFunction<void(const FUserRef&)> Callback)
{
    if (UUserManager::Get()->GetCurrentUser().IsValid())
    {
        DisconnectUser();
    }

    const FToken Token = FToken::Anonymous();
    const FUser User{TEXT("anonymous")};
    TokenManager->SetTokenProvider(MakeUnique<FConstantTokenProvider>(Token), User.Id);
    ConnectUserInternal(User, Callback);
}

void UStreamChatClientComponent::ConnectGuestUser(const FUser& User, TFunction<void(const FUserRef&)> Callback)
{
    const FToken AnonToken = FToken::Anonymous();
    TokenManager->SetTokenProvider(MakeUnique<FConstantTokenProvider>(AnonToken), User.Id);

    Api->CreateGuest(
        FUserObjectRequestDto(User),
        [WeakThis = TWeakObjectPtr<UStreamChatClientComponent>(this), Callback](const FGuestResponseDto& Response)
        {
            if (!WeakThis.IsValid())
            {
                return;
            }

            const FToken GuestToken = FToken::Jwt(Response.AccessToken);
            const FUser GuestUser = *UUserManager::Get()->UpsertUser(Response.User);
            WeakThis->TokenManager->SetTokenProvider(MakeUnique<FConstantTokenProvider>(GuestToken), GuestUser.Id);
            WeakThis->ConnectUserInternal(GuestUser, Callback);
        });
}

void UStreamChatClientComponent::DisconnectUser()
{
    if (Socket)
    {
        Socket->Events().UnsubscribeAll(this);
        Socket->Disconnect();
    }
    TokenManager->Reset();
    UUserManager::Get()->ResetCurrentUser();
}

void UStreamChatClientComponent::QueryChannels(
    TFunction<void(const TArray<UChatChannel*>&)> Callback,
    const TOptional<FFilter> Filter,
    const TArray<FChannelSortOption>& SortOptions,
    const EChannelFlags Flags,
    const FChannelPaginationOptions& PaginationOptions)
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
            WeakThis->SetChannels(NewChannels);

            if (Callback)
            {
                Callback(NewChannels);
            }
        },
        Socket->GetConnectionId(),
        Flags,
        FilterJson,
        Util::Convert<FSortParamRequestDto>(SortOptions),
        {},
        {},
        PaginationOptions.Limit,
        PaginationOptions.Offset);
}

void UStreamChatClientComponent::CreateChannel(const FChannelProperties& ChannelProperties, const TFunction<void(UChatChannel*)> Callback)
{
    QueryChannel(ChannelProperties, EChannelFlags::None, Callback);
}

void UStreamChatClientComponent::WatchChannel(const FChannelProperties& ChannelProperties, const TFunction<void(UChatChannel*)> Callback)
{
    QueryChannel(ChannelProperties, EChannelFlags::State | EChannelFlags::Watch, Callback);
}

void UStreamChatClientComponent::QueryChannel(const FChannelProperties& ChannelProperties, const EChannelFlags Flags, TFunction<void(UChatChannel*)> Callback)
{
    // TODO Can we return something from ConnectUser() that is required for this function to prevent ordering ambiguity?
    check(Socket->IsConnected());

    const TOptional<FString> OptionalId = ChannelProperties.Id.IsEmpty() ? TOptional<FString>{} : ChannelProperties.Id;
    const FChannelRequestDto Data(ChannelProperties);
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
        ChannelProperties.Type,
        Socket->GetConnectionId(),
        Flags,
        Data,
        OptionalId);
}

void UStreamChatClientComponent::QueryAdditionalChannels(EPaginationDirection Direction, int32 Limit)
{
}

void UStreamChatClientComponent::QueryUsers(
    TFunction<void(const TArray<FUserRef>&)> Callback,
    const FFilter& Filter,
    const TArray<FUserSortOption>& Sort,
    const bool bPresence,
    const TOptional<uint32> Limit,
    const TOptional<uint32> Offset) const
{
    Api->QueryUsers(
        [Callback](const FUsersResponseDto& Dto)
        {
            if (Callback)
            {
                UUserManager* UserManager = UUserManager::Get();
                TArray<FUserRef> Users;
                Algo::Transform(Dto.Users, Users, [UserManager](const FUserResponseDto& UserResponseDto) { return UserManager->UpsertUser(UserResponseDto); });
                Callback(Users);
            }
        },
        Socket->GetConnectionId(),
        bPresence,
        Filter.ToJsonObject(),
        Util::Convert<FSortParamRequestDto>(Sort),
        Limit,
        Offset);
}

void UStreamChatClientComponent::SearchMessages(
    TFunction<void(const TArray<FMessage>&)> Callback,
    const FFilter& ChannelFilter,
    const TOptional<FString>& Query,
    const TOptional<FFilter>& MessageFilter,
    const TArray<FMessageSortOption>& Sort,
    const TOptional<uint32> MessageLimit) const
{
    TOptional<TSharedRef<FJsonObject>> MessageFilterJson;
    if (MessageFilter.IsSet())
    {
        MessageFilterJson.Emplace(MessageFilter.GetValue().ToJsonObject());
    }

    Api->SearchMessages(
        [Callback](const FSearchResponseDto& Response)
        {
            if (Callback)
            {
                Callback(FMessage::FromSearchResults(Response.Results));
            }
        },
        ChannelFilter.ToJsonObject(),
        Query,
        MessageFilterJson,
        Util::Convert<FSortParamRequestDto>(Sort),
        MessageLimit);
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
