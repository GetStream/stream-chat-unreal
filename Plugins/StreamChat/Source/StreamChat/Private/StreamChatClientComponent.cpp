// Copyright Stream.IO, Inc. All Rights Reserved.

#include "StreamChatClientComponent.h"

#include "Algo/Transform.h"
#include "Channel/ChatChannel.h"
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
    UserManager = FUserManager::Create(User);
    Socket = IChatSocket::Create(
        TokenManager.ToSharedRef(),
        ApiKey,
        GetDefault<UStreamChatSettings>()->Host,
        Util::Convert<FUserObjectDto>(User));
    Socket->Connect(
        [WeakThis = TWeakObjectPtr<UStreamChatClientComponent>(this), Callback](const FOwnUserDto& OwnUser)
        {
            if (!WeakThis.IsValid())
            {
                return;
            }
            const FUserRef Ref = WeakThis->UserManager->UpsertUser(OwnUser);
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
    return UChatChannel::Create(this, Api.ToSharedRef(), Socket.ToSharedRef(), UserManager.ToSharedRef(), Dto);
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
    UserManager->UpsertUser(Event.User);
}

void UStreamChatClientComponent::ConnectUser(
    const FUser& User,
    TUniquePtr<ITokenProvider> TokenProvider,
    const TFunction<void(const FUserRef&)> Callback)
{
    TokenManager->SetTokenProvider(MoveTemp(TokenProvider), User.Id);
    ConnectUserInternal(User, Callback);
}

void UStreamChatClientComponent::ConnectUser(
    const FUser& User,
    const FString& Token,
    const TFunction<void(const FUserRef&)> Callback)
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
    UserManager.Reset();
}

void UStreamChatClientComponent::QueryChannels(
    TFunction<void(const TArray<UChatChannel*>&)> Callback,
    const TOptional<FFilter> Filter,
    const TArray<FSortOption>& SortOptions)
{
    // TODO Can we return something from ConnectUser() that is required for this function to prevent ordering ambiguity?
    check(Socket->IsConnected());

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
                [WeakThis](const FChannelStateResponseFieldsDto& ResponseChannel)
                { return WeakThis->CreateChannelObject(ResponseChannel); });
            WeakThis->Channels = NewChannels;

            if (Callback)
            {
                Callback(NewChannels);
            }
        },
        Socket->GetConnectionId(),
        EChannelFlags::State | EChannelFlags::Watch | EChannelFlags::Presence,
        Filter->ToJsonObjectWrapper(),
        Util::Convert<FSortParamRequestDto>(SortOptions));
}

void UStreamChatClientComponent::WatchChannel(
    TFunction<void(UChatChannel*)> Callback,
    const FString& Type,
    const TOptional<FString>& Id,
    const TOptional<TArray<FString>> Members)
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
