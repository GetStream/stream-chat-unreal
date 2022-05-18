// Copyright 2022 Stream.IO, Inc. All Rights Reserved.

#include "Channel/MessageStore.h"

#include "Algo/Transform.h"
#include "Channel/Message.h"
#include "User/UserManager.h"

void FMessageStore::Append(const FMessages& NewMessages)
{
    // TODO check new messages aren't already present
    // Current (old) messages go AFTER new messages
    Messages.Append(NewMessages);
}

void FMessageStore::Append(const FMessageStore& Store)
{
    Append(Store.Messages);
}

void FMessageStore::Append(const TArray<FMessageDto> Dtos, UUserManager* UserManager)
{
    FMessages NewMessages;
    Algo::Transform(Dtos, NewMessages, [&](const FMessageDto& Dto) { return MakeShared<FMessage>(Dto, UserManager); });
    Append(NewMessages);
}

bool FMessageStore::AddMessage(const FMessage& NewMessage)
{
    TRACE_CPUPROFILER_EVENT_SCOPE(TEXT("FMessageStore::AddMessage"))
    // TODO Threads
    // TODO Quoting
    const int32 Index = Messages.FindLastByPredicate([&](const FMessageRef& M) { return M->Id == NewMessage.Id; });
    if (Index != INDEX_NONE)
    {
        *Messages[Index] = NewMessage;
        return false;
    }

    Messages.Add(MakeShared<FMessage>(NewMessage));
    return true;
}

bool FMessageStore::AddMessage(const FMessageDto& Dto, UUserManager* UserManager)
{
    return AddMessage(FMessage{Dto, UserManager});
}

bool FMessageStore::IsEmpty() const
{
    return Messages.IsEmpty();
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

    // TODO perf
    const int32 Index = Messages.IndexOfByPredicate([&](const FMessageRef& M) { return M->Id == QueryMessage.Id; });
    if (Index == INDEX_NONE || Index == Messages.Num() - 1)
    {
        return {};
    }
    return Messages[Index + 1];
}

FMessages FMessageStore::FilterRecent(const FTimespan& Since) const
{
    return Messages.FilterByPredicate([&](const FMessageRef& M) { return FDateTime::UtcNow() - M->CreatedAt < Since; });
}

const FMessages& FMessageStore::GetMessages() const
{
    return Messages;
}
