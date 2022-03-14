// Copyright 2022 Stream.IO, Inc. All Rights Reserved.

#include "Channel/Message.h"

#include "Algo/Transform.h"
#include "Request/Message/MessageRequestDto.h"
#include "Response/Message/MessageDto.h"
#include "User/UserManager.h"

FMessage::FMessage() = default;

FMessage::FMessage(const FMessageDto& Dto, UUserManager* UserManager)
    : Id{Dto.Id}
    , Text{Dto.Text}
    , State{EMessageSendState::Sent}    // Assume response dto => sent
    , User{UserManager->UpsertUser(Dto.User)}
    , Type{static_cast<EMessageType>(Dto.Type)}
    , MentionedUsers{UserManager->UpsertUsers(Dto.MentionedUsers)}
    , CreatedAt{Dto.CreatedAt}
    , UpdatedAt{Dto.UpdatedAt}
    , DeletedAt{Dto.GetDeletedAt()}
    , Reactions{FReactions::CollectReactions(UserManager, Dto.ReactionCounts, Dto.ReactionScores, Dto.LatestReactions, Dto.OwnReactions)}
    , ParentId{Dto.ParentId}
    , bIsSilent{Dto.bSilent}
    , bIsShadowed{Dto.bShadowed}
    , Html{Dto.Html}
    , ExtraData{Dto.AdditionalFields}
{
}

FMessage::FMessage(const FString& Text)
    : Id{FGuid::NewGuid().ToString(EGuidFormats::DigitsWithHyphens)}, Text{Text}, CreatedAt{FDateTime::UtcNow()}, UpdatedAt{FDateTime::UtcNow()}
{
}

FMessageRequestDto FMessage::ToRequestDto(const FString& Cid) const
{
    return FMessageRequestDto{Cid, {}, Id, {}, Reactions.GetScores(), bIsSilent, Text, ExtraData};
}

TArray<FMessage> FMessage::FromSearchResults(const TArray<FSearchResultDto>& Result)
{
    UUserManager* UserManager = UUserManager::Get();
    TArray<FMessage> Messages;
    Algo::Transform(Result, Messages, [UserManager](const FSearchResultDto& R) { return FMessage{R.Message, UserManager}; });
    return Messages;
}
