// Copyright Stream.IO, Inc. All Rights Reserved.

#include "StreamChatClientComponent.h"

#include "ChatApi.h"
#include "Socket/ChatSocket.h"
#include "Token/ConstantTokenProvider.h"
#include "Token/TokenManager.h"

UStreamChatClientComponent::UStreamChatClientComponent() : TokenManager(MakeUnique<FTokenManager>()), Api(MakeUnique<FChatApi>())
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
}

void UStreamChatClientComponent::ConnectUser(const FUser& User, const FString& Token)
{
	TokenManager->SetTokenProvider(MakeUnique<FConstantTokenProvider>(Token));
	Socket = MakeShared<FChatSocket>(ApiKey, User, *TokenManager);
	Socket->Connect();
}

void UStreamChatClientComponent::DisconnectUser()
{
	TokenManager->Reset();
	if (Socket)
	{
		Socket->Disconnect();
	}
}
