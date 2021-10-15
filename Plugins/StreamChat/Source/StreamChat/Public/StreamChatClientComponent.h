// Copyright Stream.IO, Inc. All Rights Reserved.

#pragma once

#include "Channel/Filter.h"
#include "Channel/SortOption.h"
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

    void ConnectUser(const FUser& User, const FString& Token, TFunction<void()> Callback = {});
    void DisconnectUser();

    void QueryChannels(
        TFunction<void(const TArray<UChatChannel*>&)> Callback,
        TOptional<FFilter> Filter = {},
        const TArray<FSortOption>& SortOptions = {});

    UChatChannel* Channel(const FString& Type, const FString& Id = {});

    UFUNCTION(BlueprintPure, Category = "Stream Chat Client")
    FUser GetCurrentUser() const;

    UFUNCTION(BlueprintPure, Category = "Stream Chat Client")
    const TArray<UChatChannel*>& GetChannels() const;

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
    TArray<UChatChannel*> Channels;
};
