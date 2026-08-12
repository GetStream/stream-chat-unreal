// Copyright 2026 Stream.IO, Inc. All Rights Reserved.

#include "Event/Channel/MessageDeletedEvent.h"
#include "Event/Channel/MessageNewEvent.h"
#include "Event/Channel/MessageUpdatedEvent.h"
#include "Event/Channel/ReactionNewEvent.h"
#include "Event/Client/HealthCheckEvent.h"
#include "Misc/AutomationTest.h"
#include "StreamJson.h"

#if WITH_DEV_AUTOMATION_TESTS

/**
 * The payloads below are the shapes the WebSocket really sends, trimmed of the fields the SDK has no
 * struct member for. They are deserialized the same way FChatSocketEvents does it, so a property
 * whose name stops converting to the key Stream uses fails here rather than going quietly missing at
 * runtime: an event with an unreadable field still deserializes, it just arrives empty.
 */
BEGIN_DEFINE_SPEC(FChatEventSpec, "StreamChat.ChatEvent", EAutomationTestFlags::ProductFilter | EAutomationTestFlags_ApplicationContextMask)
END_DEFINE_SPEC(FChatEventSpec)

void FChatEventSpec::Define()
{
    Describe(
        "message.new",
        [this]
        {
            It("should read the whole payload",
               [this]
               {
                   const FString Json = R"(
{
    "type": "message.new",
    "cid": "messaging:fun",
    "channel_id": "fun",
    "channel_type": "messaging",
    "created_at": "2026-08-12T09:30:00.123Z",
    "team": "red",
    "watcher_count": 7,
    "user": {
        "id": "author",
        "name": "Author",
        "online": true
    },
    "message": {
        "id": "message-1",
        "cid": "messaging:fun",
        "text": "hello",
        "type": "regular",
        "user": {"id": "author"}
    }
}
)";
                   const FMessageNewEvent Event = Json::Deserialize<FMessageNewEvent>(Json);
                   TestEqual("Type", Event.Type, FMessageNewEvent::StaticType());
                   TestEqual("Cid", Event.Cid, TEXT("messaging:fun"));
                   TestEqual("Channel ID", Event.ChannelId, TEXT("fun"));
                   TestEqual("Channel type", Event.ChannelType, TEXT("messaging"));
                   TestEqual("Created at", Event.CreatedAt, FDateTime{2026, 8, 12, 9, 30, 0, 123});
                   TestEqual("Team", Event.Team, TEXT("red"));
                   TestEqual("Watcher count", Event.WatcherCount, 7u);
                   TestEqual("User", Event.User.Id, TEXT("author"));
                   TestTrue("User is online", Event.User.bOnline);
                   TestEqual("Message", Event.Message.Id, TEXT("message-1"));
                   TestEqual("Message text", Event.Message.Text, TEXT("hello"));
               });

            It("should read the participants of a thread reply",
               [this]
               {
                   const FString Json = R"(
{
    "type": "message.new",
    "cid": "messaging:fun",
    "thread_participants": [{"id": "author"}, {"id": "replier"}],
    "message": {"id": "reply-1", "parent_id": "message-1", "show_in_channel": true}
}
)";
                   const FMessageNewEvent Event = Json::Deserialize<FMessageNewEvent>(Json);
                   TestEqual("Thread participants", Event.ThreadParticipants.Num(), 2);
                   if (Event.ThreadParticipants.Num() == 2)
                   {
                       TestEqual("First participant", Event.ThreadParticipants[0].Id, TEXT("author"));
                       TestEqual("Second participant", Event.ThreadParticipants[1].Id, TEXT("replier"));
                   }
                   TestEqual("Parent", Event.Message.ParentId, TEXT("message-1"));
                   TestTrue("Shown in channel", Event.Message.bShowInChannel);
               });

            It("should leave the watcher count at its sentinel when the API sent none",
               [this]
               {
                   // A count of zero is a real answer, so it must be distinguishable from no answer
                   const FMessageNewEvent Event = Json::Deserialize<FMessageNewEvent>(TEXT(R"({"type": "message.new"})"));
                   TestEqual("Watcher count", Event.WatcherCount, TNumericLimits<uint32>::Max());
                   TestEqual("No team", Event.Team, TEXT(""));
               });
        });

    It("should read a message.updated payload",
       [this]
       {
           const FString Json = R"(
{
    "type": "message.updated",
    "cid": "messaging:fun",
    "channel_id": "fun",
    "channel_type": "messaging",
    "team": "red",
    "user": {"id": "author"},
    "message": {
        "id": "message-1",
        "text": "hello again",
        "updated_at": "2026-08-12T09:31:00Z"
    }
}
)";
           const FMessageUpdatedEvent Event = Json::Deserialize<FMessageUpdatedEvent>(Json);
           TestEqual("Type", Event.Type, FMessageUpdatedEvent::StaticType());
           TestEqual("Team", Event.Team, TEXT("red"));
           TestEqual("User", Event.User.Id, TEXT("author"));
           TestEqual("Message text", Event.Message.Text, TEXT("hello again"));
           TestEqual("Message updated at", Event.Message.UpdatedAt, FDateTime{2026, 8, 12, 9, 31, 0});
       });

    Describe(
        "message.deleted",
        [this]
        {
            It("should read a hard delete",
               [this]
               {
                   const FString Json = R"(
{
    "type": "message.deleted",
    "cid": "messaging:fun",
    "hard_delete": true,
    "thread_participants": [{"id": "author"}],
    "user": {"id": "moderator"},
    "message": {"id": "message-1", "type": "deleted"}
}
)";
                   const FMessageDeletedEvent Event = Json::Deserialize<FMessageDeletedEvent>(Json);
                   TestEqual("Type", Event.Type, FMessageDeletedEvent::StaticType());
                   TestTrue("Hard delete", Event.bHardDelete);
                   // Not the author: anybody with the right to moderate can delete a message
                   TestEqual("User", Event.User.Id, TEXT("moderator"));
                   TestEqual("Thread participants", Event.ThreadParticipants.Num(), 1);
                   TestEqual("Message type", Event.Message.Type, EMessageTypeDto::Deleted);
               });

            It("should treat a delete with no hard_delete key as a soft one",
               [this]
               {
                   const FString Json = R"({"type": "message.deleted", "message": {"id": "message-1"}})";
                   const FMessageDeletedEvent Event = Json::Deserialize<FMessageDeletedEvent>(Json);
                   TestFalse("Soft delete", Event.bHardDelete);
               });
        });

    It("should read a reaction.new payload",
       [this]
       {
           // The message arrives in full with its counts already updated, so a client can replace
           // its copy instead of applying the reaction itself
           const FString Json = R"(
{
    "type": "reaction.new",
    "cid": "messaging:fun",
    "channel_id": "fun",
    "channel_type": "messaging",
    "team": "red",
    "user": {"id": "reactor"},
    "reaction": {
        "message_id": "message-1",
        "type": "like",
        "score": 3,
        "user_id": "reactor",
        "created_at": "2026-08-12T09:32:00Z"
    },
    "message": {
        "id": "message-1",
        "reaction_counts": {"like": 1},
        "reaction_scores": {"like": 3}
    }
}
)";
           const FReactionNewEvent Event = Json::Deserialize<FReactionNewEvent>(Json);
           TestEqual("Type", Event.Type, FReactionNewEvent::StaticType());
           TestEqual("Team", Event.Team, TEXT("red"));
           TestEqual("User", Event.User.Id, TEXT("reactor"));
           TestEqual("Reaction type", Event.Reaction.Type, FName{TEXT("like")});
           TestEqual("Reaction score", Event.Reaction.Score, 3);
           TestEqual("Reaction message", Event.Reaction.MessageId, TEXT("message-1"));
           TestEqual("Reacting user", Event.Reaction.UserId, TEXT("reactor"));
           TestEqual("Message carries the new score", Event.Message.ReactionScores.FindRef(FName{TEXT("like")}), 3);
       });

    Describe(
        "health.check",
        [this]
        {
            It("should read the connected user of the first health check",
               [this]
               {
                   const FString Json = R"(
{
    "type": "health.check",
    "connection_id": "3ec4c9fd-3d5e-4f4c-9d1a-1b2c3d4e5f60",
    "created_at": "2026-08-12T09:33:00Z",
    "me": {
        "id": "me",
        "role": "user",
        "total_unread_count": 4,
        "unread_channels": 2
    }
}
)";
                   const FHealthCheckEvent Event = Json::Deserialize<FHealthCheckEvent>(Json);
                   TestEqual("Type", Event.Type, FHealthCheckEvent::StaticType());
                   TestEqual("Connection ID", Event.ConnectionId, TEXT("3ec4c9fd-3d5e-4f4c-9d1a-1b2c3d4e5f60"));
                   TestEqual("Me", Event.Me.Id, TEXT("me"));
                   TestEqual("Total unread", Event.Me.TotalUnreadCount, 4);
                   TestEqual("Unread channels", Event.Me.UnreadChannels, 2);
                   TestEqual("No channel", Event.Cid, TEXT(""));
               });

            It("should read the channel of a watched channel's health check",
               [this]
               {
                   const FString Json = R"({"type": "health.check", "connection_id": "abc", "cid": "messaging:fun"})";
                   const FHealthCheckEvent Event = Json::Deserialize<FHealthCheckEvent>(Json);
                   TestEqual("Cid", Event.Cid, TEXT("messaging:fun"));
                   // Only the first health check of a connection carries the user
                   TestEqual("No user", Event.Me.Id, TEXT(""));
               });
        });
}

#endif    // WITH_DEV_AUTOMATION_TESTS
