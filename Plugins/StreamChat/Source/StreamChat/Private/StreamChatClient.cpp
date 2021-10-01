// © Stream.IO, Inc. All Rights Reserved.

#include "StreamChatClient.h"

#include "StreamChatSettings.h"

bool UStreamChatClient::ShouldCreateSubsystem(UObject* Outer) const
{
	if (GetDefault<UStreamChatSettings>()->ApiKey.IsEmpty())
	{
		UE_LOG(LogTemp, Warning, TEXT("Stream Chat client will not be initialized without an API key"))
		return false;
	}

	return Super::ShouldCreateSubsystem(Outer);
}

void UStreamChatClient::Initialize(FSubsystemCollectionBase& Collection)
{
	Super::Initialize(Collection);
	UE_LOG(LogTemp, Log, TEXT("Stream Chat client initialized"))
}
