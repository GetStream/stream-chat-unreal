// Copyright Stream.IO, Inc. All Rights Reserved.

#include "Reaction/ReactionGroup.h"

bool UReactionGroupBlueprintLibrary::HasOwnReaction(const FReactionGroup& ReactionGroup)
{
    return ReactionGroup.OwnReaction.IsSet();
}

void UReactionGroupBlueprintLibrary::GetOwnReaction(const FReactionGroup& ReactionGroup, bool& bIsSet, FReaction& Reaction)
{
    bIsSet = ReactionGroup.OwnReaction.IsSet();
    if (bIsSet)
    {
        Reaction = ReactionGroup.OwnReaction.GetValue();
    }
}
