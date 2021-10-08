// Copyright Stream.IO, Inc. All Rights Reserved.

#pragma once

#include "Channel/Message.h"
#include "CoreMinimal.h"
#include "UObject/NoExportTypes.h"

#include "ChatChannel.generated.h"

class FChatApi;
class FChatSocket;
struct FChannelStateResponseDto;
struct FNewMessageEvent;

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FMessagesUpdatedDelegate, const TArray<FMessage>&, Messages);

/**
 *
 */
UCLASS()
class STREAMCHAT_API UChatChannel final : public UObject
{
    GENERATED_BODY()

public:
    static UChatChannel* Create(const TSharedRef<FChatApi>&, FChatSocket&, const FString& Type, const FString& Id);

    void Watch(TFunction<void()> Callback = {});

    UFUNCTION(BlueprintCallable)
    void SendMessage(const FString& Message, const FUser& FromUser);

    UFUNCTION(BlueprintPure)
    const TArray<FMessage>& GetMessages() const;

    UPROPERTY(BlueprintAssignable)
    FMessagesUpdatedDelegate MessagesUpdated;

private:
    void ApplyState(const FChannelStateResponseDto&);
    void AddMessage(const FMessage&);

    void OnNewMessage(const FNewMessageEvent&);

    UPROPERTY(BlueprintReadOnly, meta = (AllowPrivateAccess))
    FString Type;

    UPROPERTY(BlueprintReadOnly, meta = (AllowPrivateAccess))
    FString Id;

    TArray<FMessage> Messages;

    FString ConnectionId;

    TSharedPtr<FChatApi> Api;
};
