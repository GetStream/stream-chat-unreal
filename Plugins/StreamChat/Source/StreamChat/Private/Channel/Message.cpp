#include "Channel/Message.h"

#include "Request/MessageRequestDto.h"
#include "Response/MessageDto.h"

FMessage::FMessage(const FMessageDto& Dto)
    : Id{Dto.Id}
    , Text{Dto.Text}
    , State{EMessageSendState::Sent}    // Assume response dto => sent
    , User{Dto.User}
    , Type{Dto.Type}
    , MentionedUsers{Dto.MentionedUsers}
    , UpdatedAt{Dto.UpdatedAt}
{
}

FMessage::FMessage(const FMessageRequestDto& Dto, const FUser& SendingUser)
    : Id{Dto.Id}
    , Text{Dto.Text}
    , State{EMessageSendState::Sending}    // Assume request dto => sending
    , User{SendingUser}
{
}

FMessage::operator FMessageRequestDto() const
{
    return FMessageRequestDto{Id, Text};
}
