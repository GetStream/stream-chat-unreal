// Copyright 2026 Stream.IO, Inc. All Rights Reserved.

#include "Channel/MessageStore.h"

#include "Channel/Message.h"
#include "User/UserManager.h"

namespace
{
/// Find a message by ID, or INDEX_NONE
int32 IndexOfMessage(const FMessages& List, const FString& Id)
{
    return List.FindLastByPredicate([&Id](const FMessageRef& M) { return M->Id == Id; });
}

/// Insert or update a message in a list, matching on ID
/// Updates happen in place so every list holding the same FMessage sees them
bool UpsertMessage(FMessages& List, const FMessage& NewMessage)
{
    const int32 Index = IndexOfMessage(List, NewMessage.Id);
    if (Index != INDEX_NONE)
    {
        *List[Index] = NewMessage;
        return false;
    }

    List.Add(MakeShared<FMessage>(NewMessage));
    return true;
}

const FMessages EmptyMessages;
}    // namespace

void FMessageStore::Append(const FMessages& NewMessages)
{
    // TODO check new messages aren't already present
    // Current (old) messages go AFTER new messages
    Messages.Append(NewMessages);
}

void FMessageStore::Append(const FMessageStore& Store)
{
    Append(Store.Messages);

    // Replies held by the other store are carried over. A channel query returns channel history and
    // not thread history, so replies fetched for a thread would otherwise be dropped every time the
    // channel paginated. A reply both stores know about is kept once.
    for (const auto& Thread : Store.Threads)
    {
        FMessages& Replies = Threads.FindOrAdd(Thread.Key);
        for (const FMessageRef& Reply : Thread.Value)
        {
            if (IndexOfMessage(Replies, Reply->Id) == INDEX_NONE)
            {
                Replies.Add(Reply);
            }
        }
    }
}

void FMessageStore::Append(const TArray<FMessageDto> Dtos, UUserManager* UserManager)
{
    FMessages NewMessages;
    NewMessages.Reserve(Dtos.Num());
    for (const FMessageDto& Dto : Dtos)
    {
        const FMessageRef Message = MakeShared<FMessage>(Dto, UserManager);
        if (Message->IsThreadReply())
        {
            // Channel state only carries a reply when it was sent with show_in_channel, but it is
            // still part of its thread, so it is registered there as the same FMessage.
            Threads.FindOrAdd(Message->ParentId).Add(Message);
            if (!Message->bShowInChannel)
            {
                continue;
            }
        }
        NewMessages.Add(Message);
    }
    Append(NewMessages);
}

void FMessageStore::AppendReplies(const FString& ParentId, const TArray<FMessageDto>& Dtos, UUserManager* UserManager)
{
    FMessages& Replies = Threads.FindOrAdd(ParentId);

    // Current (previously fetched) replies go AFTER the newly fetched ones, same as the channel list,
    // so paginating backwards through a thread keeps it oldest-first. A reply we already hold is
    // updated in place rather than replaced, so the channel list and any list view showing it stay
    // pointed at the same object. Opening a thread twice refetches its newest page, so overlapping
    // pages have to merge rather than duplicate.
    FMessages Merged;
    Merged.Reserve(Dtos.Num() + Replies.Num());
    for (const FMessageDto& Dto : Dtos)
    {
        const FMessage NewReply{Dto, UserManager};
        const int32 Index = IndexOfMessage(Replies, NewReply.Id);
        if (Index == INDEX_NONE)
        {
            Merged.Add(MakeShared<FMessage>(NewReply));
        }
        else
        {
            *Replies[Index] = NewReply;
            Merged.Add(Replies[Index]);
        }
    }
    for (const FMessageRef& Held : Replies)
    {
        if (IndexOfMessage(Merged, Held->Id) == INDEX_NONE)
        {
            Merged.Add(Held);
        }
    }
    Replies = MoveTemp(Merged);

    for (const FMessageRef& Reply : Replies)
    {
        if (Reply->bShowInChannel)
        {
            ShowReplyInChannel(Reply);
        }
    }
}

