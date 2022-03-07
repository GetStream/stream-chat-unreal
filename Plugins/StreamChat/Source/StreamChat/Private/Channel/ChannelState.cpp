// Copyright 2022 Stream.IO, Inc. All Rights Reserved.

#include "Channel/ChannelState.h"

#include "Algo/Transform.h"
#include "Channel/Message.h"
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
    const FUserRef CurrentUser = UUserManager::Get()->GetCurrentUser();
    const bool bIsOwnMessage = Message.User == CurrentUser;
    if (bIsOwnMessage)
    {
        return false;
    }
    const bool bMessageFromMutedUser =
        CurrentUser->MutedUsers.ContainsByPredicate([Message](const FMutedUser& MutedUser) { return MutedUser.User == Message.User; });
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
    : WatcherCount{Dto.WatcherCount}, Read{Util::Convert<FRead>(Dto.Read, UserManager)}, Messages{Util::Convert<FMessage>(Dto.Messages, UserManager)}
{
    // TODO Watchers
    // TODO Attachment
    // TODO Pinned messages
}

void FChannelState::Append(const FChannelStateResponseFieldsDto& Dto, UUserManager* UserManager)
{
    FChannelState NewState{Dto, UserManager};
    // Current (old) messages go AFTER new messages
    NewState.Messages.Append(Messages);
    *this = NewState;
}

void FChannelState::AddMessage(const FMessage& Message)
{
    // TODO Threads
    // TODO Quoting
    const int32 Index = Messages.FindLastByPredicate([&](const FMessage& M) { return M.Id == Message.Id; });
    if (Index != INDEX_NONE)
    {
        Messages[Index] = Message;
    }
    else
    {
        Messages.Add(Message);

        if (CountMessageAsUnread(Message))
        {
            if (FRead* CurrentUserRead = GetCurrentUserRead())
            {
                CurrentUserRead->AddUnreadMessage();
            }
        }
    }
}

const TArray<FMessage>& FChannelState::GetMessages() const
{
    return Messages;
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
