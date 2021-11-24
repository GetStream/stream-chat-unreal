// Copyright Stream.IO, Inc. All Rights Reserved.

#include "Channel/ChannelState.h"

#include "Algo/Transform.h"
#include "Channel/Message.h"
#include "Response/Channel/ChannelStateResponseFieldsDto.h"
#include "User/UserManager.h"

FChannelState::FChannelState() = default;

FChannelState::FChannelState(const FChannelStateResponseFieldsDto& Dto, FUserManager& UserManager)
    : Type{Dto.Channel.Type}
    , Id{Dto.Channel.Id}
    , Name{Dto.Channel.Name}
    , ImageUrl{Dto.Channel.Image}
    , Cid{Dto.Channel.Cid}
    , Config{Dto.Channel.Config}
    , Messages{Convert(Dto, UserManager)}
{
    SetMembers(UserManager, Dto.Members);
}

void FChannelState::Merge(const FChannelStateResponseFieldsDto& Dto, FUserManager& UserManager)
{
    const TArray<FMessage> NewMessages = Convert(Dto, UserManager);
    Messages.Insert(NewMessages, 0);
    SetMembers(UserManager, Dto.Members);
    // TODO Watchers
    // TODO Read
    // TODO Attachment
    // TODO Pinned messages
}

void FChannelState::AddMessage(const FMessage& Message)
{
    // TODO Threads
    // TODO Quoting

    if (const int32 Index = Messages.FindLastByPredicate([&](const FMessage& M) { return M.Id == Message.Id; });
        Index != INDEX_NONE)
    {
        Messages[Index] = Message;
    }
    else
    {
        Messages.Add(Message);
    }
}

TArray<FUserRef> FChannelState::GetOtherMemberUsers() const
{
    constexpr auto NotCurrentUser = [](const FMember& M)
    {
        return !M.User.IsCurrent();
    };
    constexpr auto ToUser = [](const FMember& M)
    {
        return M.User;
    };
    TArray<FUserRef> OtherUsers;
    Algo::TransformIf(Members, OtherUsers, NotCurrentUser, ToUser);
    return OtherUsers;
}

const TArray<FMessage>& FChannelState::GetMessages() const
{
    return Messages;
}

void FChannelState::SetMembers(FUserManager& UserManager, const TArray<FChannelMemberDto>& Dto)
{
    Algo::Transform(Dto, Members, [&](const FChannelMemberDto& MemberDto) { return FMember{UserManager, MemberDto}; });
}

TArray<FMessage> FChannelState::Convert(const FChannelStateResponseFieldsDto& Dto, FUserManager& UserManager)
{
    TArray<FMessage> NewMessages;
    Algo::Transform(
        Dto.Messages,
        NewMessages,
        [&](const FMessageDto& MessageDto) {
            return FMessage{UserManager, MessageDto};
        });
    for (FMessage& Message : NewMessages)
    {
        Message.bIsRead = Dto.Read.ContainsByPredicate(
            [&Message, &UserManager](const FReadDto& Read)
            { return Read.User.Id != UserManager.GetCurrentUser()->Id && Read.LastRead > Message.CreatedAt; });
    }
    return NewMessages;
}
