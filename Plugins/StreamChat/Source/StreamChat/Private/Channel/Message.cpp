#include "Channel/Message.h"

#include "Api/Dto/Request/MessageRequestDto.h"
#include "Api/Dto/Response/MessageDto.h"

FMessage::FMessage(const FMessageDto& Dto)
    : Id{Dto.Id}
    , Text{Dto.Text}
    , State{EMessageSendState::Sent}    // Assume response dto => sent
    , Type{Dto.Type}
    , MentionedUsers{Dto.MentionedUsers}
    , User{Dto.User}
    , UpdatedAt{Dto.UpdatedAt}
{
}

FMessage::FMessage(const FMessageRequestDto& Dto, const FUser& SendingUser)
    : Id{Dto.Id}, Text{Dto.Text}, State{EMessageSendState::Sending}, User{SendingUser}
// Assume request dto => sending
{
}
