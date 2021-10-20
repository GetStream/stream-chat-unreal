// Copyright Stream.IO, Inc. All Rights Reserved.

#pragma once

#include "Channel/Message.h"
#include "CoreMinimal.h"
#include "UObject/NoExportTypes.h"

#include "ChatChannel.generated.h"

class FChatApi;
class IChatSocket;
class UStreamChatClientComponent;
struct FChannelStateResponseDto;
struct FChannelStateResponseFieldsDto;
struct FMessageDeletedEvent;
struct FMessageNewEvent;
struct FMessageUpdatedEvent;
struct FReactionDeletedEvent;
struct FReactionNewEvent;
struct FReactionUpdatedEvent;

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FMessagesUpdatedDelegate, const TArray<FMessage>&, Messages);

/**
 * The client-side representation of a Stream Chat channel
 */
UCLASS(BlueprintType)
class STREAMCHAT_API UChatChannel final : public UObject
{
    GENERATED_BODY()

public:
    static UChatChannel* Create(const UStreamChatClientComponent&, const FString& Type, const FString& Id);
    static UChatChannel* Create(const UStreamChatClientComponent&, const FChannelStateResponseFieldsDto&);

    void Watch(TFunction<void()> Callback = {});

    UFUNCTION(BlueprintCallable, Category = "Stream Chat Channel")
    void SendMessage(const FString& Text, const FUser& FromUser);

    UFUNCTION(BlueprintCallable, Category = "Stream Chat Channel")
    void UpdateMessage(const FMessage& Message);

    UFUNCTION(BlueprintCallable, Category = "Stream Chat Channel")
    void DeleteMessage(const FMessage& Message);

    UFUNCTION(BlueprintPure, Category = "Stream Chat Channel")
    const TArray<FMessage>& GetMessages() const;

    UFUNCTION(BlueprintCallable, Category = "Stream Chat Channel")
    void SendReaction(const FMessage& Message, const FName& ReactionType, bool bEnforceUnique = true);

    UPROPERTY(BlueprintAssignable, Category = "Stream Chat Channel")
    FMessagesUpdatedDelegate MessagesUpdated;

private:
    void InitializeState(const FChannelStateResponseFieldsDto&);
    void AddMessage(const FMessage&);

    void OnMessageNew(const FMessageNewEvent&);
    void OnMessageUpdated(const FMessageUpdatedEvent&);
    void OnMessageDeleted(const FMessageDeletedEvent&);
    void OnReactionNew(const FReactionNewEvent&);
    void OnReactionUpdated(const FReactionUpdatedEvent&);
    void OnReactionDeleted(const FReactionDeletedEvent&);

    UPROPERTY(BlueprintReadOnly, meta = (AllowPrivateAccess), Category = "Stream Chat Channel")
    FString Type;

    UPROPERTY(BlueprintReadOnly, meta = (AllowPrivateAccess), Category = "Stream Chat Channel")
    FString Id;

    // Not in spec so might not be set
    UPROPERTY(BlueprintReadOnly, meta = (AllowPrivateAccess), Category = "Stream Chat Channel")
    FString Name;

    // Not in spec so might not be set
    UPROPERTY(BlueprintReadOnly, meta = (AllowPrivateAccess), Category = "Stream Chat Channel")
    FString ImageUrl;

    /// The cid of this channel
    FString Cid;

    TArray<FMessage> Messages;

    FString ConnectionId;
    FUser User;
    TSharedPtr<FChatApi> Api;
};
