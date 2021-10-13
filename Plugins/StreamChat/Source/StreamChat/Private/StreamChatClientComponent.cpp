// Copyright Stream.IO, Inc. All Rights Reserved.

#include "StreamChatClientComponent.h"

#include "Algo/Transform.h"
#include "Channel/ChatChannel.h"
#include "ChatApi.h"
#include "Dto/Response/ChannelsResponseDto.h"
#include "Dto/Util.h"
#include "PaginationOptions.h"
#include "Socket/ChatSocket.h"
#include "StreamChatSettings.h"
#include "Token/ConstantTokenProvider.h"
#include "Token/TokenManager.h"

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

void UStreamChatClientComponent::ConnectUser(const FUser& User, const FString& Token, const TFunction<void()> Callback)
{
    CurrentUser = User;
    // TODO: I don't like that the TokenManager is stateful. Maybe instantiate a token provider each time one is
    // needed?
    TokenManager->SetTokenProvider(MakeUnique<FConstantTokenProvider>(Token));
    Socket = MakeShared<FChatSocket>(ApiKey, User, *TokenManager);
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
    const TOptional<FFilter>& Filter,
    const TOptional<TArray<FSortOption>>& SortOptions,
    TFunction<void(const TArray<UChatChannel*>&)> Callback)
{
    // TODO Can we return something from ConnectUser() that is required for this function to prevent ordering ambiguity?
    check(Socket->IsConnected());

    Api->QueryChannels(
        Socket->GetConnectionId(),
        Util::Convert<FFilterDto>(Filter),
        Util::Convert<FSortOptionDto>(SortOptions),
        {},
        {},
        EChannelFlags::State | EChannelFlags::Watch,
        {},
        [this, Callback](const FChannelsResponseDto& Response)
        {
            TArray<UChatChannel*> NewChannels;
            Algo::Transform(
                Response.Channels,
                NewChannels,
                [this](const FChannelStateResponseFieldsDto& ResponseChannel) {
                    return UChatChannel::Create(
                        Api.ToSharedRef(), *Socket, ResponseChannel.Channel.Type, ResponseChannel.Channel.Id);
                });
            Callback(NewChannels);
        });
}

UChatChannel* UStreamChatClientComponent::Channel(const FString& Type, const FString& Id)
{
    // TODO Can we return something from ConnectUser() that is required for this function to prevent ordering ambiguity?
    check(Socket->IsConnected());

    UChatChannel* Channel = UChatChannel::Create(Api.ToSharedRef(), *Socket, Type, Id);

    // TODO This is obviously wrong
    Channels.Add(Type + TEXT(":") + Id, Channel);
    return Channel;
}

FUser UStreamChatClientComponent::GetCurrentUser() const
{
    // TODO Better optional support
    return CurrentUser.Get(FUser{TEXT("ERROR")});
}
