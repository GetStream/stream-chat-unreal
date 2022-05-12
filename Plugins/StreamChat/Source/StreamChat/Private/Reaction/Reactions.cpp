// Copyright 2022 Stream.IO, Inc. All Rights Reserved.

#include "Reaction/Reactions.h"

#include "Reaction/Reaction.h"
#include "Reaction/ReactionGroup.h"
#include "Response/Reaction/ReactionDto.h"
#include "User/UserManager.h"

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
        Result.AddReaction(FReaction{Own, UserManager}, false);
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

void FReactions::AddReaction(const FReaction& Reaction, const bool bUpdateCounts)
{
    auto& Group = ReactionGroups.FindOrAdd(Reaction.Type);
    if (FReaction* Found = Group.LatestReactions.FindByPredicate([&](const FReaction& R) { return R.User == Reaction.User; }))
    {
        *Found = Reaction;
    }
    else
    {
        // Latest reactions are ordered reverse-chronologically
        Group.LatestReactions.Insert(Reaction, 0);
        if (bUpdateCounts)
        {
            ++Group.Count;
            ++Group.TotalScore;
        }
        if (Group.Type == NAME_None)
        {
            // First reaction of this type
            Group.Type = Reaction.Type;
            ReactionGroups.KeySort(FNameLexicalLess());
        }
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

TOptional<FReaction> FReactions::GetOwnReaction(const FName& ReactionType, const UUserManager* UserManager) const
{
    if (const FReactionGroup* Group = ReactionGroups.Find(ReactionType))
    {
        return Group->GetOwnReaction(UserManager);
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

int FReactions::LocalCount() const
{
    int32 Sum = 0;
    for (auto&& Pair : ReactionGroups)
    {
        Sum += Pair.Value.LatestReactions.Num();
    }
    return Sum;
}

bool FReactions::HasAllDataLocally() const
{
    for (auto&& Pair : ReactionGroups)
    {
        if (!Pair.Value.HasAllDataLocally())
        {
            return false;
        }
    }
    return true;
}

bool UReactionsBlueprintLibrary::HasOwnReaction(const FReactions& Reactions, const FName& ReactionType)
{
    return Reactions.GetOwnReaction(ReactionType, UUserManager::Get()).IsSet();
}

bool UReactionsBlueprintLibrary::IsEmpty(const FReactions& Reactions)
{
    return Reactions.IsEmpty();
}
