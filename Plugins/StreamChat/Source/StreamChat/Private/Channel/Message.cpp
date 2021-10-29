#include "Channel/Message.h"

#include "Request/Message/MessageRequestDto.h"
#include "Response/Message/MessageDto.h"

FMessage::FMessage(const FMessageDto& Dto)
    : Id{Dto.Id}
    , Text{Dto.Text}
    , State{EMessageSendState::Sent}    // Assume response dto => sent
    , User{Dto.User}
    , Type{static_cast<EMessageType>(Dto.Type)}
    , MentionedUsers{Dto.MentionedUsers}
    , UpdatedAt{Dto.UpdatedAt}
    , DeletedAt{Dto.DeletedAt}
    , Reactions{FReactions::CollectReactions(
          Dto.ReactionCounts,
          Dto.ReactionScores,
          Dto.LatestReactions,
          Dto.OwnReactions)}
    , bIsRead(false)
{
}

FMessage::FMessage(const FMessageRequestDto& Dto, const FUser& SendingUser)
    : Id{Dto.Id}
    , Text{Dto.Text}
    , State{EMessageSendState::Sending}    // Assume request dto => sending
    , User{SendingUser}
    , Type(EMessageType::Regular)
    , bIsRead(false)
{
}

FMessage::operator FMessageRequestDto() const
{
    return FMessageRequestDto{{}, {}, Id, {}, Reactions.GetScores(), false, Text};
}
