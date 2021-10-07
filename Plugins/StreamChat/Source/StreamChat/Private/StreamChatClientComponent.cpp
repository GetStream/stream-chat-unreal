// Copyright Stream.IO, Inc. All Rights Reserved.

#include "StreamChatClientComponent.h"

#include "Api/ChatApi.h"
#include "ChatChannel.h"
#include "Socket/ChatSocket.h"
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
    Api = MakeShared<FChatApi>(ApiKey, TokenManager.ToSharedRef());
}

void UStreamChatClientComponent::ConnectUser(const TFunction<void()> Callback, const FUser& User, const FString& Token)
{
    // TODO: I don't like that the TokenManager is stateful. Maybe instantiate a token provider each time one is
    // needed?
    TokenManager->SetTokenProvider(MakeUnique<FConstantTokenProvider>(Token));
    Socket = MakeShared<FChatSocket>(ApiKey, User, *TokenManager);
    Socket->Connect(Callback);
}

void UStreamChatClientComponent::DisconnectUser()
{
    TokenManager->Reset();
    if (Socket)
    {
        Socket->Disconnect();
    }
}

UChatChannel* UStreamChatClientComponent::Channel(const FString& Type, const FString& Id)
{
    // TODO Can we return something from ConnectUser() that is required for this function to prevent ordering ambiguity?
    check(Socket->IsConnected());

    UChatChannel* Channel = UChatChannel::Create(Api.ToSharedRef(), Socket->GetConnectionId(), Type, Id);

    // TODO This is obviously wrong
    Channels.Add(Type + TEXT(":") + Id, Channel);
    return Channel;
}
