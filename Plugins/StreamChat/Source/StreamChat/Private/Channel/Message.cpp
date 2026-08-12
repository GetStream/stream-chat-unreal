// Copyright 2026 Stream.IO, Inc. All Rights Reserved.

#include "Channel/Message.h"

#include "Algo/Transform.h"
#include "Request/Message/MessageRequestDto.h"
#include "Response/Message/MessageDto.h"
#include "User/UserManager.h"

namespace
{
/**
 * Pick the moderation verdict out of a message DTO.
 *
 * An app configured for Moderation V2 gets its verdict in `moderation`, and one still on the older
 * API gets it in `moderation_details`. Only ever one of the two arrives, so preferring the newer
 * field and falling back covers both without the caller having to care which is in use.
 */
FMessageModeration PickModeration(const FMessageDto& Dto)
{
    const FMessageModeration V2{Dto.Moderation};
    if (V2.IsSet())
    {
        return V2;
    }
    return FMessageModeration{Dto.ModerationDetails};
}

/// The key auto-translation names the message's own language under
const TCHAR* LanguageKey = TEXT("language");
/// The suffix auto-translation appends to a language code to key that language's text
const TCHAR* TranslationSuffix = TEXT("_text");

/**
 * Split the `i18n` object into a language code -> text map.
 *
 * The API packs both the detected language and every translation into one flat object, keyed
 * `language` and `<code>_text` respectively. Callers want to look a translation up by plain language
 * code, so the suffix is stripped here rather than at every call site.
 */
TMap<FString, FString> PickTranslations(const FMessageDto& Dto)
{
    TMap<FString, FString> Translations;
    for (const TPair<FString, FString>& Pair : Dto.I18n)
    {
        if (Pair.Key.EndsWith(TranslationSuffix))
        {
            Translations.Add(Pair.Key.LeftChop(FCString::Strlen(TranslationSuffix)), Pair.Value);
        }
    }
    return Translations;
}
}    // namespace

FMessage::FMessage() = default;

FMessage::FMessage(const FMessageDto& Dto, UUserManager* UserManager)
    : Id{Dto.Id}
    , Cid{Dto.Cid}
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
    , bShowInChannel{Dto.bShowInChannel}
    , ReplyCount{static_cast<int32>(Dto.ReplyCount)}
    , ThreadParticipants{UserManager->UpsertUsers(Dto.ThreadParticipants)}
    , bIsSilent{Dto.bSilent}
    , bIsShadowed{Dto.bShadowed}
    , Moderation{PickModeration(Dto)}
    , Html{Dto.Html}
    , Command{Dto.Command}
    , bPinned{Dto.bPinned}
    , PinnedAt{Dto.PinnedAt}
    , PinnedBy{UserManager->UpsertUser(Dto.PinnedBy)}
    , PinExpires{Dto.PinExpires}
    , QuotedMessageId{Dto.QuotedMessageId}
    , Translations{PickTranslations(Dto)}
    , OriginalLanguage{Dto.I18n.FindRef(LanguageKey)}
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

FMessageRequestDto FMessage::ToRequestDto(const FString& ChannelCid) const
{
    // Assigned by name rather than positionally: the previous aggregate initialiser silently
    // shifted every field along when a new one was added to the DTO.
    FMessageRequestDto Dto;
    Algo::Transform(Attachments, Dto.Attachments, [](const FAttachment& Attachment) { return Attachment.ToDto(); });
    Dto.Cid = ChannelCid;
    Dto.Id = Id;
    Algo::Transform(MentionedUsers, Dto.MentionedUsers, [](const FUserRef& User) { return User->Id; });
    Dto.ParentId = ParentId;
    Dto.PinExpires = PinExpires;
    Dto.bPinned = bPinned;
    Dto.QuotedMessageId = QuotedMessageId;
    Dto.ReactionScores = Reactions.GetScores();
    // Only meaningful on a reply, and the backend rejects it on anything else
    Dto.bShowInChannel = IsThreadReply() && bShowInChannel;
    Dto.bSilent = bIsSilent;
    Dto.Text = Text;
    Dto.AdditionalFields = ExtraData;
    return Dto;
}

bool FMessage::IsThreadReply() const
{
    return !ParentId.IsEmpty();
}

bool FMessage::IsThreadStart() const
{
    return ReplyCount > 0;
}

bool FMessage::IsBounced() const
{
    return Moderation.Action == EMessageModerationAction::Bounce;
}

bool FMessage::IsRemovedByModeration() const
{
    return Moderation.Action == EMessageModerationAction::Remove;
}

bool FMessage::IsModerationError() const
{
    return IsBounced() && Type == EMessageType::Error && User.IsCurrent();
}

bool FMessage::IsQuoting() const
{
    return !QuotedMessageId.IsEmpty();
}

const FString& FMessage::GetTranslation(const FString& Language) const
{
    if (const FString* Translation = Translations.Find(Language))
    {
        return *Translation;
    }
    return Text;
}
