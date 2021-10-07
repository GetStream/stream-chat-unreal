// Copyright Stream.IO, Inc. All Rights Reserved.

#pragma once

#include "Channel/Message.h"
#include "CoreMinimal.h"
#include "UObject/NoExportTypes.h"

#include "ChatChannel.generated.h"

class FChatApi;

/**
 *
 */
UCLASS()
class STREAMCHAT_API UChatChannel final : public UObject
{
    GENERATED_BODY()

public:
    static UChatChannel*
    Create(const TSharedRef<FChatApi>&, const FString& ConnectionId, const FString& Type, const FString& Id);

    void Watch(TFunction<void()> Callback = {});

    UFUNCTION(BlueprintCallable)
    void SendMessage(const FString& Message);

    UFUNCTION(BlueprintPure)
    const TArray<FMessage>& GetMessages() const;

private:
    UPROPERTY(BlueprintReadOnly, meta = (AllowPrivateAccess))
    FString Type;

    UPROPERTY(BlueprintReadOnly, meta = (AllowPrivateAccess))
    FString Id;

    TArray<FMessage> Messages;

    FString ConnectionId;

    TSharedPtr<FChatApi> Api;
};
