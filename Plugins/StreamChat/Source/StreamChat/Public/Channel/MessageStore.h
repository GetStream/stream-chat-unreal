// Copyright 2022 Stream.IO, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Message.h"

#include "MessageStore.generated.h"

using FMessageRef = TSharedRef<FMessage>;
using FMessages = TArray<FMessageRef>;

/**
 * @brief A store for a list of messages
 * @ingroup StreamChat
 */
USTRUCT(BlueprintType)
struct STREAMCHAT_API FMessageStore
{
    GENERATED_BODY()

    /**
     * @brief Locally upsert a list of messages to this store
     */
    void Append(const FMessageStore&);
    void Append(const TArray<FMessageDto>, UUserManager*);

    /**
     * @brief Locally upsert a message to this store
     * @return Whether message was added (as opposed to updated)
     */
    bool AddMessage(const FMessage&);
    bool AddMessage(const FMessageDto&, UUserManager*);

    bool IsEmpty() const;
    // Must check if store is empty first or this will crash
    const FMessage& First();
    // Must check if store is empty first or this will crash
    const FMessage& Last();

    /// Get message following the given message
    TSharedPtr<FMessage> Next(const FMessage&) const;

    FMessages FilterRecent(const FTimespan& Since) const;

    /// Get all the messages of this channel
    const FMessages& GetMessages() const;

private:
    void Append(const FMessages&);
    FMessages Messages;
};
