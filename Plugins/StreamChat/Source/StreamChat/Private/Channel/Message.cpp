#include "Channel/Message.h"

#include "Api/Dto/Response/MessageDto.h"

FMessage::FMessage(const FMessageDto& Dto)
    : Id(Dto.Id)
    , Text(Dto.Text)
    , Type(Dto.Type)
    , MentionedUsers(Dto.MentionedUsers)
    , bSilent(Dto.bSilent)
    , bShadowed(Dto.bShadowed)
    , User(Dto.User)
    , bPinned(Dto.bPinned)
{
}
