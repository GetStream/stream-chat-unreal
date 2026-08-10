// Copyright 2026 Stream.IO, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Message.h"

#include "MessageStore.generated.h"

using FMessageRef = TSharedRef<FMessage>;
using FMessages = TArray<FMessageRef>;

/**
 * @brief A store for a list of messages
 *
 * Holds the channel's own message list, plus the replies of any thread that has been fetched or
 * observed. A thread reply is not part of the channel message list unless it was sent with
 * bShowInChannel, which is how the backend reports it too: querying channel state returns the
 * channel's history, not its threads. Storing them separately is what stops a thread's replies from
 * appearing interleaved with the channel history.
 *
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
     *
     * A thread reply goes into its thread, and additionally into the channel message list if it was
     * sent with bShowInChannel. Adding a reply which wasn't held yet also bumps the reply count of
     * its parent, if the parent is held here.
     *
     * @return Whether message was added (as opposed to updated)
     */
    bool AddMessage(const FMessage&);
    bool AddMessage(const FMessageDto&, UUserManager*);

    /**
     * @brief Locally upsert the replies of a thread, oldest first
     *
     * Replies already held for the thread are kept, and are ordered after the given ones, so
     * paginating backwards through a thread keeps the list oldest-first.
     */
    void AppendReplies(const FString& ParentId, const TArray<FMessageDto>&, UUserManager*);

    bool IsEmpty() const;
    // Must check if store is empty first or this will crash
    const FMessage& First();
    // Must check if store is empty first or this will crash
    const FMessage& Last();

    /// Get message following the given message. Looks within the thread if given a thread reply.
    TSharedPtr<FMessage> Next(const FMessage&) const;

    FMessages FilterRecent(const FTimespan& Since) const;

    /// Get all the messages of this channel. Excludes thread replies not sent with bShowInChannel.
    const FMessages& GetMessages() const;

    /// Get the replies held locally for the given thread, oldest first. Empty if none are held.
    const FMessages& GetReplies(const FString& ParentId) const;

private:
    void Append(const FMessages&);
    /// Register a reply against its parent, sharing one FMessage with the channel list if it is in both
    bool AddReply(const FMessage&);
    /// Make a reply visible in the channel message list too, as bShowInChannel asks
    void ShowReplyInChannel(const FMessageRef&);
    void IncrementReplyCount(const FString& ParentId);

    FMessages Messages;
    /// Replies of each known thread, keyed by the ID of the message which started it
    TMap<FString, FMessages> Threads;
};
