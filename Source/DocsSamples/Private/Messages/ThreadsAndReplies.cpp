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

    // A reply is recognised by its parent, not by its type: the API reports thread replies as
    // regular messages, so this is the check to use rather than comparing Type
    const bool bIsReply = Reply.IsThreadReply();
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

    // Whatever has been fetched so far, oldest first
    const FMessages& Replies = Channel->GetReplies(ParentMessage);
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
                const FMessage& Parent = Thread.ParentMessage;
                const TArray<FMessage>& Preview = Thread.LatestReplies;
            }
            // Pass NextPage back as the fifth argument to fetch the following page, if it isn't empty
        });

    // Only threads with unread replies
    Client->QueryThreads(FFilter::Equal(TEXT("has_unread"), true));
}

// https://getstream.io/chat/docs/unreal/threads/?language=unreal#getting-a-thread-by-id
void GetThreadById()
{
    // Pass a participant limit to get participants back. Left unset, this endpoint returns none at
    // all rather than defaulting to some number of them.
    Client->GetThread(
        ParentMessage.Id,
        10,    // Reply limit
        25,    // Participant limit
        [](const FChatThread& Thread)
        {
            const int32 ReplyCount = Thread.ReplyCount;
            const TArray<FUserRef>& Participants = Thread.ThreadParticipants;
        });
}
}    // namespace

// https://getstream.io/chat/docs/unreal/threads/?language=unreal#quote-message
void QuoteMessage()
{
    // Not yet supported in the Unreal SDK
}
}    // namespace ThreadsAndReplies
