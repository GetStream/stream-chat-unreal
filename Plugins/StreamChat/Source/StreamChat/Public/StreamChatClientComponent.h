// Copyright Stream.IO, Inc. All Rights Reserved.

#pragma once

#include "Channel/Filter.h"
#include "Channel/SortOption.h"
#include "Components/ActorComponent.h"
#include "CoreMinimal.h"
#include "User.h"

#include "StreamChatClientComponent.generated.h"

class FChatApi;
class IChatSocket;
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

    // TODO does this need to be exposed?
    void UpdateMessage(const FString& Id, const FString& Text) const;
    void DeleteMessage(const FString& Id) const;

    UFUNCTION(BlueprintPure, Category = "Stream Chat Client")
    FUser GetCurrentUser() const;

    UFUNCTION(BlueprintPure, Category = "Stream Chat Client")
    const TArray<UChatChannel*>& GetChannels() const;

    TSharedRef<FChatApi> GetApi() const;
    TSharedRef<IChatSocket> GetSocket() const;

    UPROPERTY(EditAnywhere, Config, Category = "Stream Chat Client", meta = (DisplayName = "API Key"))
    FString ApiKey;

private:
    // Called when the game starts
    virtual void BeginPlay() override;

    TSharedPtr<FTokenManager> TokenManager;
    TSharedPtr<FChatApi> Api;
    TSharedPtr<IChatSocket> Socket;

    TOptional<FUser> CurrentUser;

    UPROPERTY(Transient)
    TArray<UChatChannel*> Channels;
};
