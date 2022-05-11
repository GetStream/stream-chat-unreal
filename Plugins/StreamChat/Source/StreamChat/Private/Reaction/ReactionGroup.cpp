// Copyright 2022 Stream.IO, Inc. All Rights Reserved.

#include "Reaction/ReactionGroup.h"

#include "User/UserManager.h"

TOptional<FReaction> FReactionGroup::GetOwnReaction(const UUserManager* UserManager) const
{
    const FUserRef Me = UserManager->GetCurrentUser();
    const FReaction* Found = LatestReactions.FindByPredicate([&](auto&& R) { return R.User == Me; });
    return Found ? *Found : TOptional<FReaction>{};
}

bool UReactionGroupBlueprintLibrary::HasOwnReaction(const FReactionGroup& ReactionGroup)
{
    return ReactionGroup.GetOwnReaction(UUserManager::Get()).IsSet();
}

void UReactionGroupBlueprintLibrary::GetOwnReaction(const FReactionGroup& ReactionGroup, bool& bIsSet, FReaction& Reaction)
{
    const TOptional<FReaction> OwnReaction = ReactionGroup.GetOwnReaction(UUserManager::Get());
    bIsSet = OwnReaction.IsSet();
    if (bIsSet)
    {
        Reaction = OwnReaction.GetValue();
    }
}
