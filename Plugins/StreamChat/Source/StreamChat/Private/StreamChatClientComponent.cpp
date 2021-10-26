// Copyright Stream.IO, Inc. All Rights Reserved.

#include "StreamChatClientComponent.h"

#include "Algo/Transform.h"
#include "Channel/ChatChannel.h"
#include "ChatApi.h"
#include "IChatSocket.h"
#include "Request/Message/MessageRequestDto.h"
#include "Response/Channel/ChannelStateResponseDto.h"
#include "Response/Channel/ChannelsResponseDto.h"
#include "StreamChatSettings.h"
#include "Token/ConstantTokenProvider.h"
#include "Token/TokenManager.h"
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
    Api = MakeShared<FChatApi>(ApiKey, GetDefault<UStreamChatSettings>()->Host, TokenManager.ToSharedRef());
}

UChatChannel* UStreamChatClientComponent::CreateChannelObject(const FChannelStateResponseFieldsDto& Dto)
{
    return UChatChannel::Create(this, Api.ToSharedRef(), Socket.ToSharedRef(), CurrentUser.GetValue(), Dto);
}

void UStreamChatClientComponent::ConnectUser(const FUser& User, const FString& Token, const TFunction<void()> Callback)
{
    CurrentUser = User;
    // TODO: I don't like that the TokenManager is stateful. Maybe instantiate a token provider each time one is
    // needed?
    TokenManager->SetTokenProvider(MakeUnique<FConstantTokenProvider>(Token));
    Socket = IChatSocket::Create(
        ApiKey, TokenManager->LoadToken(), GetDefault<UStreamChatSettings>()->Host, static_cast<FUserObjectDto>(User));
    Socket->Connect(Callback);
}

void UStreamChatClientComponent::DisconnectUser()
{
    if (Socket)
    {
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
            Callback(NewChannels);
        },
        Socket->GetConnectionId(),
        EChannelFlags::State | EChannelFlags::Watch,
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
