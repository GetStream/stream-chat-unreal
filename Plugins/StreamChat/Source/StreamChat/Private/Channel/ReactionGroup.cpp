#include "Channel/ReactionGroup.h"

#include "Response/ReactionDto.h"
#include "Util.h"

TMap<FName, FReactionGroup> FReactionGroup::CollectReactions(
    const TMap<FName, int32>& ReactionCounts,
    const TMap<FName, int32>& ReactionScores,
    const TArray<FReactionDto>& LatestReactions,
    const TArray<FReactionDto>& OwnReactions)
{
    TMap<FName, FReactionGroup> Result;
    // Assume ReactionCounts has all reaction types
    for (auto [Type, Count] : ReactionCounts)
    {
        const int32 Score = ReactionScores.FindRef(Type);
        Result.Emplace(Type, FReactionGroup{Type, Count, Score});
    }

    for (const FReactionDto& Latest : LatestReactions)
    {
        Result.FindOrAdd(Latest.Type).LatestReactions.Add(Util::Convert<FReaction>(Latest));
    }
    for (const FReactionDto& Own : OwnReactions)
    {
        Result.FindOrAdd(Own.Type).OwnReactions.Add(Util::Convert<FReaction>(Own));
    }
    return Result;
}

TMap<FName, int32> FReactionGroup::GetScores(const TMap<FName, FReactionGroup>& ReactionGroups)
{
    TMap<FName, int32> Result;
    for (auto [Type, Group] : ReactionGroups)
    {
        Result.Emplace(Type, Group.TotalScore);
    }
    return Result;
}
