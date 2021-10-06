// Copyright Stream.IO, Inc. All Rights Reserved.

#pragma once

#include "Components/ActorComponent.h"
#include "CoreMinimal.h"

#include "StreamChatClientComponent.generated.h"

class FChatApi;
class FChatSocket;
class FTokenManager;
class UChatChannel;
struct FUser;

UCLASS(ClassGroup = (Custom), meta = (BlueprintSpawnableComponent))
class STREAMCHAT_API UStreamChatClientComponent final : public UActorComponent
{
    GENERATED_BODY()

public:
    UStreamChatClientComponent();

    void ConnectUser(const TFunction<void()> Callback, const FUser& User, const FString& Token);
    void DisconnectUser();

    UChatChannel* Channel(const FString& Type, const FString& Id = {});

    UPROPERTY(EditAnywhere, Config, meta = (DisplayName = "API Key"))
    FString ApiKey;

private:
    // Called when the game starts
    virtual void BeginPlay() override;

    TSharedPtr<FTokenManager> TokenManager;
    TSharedPtr<FChatApi> Api;
    TSharedPtr<FChatSocket> Socket;

    UPROPERTY(Transient)
    TMap<FString, UChatChannel*> Channels;
};
