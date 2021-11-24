// Copyright Stream.IO, Inc. All Rights Reserved.

#include "Reaction/Reaction.h"

#include "Response/Reaction/ReactionDto.h"
#include "User/UserManager.h"

FReaction::FReaction() = default;

FReaction::FReaction(FUserManager& UserManager, const FReactionDto& Dto)
    : Type{Dto.Type}, Score{Dto.Score}, User{UserManager.UpsertUser(Dto.User)}, MessageId{Dto.MessageId}, CreatedAt{Dto.CreatedAt}, UpdatedAt{Dto.UpdatedAt}
{
}

FReaction::FReaction(const FName& Type, const FUserRef& User, const FString& MessageId) : Type{Type}, User{User}, MessageId{MessageId}
{
}
