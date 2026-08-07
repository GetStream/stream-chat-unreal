// Copyright 2026 Stream.IO, Inc. All Rights Reserved.

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
    Algo::Transform(Dto.Attachments, Attachments, [](const FAttachmentDto& Attachment) { return FAttachment{Attachment}; });
}

FMessage::FMessage(const FSearchResultDto& Dto, UUserManager* UserManager) : FMessage{Dto.Message, UserManager}
{
}

FMessage::FMessage(const FString& Text)
    : Id{FGuid::NewGuid().ToString(EGuidFormats::DigitsWithHyphens)}, Text{Text}, CreatedAt{FDateTime::UtcNow()}, UpdatedAt{FDateTime::UtcNow()}
{
}

FMessageRequestDto FMessage::ToRequestDto(const FString& Cid) const
{
    // Assigned by name rather than positionally: the previous aggregate initialiser silently
    // shifted every field along when a new one was added to the DTO.
    FMessageRequestDto Dto;
    Algo::Transform(Attachments, Dto.Attachments, [](const FAttachment& Attachment) { return Attachment.ToDto(); });
    Dto.Cid = Cid;
    Dto.Id = Id;
    Dto.ReactionScores = Reactions.GetScores();
    Dto.bSilent = bIsSilent;
    Dto.Text = Text;
    Dto.AdditionalFields = ExtraData;
    return Dto;
}
