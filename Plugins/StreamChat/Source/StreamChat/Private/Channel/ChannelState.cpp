// Copyright 2021 Stream.IO, Inc. All Rights Reserved.

#include "Channel/ChannelState.h"

#include "Algo/Transform.h"
#include "Channel/Message.h"
#include "Response/Channel/ChannelStateResponseFieldsDto.h"
#include "User/UserManager.h"
#include "Util.h"

FChannelState::FChannelState() = default;

FChannelState::FChannelState(const FChannelStateResponseFieldsDto& Dto, UUserManager* UserManager)
    : WatcherCount{Dto.WatcherCount}, Read{Util::Convert<FRead>(Dto.Read, UserManager)}, Messages{Convert(Dto, UserManager)}
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

    if (const int32 Index = Messages.FindLastByPredicate([&](const FMessage& M) { return M.Id == Message.Id; }); Index != INDEX_NONE)
    {
        Messages[Index] = Message;
    }
    else
    {
        Messages.Add(Message);
    }
}

const TArray<FMessage>& FChannelState::GetMessages() const
{
    return Messages;
}

int32 FChannelState::UnreadCount() const
{
    if (const FRead* CurrentUserRead = Read.FindByPredicate([](const FRead& R) { return R.User.IsCurrent(); }))
    {
        return CurrentUserRead->UnreadMessages;
    }
    return 0;
}

TArray<FMessage> FChannelState::Convert(const FChannelStateResponseFieldsDto& Dto, UUserManager* UserManager)
{
    TArray<FMessage> NewMessages = Util::Convert<FMessage>(Dto.Messages, UserManager);
    for (FMessage& Message : NewMessages)
    {
        Message.bIsRead =
            Dto.Read.ContainsByPredicate([&Message, &UserManager](const FReadDto& ReadDto)
                                         { return ReadDto.User.Id != UserManager->GetCurrentUser()->Id && ReadDto.LastRead > Message.CreatedAt; });
    }
    return NewMessages;
}
