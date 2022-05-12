// Copyright 2022 Stream.IO, Inc. All Rights Reserved.

#include "Reaction/Reaction.h"

#include "Response/Reaction/ReactionDto.h"
#include "User/UserManager.h"

FReaction::FReaction() = default;

FReaction::FReaction(const FReactionDto& Dto, UUserManager* UserManager)
    : Type{Dto.Type}, Score{Dto.Score}, User{UserManager->UpsertUser(Dto.User)}, MessageId{Dto.MessageId}, CreatedAt{Dto.CreatedAt}, UpdatedAt{Dto.UpdatedAt}
{
}

FReaction::FReaction(const FName& Type, const FUserRef& User, const FString& MessageId, const int32 Score)
    : Type{Type}, Score{Score}, User{User}, MessageId{MessageId}
{
}
