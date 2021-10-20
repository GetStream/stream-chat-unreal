#include "Reaction/Reaction.h"

#include "Response/Reaction/ReactionDto.h"

FReaction::FReaction(const FReactionDto& Dto)
    : Type{Dto.Type}
    , Score{Dto.Score}
    , User{Dto.User}
    , MessageId{Dto.MessageId}
    , CreatedAt{Dto.CreatedAt}
    , UpdatedAt{Dto.UpdatedAt}
{
}

FReaction::FReaction(const FName& Type, const FUser& User, const FString& MessageId)
    : Type{Type}, User{User}, MessageId{MessageId}
{
}
