#include "Channel/ChannelState.h"

#include "Channel/Message.h"
#include "Response/Channel/ChannelStateResponseFieldsDto.h"

FChannelState::FChannelState(const FChannelStateResponseFieldsDto& Dto)
    : Type{Dto.Channel.Type}
    , Id{Dto.Channel.Id}
    , Members{Dto.Members}
    , Name{Dto.Channel.Name}
    , ImageUrl{Dto.Channel.Image}
    , Cid{Dto.Channel.Cid}
    , Config{Dto.Channel.Config}
    , Messages{Dto.Messages}
{
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
