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
    , Reactions{FReactionGroup::CollectReactions(
          Dto.ReactionCounts,
          Dto.ReactionScores,
          Dto.LatestReactions,
          Dto.OwnReactions)}
{
}

FMessage::FMessage(const FMessageRequestDto& Dto, const FUser& SendingUser)
    : Id{Dto.Id}
    , Text{Dto.Text}
    , State{EMessageSendState::Sending}    // Assume request dto => sending
    , User{SendingUser}
    , Type(EMessageType::Regular)
{
}

FMessage::operator FMessageRequestDto() const
{
    return FMessageRequestDto{{}, {}, Id, {}, FReactionGroup::GetScores(Reactions), false, Text};
}
