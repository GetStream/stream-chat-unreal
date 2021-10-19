#include "Channel/Message.h"

#include "Request/MessageRequestDto.h"
#include "Response/MessageDto.h"

FMessage::FMessage(const FMessageDto& Dto)
    : Id{Dto.Id}
    , Text{Dto.Text}
    , State{EMessageSendState::Sent}    // Assume response dto => sent
    , User{Dto.User}
    , Type{static_cast<EMessageType>(Dto.Type)}
    , MentionedUsers{Dto.MentionedUsers}
    , UpdatedAt{Dto.UpdatedAt}
    , DeletedAt{Dto.DeletedAt}
    , ReactionCounts{Dto.ReactionCounts}
    , ReactionScores{Dto.ReactionScores}
    , LatestReactions{Dto.LatestReactions}
    , OwnReactions{Dto.OwnReactions}
{
}

FMessage::FMessage(const FMessageRequestDto& Dto, const FUser& SendingUser)
    : Id{Dto.Id}
    , Text{Dto.Text}
    , State{EMessageSendState::Sending}    // Assume request dto => sending
    , User{SendingUser}
    , Type(EMessageType::Regular)
    , ReactionScores{Dto.ReactionScores}
{
}

FMessage::operator FMessageRequestDto() const
{
    return FMessageRequestDto{{}, {}, Id, {}, ReactionScores, false, Text};
}
