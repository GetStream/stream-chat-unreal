// Copyright Stream.IO, Inc. All Rights Reserved.

#pragma once

#include "Channel/Message.h"
#include "CoreMinimal.h"
#include "UObject/NoExportTypes.h"

#include "ChatChannel.generated.h"

class FChatApi;
class FChatSocket;
struct FChannelStateResponseDto;
struct FChannelStateResponseFieldsDto;
struct FNewMessageEvent;

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FMessagesUpdatedDelegate, const TArray<FMessage>&, Messages);

/**
 * The client-side representation of a Stream Chat channel
 */
UCLASS(BlueprintType)
class STREAMCHAT_API UChatChannel final : public UObject
{
    GENERATED_BODY()

public:
    static UChatChannel* Create(const TSharedRef<FChatApi>&, FChatSocket&, const FString& Type, const FString& Id);
    static UChatChannel* Create(const TSharedRef<FChatApi>&, FChatSocket&, const FChannelStateResponseFieldsDto&);

    void Watch(TFunction<void()> Callback = {});

    UFUNCTION(BlueprintCallable, Category = "Stream Chat Channel")
    void SendMessage(const FString& Message, const FUser& FromUser);

    UFUNCTION(BlueprintPure, Category = "Stream Chat Channel")
    const TArray<FMessage>& GetMessages() const;

    UPROPERTY(BlueprintAssignable, Category = "Stream Chat Channel")
    FMessagesUpdatedDelegate MessagesUpdated;

private:
    void InitializeState(const FChannelStateResponseFieldsDto&);
    void AddMessage(const FMessage&);

    void OnNewMessage(const FNewMessageEvent&);

    UPROPERTY(BlueprintReadOnly, meta = (AllowPrivateAccess), Category = "Stream Chat Channel")
    FString Type;

    UPROPERTY(BlueprintReadOnly, meta = (AllowPrivateAccess), Category = "Stream Chat Channel")
    FString Id;

    TArray<FMessage> Messages;

    FString ConnectionId;

    TSharedPtr<FChatApi> Api;
};
