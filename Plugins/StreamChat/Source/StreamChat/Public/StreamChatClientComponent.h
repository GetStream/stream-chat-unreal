// Copyright Stream.IO, Inc. All Rights Reserved.

#pragma once

#include "Components/ActorComponent.h"
#include "CoreMinimal.h"
#include "User.h"

#include "StreamChatClientComponent.generated.h"

class FChatApi;
class FChatSocket;
class FTokenManager;
class UChatChannel;

UCLASS(ClassGroup = (Custom), meta = (BlueprintSpawnableComponent))
class STREAMCHAT_API UStreamChatClientComponent final : public UActorComponent
{
    GENERATED_BODY()

public:
    UStreamChatClientComponent();

    void ConnectUser(const FUser& User, const FString& Token, TFunction<void()> Callback);
    void DisconnectUser();

    UChatChannel* Channel(const FString& Type, const FString& Id = {});

    UFUNCTION(BlueprintPure, Category = "Stream Chat Client")
    FUser GetCurrentUser() const;

    UPROPERTY(EditAnywhere, Config, Category = "Stream Chat Client", meta = (DisplayName = "API Key"))
    FString ApiKey;

private:
    // Called when the game starts
    virtual void BeginPlay() override;

    TSharedPtr<FTokenManager> TokenManager;
    TSharedPtr<FChatApi> Api;
    TSharedPtr<FChatSocket> Socket;

    TOptional<FUser> CurrentUser;

    UPROPERTY(Transient)
    TMap<FString, UChatChannel*> Channels;
};
