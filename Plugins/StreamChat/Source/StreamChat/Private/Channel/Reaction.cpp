#include "Channel/Reaction.h"

#include "Response/ReactionDto.h"

FReaction::FReaction(const FReactionDto& Dto)
    : Type{Dto.Type}
    , Score{Dto.Score}
    , User{Dto.User}
    , MessageId{Dto.MessageId}
    , CreatedAt{Dto.CreatedAt}
    , UpdatedAt{Dto.UpdatedAt}
{
}
