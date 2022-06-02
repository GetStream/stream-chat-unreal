// Copyright 2022 Stream.IO, Inc. All Rights Reserved.

#include "Channel/ChannelState.h"

#include "Channel/Message.h"
#include "Moderation/MutedUser.h"
#include "Response/Channel/ChannelStateResponseFieldsDto.h"
#include "User/UserManager.h"
#include "Util.h"

namespace
{
bool CountMessageAsUnread(const FMessage& Message)
{
    if (Message.bIsSilent || Message.bIsShadowed)
    {
        return false;
    }
    if (Message.User.IsCurrent())
    {
        return false;
    }
    const bool bMessageFromMutedUser = UUserManager::Get()->GetCurrentUser().HasMutedUser(Message.User);
    if (bMessageFromMutedUser)
    {
        return false;
    }
    const bool bIsThreadMessage = !Message.ParentId.IsEmpty();
    if (bIsThreadMessage)
    {
        return false;
    }

    return true;
}
}    // namespace

FChannelState::FChannelState() = default;

FChannelState::FChannelState(const FChannelStateResponseFieldsDto& Dto, UUserManager* UserManager)
    : WatcherCount{Dto.WatcherCount}, Watchers{UserManager->UpsertUsers(Dto.Watchers)}, Read{Util::Convert<FRead>(Dto.Read, UserManager)}
{
    Messages.Append(Dto.Messages, UserManager);
    // TODO Attachment
    // TODO Pinned messages
}

void FChannelState::Append(const FChannelStateResponseFieldsDto& Dto, UUserManager* UserManager)
{
    FChannelState NewState{Dto, UserManager};
    NewState.Messages.Append(Messages);
    *this = NewState;
}

void FChannelState::AddMessage(const FMessage& Message)
{
    TRACE_CPUPROFILER_EVENT_SCOPE(TEXT("FChannelState::AddMessage"))
    // TODO Threads
    // TODO Quoting
    if (Messages.AddMessage(Message))
    {
        if (CountMessageAsUnread(Message))
        {
            if (FRead* CurrentUserRead = GetCurrentUserRead())
            {
                CurrentUserRead->AddUnreadMessage();
            }
        }
    }
}

bool FChannelState::IsMessageRead(const FMessage& Message) const
{
    return Read.ContainsByPredicate([&Message](const FRead& R) { return !R.User.IsCurrent() && R.LastRead > Message.CreatedAt; });
}

void FChannelState::MarkRead()
{
    if (FRead* CurrentUserRead = GetCurrentUserRead())
    {
        CurrentUserRead->MarkRead();
    }
}

int32 FChannelState::UnreadCount() const
{
    if (const FRead* CurrentUserRead = GetCurrentUserRead())
    {
        return CurrentUserRead->UnreadMessages;
    }
    return 0;
}

FRead* FChannelState::GetCurrentUserRead()
{
    return Read.FindByPredicate([](const FRead& R) { return R.User.IsCurrent(); });
}

const FRead* FChannelState::GetCurrentUserRead() const
{
    return Read.FindByPredicate([](const FRead& R) { return R.User.IsCurrent(); });
}
