// Copyright 2026 Stream.IO, Inc. All Rights Reserved.

#include "Channel/ChatChannel.h"
#include "CoreMinimal.h"
#include "StreamChatClientComponent.h"

namespace ThreadsAndReplies
{
UChatChannel* Channel;
FMessage ParentMessage;

// https://getstream.io/chat/docs/unreal/threads/?language=unreal
void ThreadsAndReplies()
{
    // Reply to a message, starting a thread if it doesn't have one yet
    Channel->SendReply(FMessage{TEXT("Hello world")}, ParentMessage);

    // Reply, and also show the reply in the main channel message list
    Channel->SendReply(FMessage{TEXT("Hello world")}, ParentMessage, true);

    // Or set the fields on the message yourself and send it as normal
    FMessage Reply{TEXT("Hello world")};
    Reply.ParentId = ParentMessage.Id;
    Reply.bShowInChannel = true;
    Channel->SendMessage(Reply);
}

// https://getstream.io/chat/docs/unreal/threads/?language=unreal#thread-pagination
void ThreadPagination()
{
    // Fetch the most recent 20 replies of a thread. A channel query returns the channel's history
    // and not its threads, so a thread's replies are fetched once, when the user opens it.
    Channel->QueryReplies(
        ParentMessage,
        20,
        [](const TArray<FMessage>& Replies)
        {
            // The replies are also kept on the channel, at Channel->GetReplies(ParentMessage)
        });

    // Then page further back as the user scrolls up the thread
    Channel->QueryAdditionalReplies(ParentMessage, 20);
}

namespace
{
UStreamChatClientComponent* Client;

// https://getstream.io/chat/docs/unreal/threads/?language=unreal#thread-list
void ThreadList()
{
    // List the threads the current user takes part in, across all of their channels, unread first
    Client->QueryThreads(
        {},    // Filter
        {},    // Sort options
        10,    // Threads per page
        2,     // Replies to preview per thread
        {},    // Page token from a previous response
        [](const TArray<FChatThread>& Threads, const FString& NextPage)
        {
            for (const FChatThread& Thread : Threads)
            {
                const int32 Unread = Thread.UnreadCount();
            }
            // Pass NextPage back as the fifth argument to fetch the following page, if it isn't empty
        });

    // Only threads with unread replies
    Client->QueryThreads(FFilter::Equal(TEXT("has_unread"), true));
}
}    // namespace

// https://getstream.io/chat/docs/unreal/threads/?language=unreal#quote-message
void QuoteMessage()
{
    // Not yet supported in the Unreal SDK
}
}    // namespace ThreadsAndReplies
