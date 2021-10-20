#include "Reaction/ReactionGroup.h"

bool FReactionGroup::HasOwnReaction() const
{
    return OwnReactions.Num() > 0;
}
