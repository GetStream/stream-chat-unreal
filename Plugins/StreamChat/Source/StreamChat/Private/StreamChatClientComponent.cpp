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

void UStreamChatClientComponent::ConnectUserInternal(const FUser& User, const TFunction<void()> Callback)
{
    CurrentUser = User;
    Socket = IChatSocket::Create(
        TokenManager.ToSharedRef(), ApiKey, GetDefault<UStreamChatSettings>()->Host, static_cast<FUserObjectDto>(User));
    Socket->Connect(Callback);

    On<FConnectionRecoveredEvent>(this, &UStreamChatClientComponent::OnConnectionRecovered);
    On<FUserPresenceChangedEvent>(this, &UStreamChatClientComponent::OnUserPresenceChanged);
}

UChatChannel* UStreamChatClientComponent::CreateChannelObject(const FChannelStateResponseFieldsDto& Dto)
{
    return UChatChannel::Create(this, Api.ToSharedRef(), Socket.ToSharedRef(), CurrentUser.GetValue(), Dto);
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
    // TODO This seems fragile and easy to get out of sync
    // Do we need a user manager + user reference system which maintains a single source of truth for users?
    // User references would be passed around everywhere instead of FUser.
    //  A user reference would contain a pointer to the manager and and index into its list of users.

    const FUser UpdatedUser = Util::Convert<FUser>(Event.User);
    // Update members in all channels
    for (UChatChannel* Channel : Channels)
    {
        for (FMember& Member : Channel->State.Members)
        {
            if (Member.User.Id == UpdatedUser.Id)
            {
                Member.User.Update(UpdatedUser);
            }
        }
    }
}

void UStreamChatClientComponent::ConnectUser(
    const FUser& User,
    TUniquePtr<ITokenProvider> TokenProvider,
    const TFunction<void()> Callback)
{
    TokenManager->SetTokenProvider(MoveTemp(TokenProvider), User.Id);
    ConnectUserInternal(User, Callback);
}

void UStreamChatClientComponent::ConnectUser(const FUser& User, const FString& Token, const TFunction<void()> Callback)
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
    CurrentUser.Reset();
}

void UStreamChatClientComponent::QueryChannels(
    TFunction<void(const TArray<UChatChannel*>&)> Callback,
    const TOptional<FFilter> Filter,
    const TArray<FSortOption>& SortOptions)
{
    // TODO Can we return something from ConnectUser() that is required for this function to prevent ordering ambiguity?
    check(Socket->IsConnected());

    Api->QueryChannels(
        [this, Callback](const FChannelsResponseDto& Response)
        {
            TArray<UChatChannel*> NewChannels;
            Algo::Transform(
                Response.Channels,
                NewChannels,
                [this](const FChannelStateResponseFieldsDto& ResponseChannel)
                { return CreateChannelObject(ResponseChannel); });
            Channels = NewChannels;
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
        [this, Callback](const FChannelStateResponseDto& Dto)
        {
            UChatChannel* Channel = CreateChannelObject(Dto);

            Channels.Add(Channel);
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
