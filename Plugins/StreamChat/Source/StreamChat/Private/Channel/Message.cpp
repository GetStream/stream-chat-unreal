#include "Channel/Message.h"

#include "Request/Message/MessageRequestDto.h"
#include "Response/Message/MessageDto.h"
#include "User/UserManager.h"

FMessage::FMessage(FUserManager& UserManager, const FMessageDto& Dto)
    : Id{Dto.Id}
    , Text{Dto.Text}
    , State{EMessageSendState::Sent}    // Assume response dto => sent
    , User{UserManager.UpsertUser(Dto.User)}
    , Type{static_cast<EMessageType>(Dto.Type)}
    , MentionedUsers{UserManager.UpsertUsers(Dto.MentionedUsers)}
    , CreatedAt{Dto.CreatedAt}
    , UpdatedAt{Dto.UpdatedAt}
    , DeletedAt{Dto.DeletedAt}
    , Reactions{FReactions::CollectReactions(
          UserManager,
          Dto.ReactionCounts,
          Dto.ReactionScores,
          Dto.LatestReactions,
          Dto.OwnReactions)}
    , bIsRead(false)
{
}

FMessage::FMessage(const FMessageRequestDto& Dto, const FUserRef& SendingUser)
    : Id{Dto.Id}
    , Text{Dto.Text}
    , State{EMessageSendState::Sending}    // Assume request dto => sending
    , User{SendingUser}
    , Type(EMessageType::Regular)
    , CreatedAt{FDateTime::UtcNow()}
    , UpdatedAt{FDateTime::UtcNow()}
    , bIsRead(false)
{
}

FMessage::operator FMessageRequestDto() const
{
    return FMessageRequestDto{{}, {}, Id, {}, Reactions.GetScores(), false, Text};
}
