// Copyright 2022 Stream.IO, Inc. All Rights Reserved.

#include "Channel/MessageStore.h"

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
    return AddMessage(MakeShared<FMessage>(NewMessage));
}

bool FMessageStore::AddMessage(const FMessageRef& Message)
{
    TRACE_CPUPROFILER_EVENT_SCOPE(TEXT("FChannelState::AddMessage"))
    // TODO Threads
    // TODO Quoting
    const int32 Index = Messages.FindLastByPredicate([&](const FMessageRef& M) { return M->Id == Message->Id; });
    if (Index != INDEX_NONE)
    {
        Messages[Index] = Message;
        return false;
    }

    Messages.Add(Message);
    return true;
}

bool FMessageStore::AddMessage(const FMessageDto& Dto, UUserManager* UserManager)
{
    return AddMessage(MakeShared<FMessage>(Dto, UserManager));
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

FMessages FMessageStore::FilterRecent(const FTimespan& Since) const
{
    return Messages.FilterByPredicate([&](const FMessageRef& M) { return FDateTime::UtcNow() - M->CreatedAt < Since; });
}

const FMessages& FMessageStore::GetMessages() const
{
    return Messages;
}