bool FMessageStore::AddMessage(const FMessage& NewMessage)
{
    TRACE_CPUPROFILER_EVENT_SCOPE(TEXT("FMessageStore::AddMessage"))
    // TODO Quoting
    if (NewMessage.IsThreadReply())
    {
        return AddReply(NewMessage);
    }

    return UpsertMessage(Messages, NewMessage);
}

bool FMessageStore::AddMessage(const FMessageDto& Dto, UUserManager* UserManager)
{
    return AddMessage(FMessage{Dto, UserManager});
}

bool FMessageStore::AddReply(const FMessage& NewReply)
{
    FMessages& Replies = Threads.FindOrAdd(NewReply.ParentId);
    const int32 Index = IndexOfMessage(Replies, NewReply.Id);
    const bool bAdded = Index == INDEX_NONE;

    const FMessageRef Reply = bAdded ? MakeShared<FMessage>(NewReply) : Replies[Index];
    if (bAdded)
    {
        Replies.Add(Reply);
        IncrementReplyCount(NewReply.ParentId);
    }
    else
    {
        *Reply = NewReply;
    }

    // An edit can also turn show_in_channel on, so this is checked on update as well as on insert
    if (NewReply.bShowInChannel)
    {
        ShowReplyInChannel(Reply);
    }
    return bAdded;
}

void FMessageStore::ShowReplyInChannel(const FMessageRef& Reply)
{
    const int32 Index = IndexOfMessage(Messages, Reply->Id);
    if (Index == INDEX_NONE)
    {
        // Both lists share one FMessage, so an update through either is seen by both
        Messages.Add(Reply);
    }
    else if (Messages[Index] != Reply)
    {
        // The channel list already had its own copy, from a channel query. Update it in place rather
        // than swapping it out, because a list view may be holding on to that exact object.
        *Messages[Index] = *Reply;
    }
}

void FMessageStore::IncrementReplyCount(const FString& ParentId)
{
    // Keeps the parent's reply count moving for a reply we learned about one at a time, over the
    // socket or from our own send. A parent refetched from the API brings the authoritative count.
    const int32 Index = IndexOfMessage(Messages, ParentId);
    if (Index != INDEX_NONE)
    {
        Messages[Index]->ReplyCount++;
    }
}

bool FMessageStore::IsEmpty() const
{
    return Messages.Num() == 0;
}

const FMessage& FMessageStore::First()
{
    return *Messages[0];
}

const FMessage& FMessageStore::Last()
{
    return *Messages.Last();
}

TSharedPtr<FMessage> FMessageStore::Next(const FMessage& QueryMessage) const
{
    TRACE_CPUPROFILER_EVENT_SCOPE(TEXT("FMessageStore::Next"))

    // A reply's neighbour is the next reply in its thread, not whatever follows it in the channel
    const FMessages& List = QueryMessage.IsThreadReply() ? GetReplies(QueryMessage.ParentId) : Messages;

    // TODO perf
    const int32 Index = List.IndexOfByPredicate([&](const FMessageRef& M) { return M->Id == QueryMessage.Id; });
    if (Index == INDEX_NONE || Index == List.Num() - 1)
    {
        return {};
    }
    return List[Index + 1];
}

FMessages FMessageStore::FilterRecent(const FTimespan& Since) const
{
    return Messages.FilterByPredicate([&](const FMessageRef& M) { return FDateTime::UtcNow() - M->CreatedAt < Since; });
}

const FMessages& FMessageStore::GetMessages() const
{
    return Messages;
}

const FMessages& FMessageStore::GetReplies(const FString& ParentId) const
{
    if (const FMessages* Replies = Threads.Find(ParentId))
    {
        return *Replies;
    }
    return EmptyMessages;
}
