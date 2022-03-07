// Copyright 2022 Stream.IO, Inc. All Rights Reserved.

#include "Reaction/Reactions.h"

#include "Reaction/Reaction.h"
#include "Reaction/ReactionGroup.h"
#include "Response/Reaction/ReactionDto.h"
#include "User/UserRef.h"

FReactions FReactions::CollectReactions(
    UUserManager* UserManager,
    const TMap<FName, int32>& ReactionCounts,
    const TMap<FName, int32>& ReactionScores,
    const TArray<FReactionDto>& LatestReactions,
    const TArray<FReactionDto>& OwnReactions)
{
    FReactions Result;
    // Assume ReactionCounts has all reaction types
    for (const auto& Count : ReactionCounts)
    {
        const int32 Score = ReactionScores.FindRef(Count.Key);
        Result.ReactionGroups.Emplace(Count.Key, FReactionGroup{Count.Key, Count.Value, Score});
    }

    for (const FReactionDto& Latest : LatestReactions)
    {
        Result.ReactionGroups.FindOrAdd(Latest.Type).LatestReactions.Add(FReaction{Latest, UserManager});
    }
    for (const FReactionDto& Own : OwnReactions)
    {
        Result.ReactionGroups.FindOrAdd(Own.Type).OwnReaction.Emplace(FReaction{Own, UserManager});
    }
    return Result;
}

TMap<FName, int32> FReactions::GetScores() const
{
    TMap<FName, int32> Result;
    for (const auto& Group : ReactionGroups)
    {
        Result.Emplace(Group.Key, Group.Value.TotalScore);
    }
    return Result;
}

void FReactions::AddReaction(const FReaction& Reaction)
{
    auto& Group = ReactionGroups.FindOrAdd(Reaction.Type);
    // Latest reactions are ordered reverse-chronologically
    Group.LatestReactions.Insert(Reaction, 0);
    ++Group.Count;
    ++Group.TotalScore;
    if (Group.Type == NAME_None)
    {
        // First reaction of this type
        Group.Type = Reaction.Type;
        ReactionGroups.KeySort(FNameLexicalLess());
    }
}

void FReactions::RemoveReactionWhere(const TFunctionRef<bool(const FReaction&)> Predicate)
{
    // Use iterator for removal during iteration
    for (TMap<FName, FReactionGroup>::TIterator It(ReactionGroups); It; ++It)
    {
        const int32 Count = It->Value.LatestReactions.RemoveAll(Predicate);
        It->Value.Count -= Count;
        It->Value.TotalScore -= Count;

        check(It->Value.Count >= 0);

        if (It->Value.Count == 0)
        {
            It.RemoveCurrent();
        }
    }
}

TOptional<FReaction> FReactions::GetOwnReaction(const FName& ReactionType) const
{
    if (const FReactionGroup* Group = ReactionGroups.Find(ReactionType))
    {
        return Group->OwnReaction;
    }
    return {};
}

const TMap<FName, FReactionGroup>& FReactions::GetReactionGroups() const
{
    return ReactionGroups;
}

bool FReactions::IsEmpty() const
{
    return ReactionGroups.Num() == 0;
}

void FReactions::UpdateOwnReactions()
{
    for (auto& Group : ReactionGroups)
    {
        if (const FReaction* OwnReaction = Group.Value.LatestReactions.FindByPredicate([](const FReaction& R) { return R.User.IsCurrent(); }))
        {
            Group.Value.OwnReaction.Emplace(*OwnReaction);
        }
        else
        {
            Group.Value.OwnReaction.Reset();
        }
    }
}

bool UReactionsBlueprintLibrary::HasOwnReaction(const FReactions& Reactions, const FName& ReactionType)
{
    return Reactions.GetOwnReaction(ReactionType).IsSet();
}

bool UReactionsBlueprintLibrary::IsEmpty(const FReactions& Reactions)
{
    return Reactions.IsEmpty();
}
