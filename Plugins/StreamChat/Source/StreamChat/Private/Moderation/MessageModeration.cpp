// Copyright 2026 Stream.IO, Inc. All Rights Reserved.

#include "Moderation/MessageModeration.h"

namespace
{
/**
 * Translate the API's action name to the enum.
 *
 * Both moderation versions land here. Their vocabularies do not overlap, so one lookup covers each:
 * V2 sends lowercase names, and V1 sends SCREAMING_SNAKE ones. V1's "block" is V2's "remove", which
 * is the one pair whose names disagree.
 */
EMessageModerationAction ParseAction(const FString& Action)
{
    if (Action.IsEmpty())
    {
        return EMessageModerationAction::None;
    }
    if (Action.Equals(TEXT("bounce"), ESearchCase::IgnoreCase) || Action.Equals(TEXT("MESSAGE_RESPONSE_ACTION_BOUNCE"), ESearchCase::IgnoreCase))
    {
        return EMessageModerationAction::Bounce;
    }
    if (Action.Equals(TEXT("remove"), ESearchCase::IgnoreCase) || Action.Equals(TEXT("MESSAGE_RESPONSE_ACTION_BLOCK"), ESearchCase::IgnoreCase))
    {
        return EMessageModerationAction::Remove;
    }
    if (Action.Equals(TEXT("flag"), ESearchCase::IgnoreCase) || Action.Equals(TEXT("MESSAGE_RESPONSE_ACTION_FLAG"), ESearchCase::IgnoreCase))
    {
        return EMessageModerationAction::Flag;
    }
    return EMessageModerationAction::Other;
}
}    // namespace

FMessageModeration::FMessageModeration(const FMessageModerationDto& Dto)
    : Action{ParseAction(Dto.Action)}
    , RawAction{Dto.Action}
    , OriginalText{Dto.OriginalText}
    , TextHarms{Dto.TextHarms}
    , ImageHarms{Dto.ImageHarms}
    , BlocklistMatched{Dto.BlocklistMatched}
    , SemanticFilterMatched{Dto.SemanticFilterMatched}
    , bPlatformCircumvented{Dto.bPlatformCircumvented}
{
}

bool FMessageModeration::IsSet() const
{
    return Action != EMessageModerationAction::None;
}
