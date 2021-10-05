// Copyright Stream.IO, Inc. All Rights Reserved.

#include "StreamChatClientComponent.h"

#include "Api/ChatApi.h"
#include "ChatChannel.h"
#include "Socket/ChatSocket.h"
#include "Token/ConstantTokenProvider.h"
#include "Token/TokenManager.h"

UStreamChatClientComponent::UStreamChatClientComponent() : TokenManager(MakeUnique<FTokenManager>())
{
	PrimaryComponentTick.bCanEverTick = false;
}

UStreamChatClientComponent::UStreamChatClientComponent(FVTableHelper&)
{
}

UStreamChatClientComponent::~UStreamChatClientComponent()
{
}

// Called when the game starts
void UStreamChatClientComponent::BeginPlay()
{
	Super::BeginPlay();

	// Initialize in begin play to ensure properties like ApiKey are loaded from BP
	Api = MakeShared<FChatApi>(ApiKey);
}

void UStreamChatClientComponent::ConnectUser(const TFunction<void()> Callback, const FUser& User, const FString& Token)
{
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
	UChatChannel* Channel = UChatChannel::Create(Api.ToSharedRef(), Type, Id);
	// TODO This is obviously wrong
	Channels.Add(Type + TEXT(":") + Id, Channel);
	return Channel;
}
