// Copyright 2026 Stream.IO, Inc. All Rights Reserved.

#include "Channel/MessageStore.h"

#include "Channel/Message.h"
#include "Misc/AutomationTest.h"

#if WITH_DEV_AUTOMATION_TESTS

namespace
{
FMessage MakeMessage(const FString& Id, const FString& Text = TEXT("hi"))
{
    FMessage Message{Text};
    Message.Id = Id;
    return Message;
}

FMessage MakeReply(const FString& Id, const FString& ParentId, const bool bShowInChannel = false)
{
    FMessage Reply = MakeMessage(Id, TEXT("a reply"));
    Reply.ParentId = ParentId;
    Reply.bShowInChannel = bShowInChannel;
    // Type is left as Regular on purpose: that is what the API reports for a reply, so the store has
    // to recognise one by its parent alone
    return Reply;
}
}    // namespace

BEGIN_DEFINE_SPEC(FMessageStoreSpec, "StreamChat.MessageStore", EAutomationTestFlags::ProductFilter | EAutomationTestFlags_ApplicationContextMask)
END_DEFINE_SPEC(FMessageStoreSpec)

void FMessageStoreSpec::Define()
{
    Describe(
        "Channel messages",
        [this]
        {
            It("should hold messages which are not replies",
               [this]
               {
                   FMessageStore Store;
                   TestTrue("Added", Store.AddMessage(MakeMessage(TEXT("a"))));
                   TestTrue("Not added twice", !Store.AddMessage(MakeMessage(TEXT("a"), TEXT("edited"))));

                   TestEqual("One message", Store.GetMessages().Num(), 1);
                   TestEqual("Message was updated in place", Store.Last().Text, TEXT("edited"));
               });

            It("should follow the channel list when asked for the next message",
               [this]
               {
                   FMessageStore Store;
                   Store.AddMessage(MakeMessage(TEXT("a")));
                   Store.AddMessage(MakeMessage(TEXT("b")));

                   const TSharedPtr<FMessage> Next = Store.Next(MakeMessage(TEXT("a")));
                   TestTrue("Next found", Next.IsValid());
                   if (Next)
                   {
                       TestEqual("Next is the following channel message", Next->Id, TEXT("b"));
                   }
                   TestFalse("Nothing follows the last message", Store.Next(MakeMessage(TEXT("b"))).IsValid());
               });
        });

    Describe(
        "Thread replies",
        [this]
        {
            It("should keep a reply out of the channel message list",
               [this]
               {
                   FMessageStore Store;
                   Store.AddMessage(MakeMessage(TEXT("parent")));
                   Store.AddMessage(MakeReply(TEXT("reply"), TEXT("parent")));

                   TestEqual("Channel list holds only the parent", Store.GetMessages().Num(), 1);
                   TestEqual("Channel list holds the parent", Store.GetMessages()[0]->Id, TEXT("parent"));
                   TestEqual("Thread holds the reply", Store.GetReplies(TEXT("parent")).Num(), 1);
                   TestEqual("Thread holds the reply", Store.GetReplies(TEXT("parent"))[0]->Id, TEXT("reply"));
               });

            It("should put a reply sent with show_in_channel in both lists",
               [this]
               {
                   FMessageStore Store;
                   Store.AddMessage(MakeMessage(TEXT("parent")));
                   Store.AddMessage(MakeReply(TEXT("reply"), TEXT("parent"), true));

                   TestEqual("Channel list holds parent and reply", Store.GetMessages().Num(), 2);
                   TestEqual("Thread holds the reply", Store.GetReplies(TEXT("parent")).Num(), 1);
                   // Both lists point at one message, so an edit through either is seen by both
                   TestTrue("Both lists share one message", Store.GetMessages()[1] == Store.GetReplies(TEXT("parent"))[0]);
               });

            It("should bump the parent's reply count for each new reply only",
               [this]
               {
                   FMessageStore Store;
                   Store.AddMessage(MakeMessage(TEXT("parent")));
                   TestEqual("Parent starts with no replies", Store.Last().ReplyCount, 0);

                   TestTrue("Reply added", Store.AddMessage(MakeReply(TEXT("reply-1"), TEXT("parent"))));
                   TestEqual("Parent has one reply", Store.Last().ReplyCount, 1);

                   TestTrue("Second reply added", Store.AddMessage(MakeReply(TEXT("reply-2"), TEXT("parent"))));
                   TestEqual("Parent has two replies", Store.Last().ReplyCount, 2);

                   // The socket echoes back a reply we already added optimistically ourselves
                   TestFalse("Known reply not added again", Store.AddMessage(MakeReply(TEXT("reply-1"), TEXT("parent"))));
                   TestEqual("Parent still has two replies", Store.Last().ReplyCount, 2);
                   TestEqual("Thread holds both replies", Store.GetReplies(TEXT("parent")).Num(), 2);
               });

            It("should track a thread whose parent is not held locally",
               [this]
               {
                   FMessageStore Store;
                   TestTrue("Reply added", Store.AddMessage(MakeReply(TEXT("reply"), TEXT("elsewhere"))));

                   TestTrue("Channel list untouched", Store.IsEmpty());
                   TestEqual("Thread holds the reply", Store.GetReplies(TEXT("elsewhere")).Num(), 1);
               });

            It("should pick a reply up into the channel list when an edit turns show_in_channel on",
               [this]
               {
                   FMessageStore Store;
                   Store.AddMessage(MakeMessage(TEXT("parent")));
                   Store.AddMessage(MakeReply(TEXT("reply"), TEXT("parent")));
                   TestEqual("Reply not in the channel list yet", Store.GetMessages().Num(), 1);

                   Store.AddMessage(MakeReply(TEXT("reply"), TEXT("parent"), true));
                   TestEqual("Reply now in the channel list", Store.GetMessages().Num(), 2);
                   TestEqual("Reply not duplicated in its thread", Store.GetReplies(TEXT("parent")).Num(), 1);
                   TestEqual("Parent's reply count did not double count", Store.GetMessages()[0]->ReplyCount, 1);
               });

            It("should follow the thread when asked for the reply after a reply",
               [this]
               {
                   FMessageStore Store;
                   Store.AddMessage(MakeMessage(TEXT("parent")));
                   Store.AddMessage(MakeReply(TEXT("reply-1"), TEXT("parent")));
                   Store.AddMessage(MakeReply(TEXT("reply-2"), TEXT("parent")));
                   Store.AddMessage(MakeMessage(TEXT("later-channel-message")));

                   const TSharedPtr<FMessage> Next = Store.Next(MakeReply(TEXT("reply-1"), TEXT("parent")));
                   TestTrue("Next found", Next.IsValid());
                   if (Next)
                   {
                       TestEqual("Next is the following reply, not a channel message", Next->Id, TEXT("reply-2"));
                   }
                   TestFalse("Nothing follows the last reply", Store.Next(MakeReply(TEXT("reply-2"), TEXT("parent"))).IsValid());
               });

            It("should return an empty list for a thread it knows nothing about",
               [this]
               {
                   const FMessageStore Store;
                   TestEqual("No replies", Store.GetReplies(TEXT("unknown")).Num(), 0);
               });

            It("should carry replies over when another store is merged in",
               [this]
               {
                   // What FChannelState::Append does on every channel pagination: build a store from
                   // the new page, then fold the currently held one into it.
                   FMessageStore Held;
                   Held.AddMessage(MakeMessage(TEXT("parent")));
                   Held.AddMessage(MakeReply(TEXT("reply"), TEXT("parent")));

                   FMessageStore NewPage;
                   NewPage.AddMessage(MakeMessage(TEXT("older")));
                   NewPage.Append(Held);

                   TestEqual("Channel messages merged", NewPage.GetMessages().Num(), 2);
                   TestEqual("Replies survived the merge", NewPage.GetReplies(TEXT("parent")).Num(), 1);
               });
        });
}

#endif
