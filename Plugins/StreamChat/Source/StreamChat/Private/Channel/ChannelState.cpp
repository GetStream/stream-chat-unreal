#include "Channel/ChannelState.h"

#include "Channel/Message.h"
#include "Response/Channel/ChannelStateResponseFieldsDto.h"
#include "Util.h"

FChannelState::FChannelState(const FChannelStateResponseFieldsDto& Dto, const FString& CurrentUserId)
    : Type{Dto.Channel.Type}
    , Id{Dto.Channel.Id}
    , Members{Dto.Members}
    , Name{Dto.Channel.Name}
    , ImageUrl{Dto.Channel.Image}
    , Cid{Dto.Channel.Cid}
    , Config{Dto.Channel.Config}
    , Messages{Convert(Dto, CurrentUserId)}
{
}

void FChannelState::Merge(const FChannelStateResponseFieldsDto& Dto, const FString& CurrentUserId)
{
    const TArray<FMessage> NewMessages = Convert(Dto, CurrentUserId);
    Messages.Insert(NewMessages, 0);
    // TODO Watchers
    Members = Util::Convert<FMember>(Dto.Members);
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

TArray<FMessage> FChannelState::Convert(const FChannelStateResponseFieldsDto& Dto, const FString& CurrentUserId)
{
    TArray<FMessage> NewMessages = Util::Convert<FMessage>(Dto.Messages);
    for (FMessage& Message : NewMessages)
    {
        Message.bIsRead = Dto.Read.ContainsByPredicate(
            [&Message, &CurrentUserId](const FReadDto& Read)
            { return Read.User.Id != CurrentUserId && Read.LastRead > Message.CreatedAt; });
    }
    return NewMessages;
}
