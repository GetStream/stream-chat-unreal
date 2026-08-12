// Copyright 2026 Stream.IO, Inc. All Rights Reserved.

#include "Moderation/MessageModeration.h"

#include "Channel/Message.h"
#include "Misc/AutomationTest.h"
#include "Response/Message/MessageDto.h"
#include "Response/Moderation/MessageModerationDto.h"
#include "User/UserManager.h"

#if WITH_DEV_AUTOMATION_TESTS

BEGIN_DEFINE_SPEC(
    FMessageModerationSpec,
    "StreamChat.MessageModeration",
    EAutomationTestFlags::ProductFilter | EAutomationTestFlags_ApplicationContextMask)
END_DEFINE_SPEC(FMessageModerationSpec)

void FMessageModerationSpec::Define()
{
    Describe(
        "Moderation V2 actions",
        [this]
        {
            It("should read a bounce",
               [this]
               {
                   FMessageModerationDto Dto;
                   Dto.Action = TEXT("bounce");
                   Dto.OriginalText = TEXT("something rude");

                   const FMessageModeration Moderation{Dto};
                   TestEqual("Action", Moderation.Action, EMessageModerationAction::Bounce);
                   TestTrue("Set", Moderation.IsSet());
                   TestEqual("Original text kept", Moderation.OriginalText, TEXT("something rude"));
               });

            It("should read a remove",
               [this]
               {
                   FMessageModerationDto Dto;
                   Dto.Action = TEXT("remove");
                   TestEqual("Action", FMessageModeration{Dto}.Action, EMessageModerationAction::Remove);
               });

            It("should read a flag",
               [this]
               {
                   FMessageModerationDto Dto;
                   Dto.Action = TEXT("flag");
                   TestEqual("Action", FMessageModeration{Dto}.Action, EMessageModerationAction::Flag);
               });
        });

    Describe(
        "Moderation V1 actions",
        [this]
        {
            It("should read a bounce",
               [this]
               {
                   FMessageModerationDto Dto;
                   Dto.Action = TEXT("MESSAGE_RESPONSE_ACTION_BOUNCE");
                   TestEqual("Action", FMessageModeration{Dto}.Action, EMessageModerationAction::Bounce);
               });

            It("should read a block as a remove",
               [this]
               {
                   // The one action the two versions disagree on the name of
                   FMessageModerationDto Dto;
                   Dto.Action = TEXT("MESSAGE_RESPONSE_ACTION_BLOCK");
                   TestEqual("Action", FMessageModeration{Dto}.Action, EMessageModerationAction::Remove);
               });

            It("should read a flag",
               [this]
               {
                   FMessageModerationDto Dto;
                   Dto.Action = TEXT("MESSAGE_RESPONSE_ACTION_FLAG");
                   TestEqual("Action", FMessageModeration{Dto}.Action, EMessageModerationAction::Flag);
               });
        });

    Describe(
        "Unrecognised verdicts",
        [this]
        {
            It("should report no verdict when the API sent none",
               [this]
               {
                   const FMessageModeration Moderation{FMessageModerationDto{}};
                   TestEqual("Action", Moderation.Action, EMessageModerationAction::None);
                   TestFalse("Not set", Moderation.IsSet());
               });

            It("should keep an action it does not know, rather than dropping it",
               [this]
               {
                   // A new server-side action must still reach the caller: this is the whole reason the
                   // DTO holds the action as a string rather than as an enum
                   FMessageModerationDto Dto;
                   Dto.Action = TEXT("some_future_action");

                   const FMessageModeration Moderation{Dto};
                   TestEqual("Action", Moderation.Action, EMessageModerationAction::Other);
                   TestTrue("Set", Moderation.IsSet());
                   TestEqual("Raw action kept", Moderation.RawAction, TEXT("some_future_action"));
               });
        });

    Describe(
        "A moderated message",
        [this]
        {
            It("should prefer the V2 verdict when the API sent both",
               [this]
               {
                   // An app is on one moderation version or the other, so both keys arriving at once is
                   // not something the API does. Pinned anyway because the fallback has to be ordered:
                   // reading V1 first would mask the newer verdict.
                   FMessageDto Dto;
                   Dto.Moderation.Action = TEXT("bounce");
                   Dto.ModerationDetails.Action = TEXT("MESSAGE_RESPONSE_ACTION_FLAG");

                   const FMessage Message{Dto, UUserManager::Get()};
                   TestEqual("Action", Message.Moderation.Action, EMessageModerationAction::Bounce);
                   TestTrue("Bounced", Message.IsBounced());
               });

            It("should fall back to the V1 verdict",
               [this]
               {
                   FMessageDto Dto;
                   Dto.ModerationDetails.Action = TEXT("MESSAGE_RESPONSE_ACTION_BOUNCE");

                   const FMessage Message{Dto, UUserManager::Get()};
                   TestEqual("Action", Message.Moderation.Action, EMessageModerationAction::Bounce);
                   TestTrue("Bounced", Message.IsBounced());
               });

            It("should report no verdict for the ordinary message",
               [this]
               {
                   const FMessageDto Dto;
                   const FMessage Message{Dto, UUserManager::Get()};
                   TestFalse("Not moderated", Message.Moderation.IsSet());
                   TestFalse("Not bounced", Message.IsBounced());
                   TestFalse("Not removed", Message.IsRemovedByModeration());
                   TestFalse("Not a moderation error", Message.IsModerationError());
               });

            It("should tell a removed message from a bounced one",
               [this]
               {
                   FMessage Message{TEXT("hi")};
                   Message.Moderation.Action = EMessageModerationAction::Remove;

                   TestFalse("Not bounced", Message.IsBounced());
                   TestTrue("Removed", Message.IsRemovedByModeration());
                   // Only a bounce is the author's to do something about
                   TestFalse("Not a moderation error", Message.IsModerationError());
               });
        });

    It("should carry the harm labels of a server side response",
       [this]
       {
           FMessageModerationDto Dto;
           Dto.Action = TEXT("remove");
           Dto.TextHarms = {TEXT("HARM_HATE"), TEXT("HARM_HARASSMENT")};
           Dto.ImageHarms = {TEXT("HARM_NUDITY")};
           Dto.BlocklistMatched = TEXT("profanity_en_2020_v1");
           Dto.SemanticFilterMatched = TEXT("no self harm");
           Dto.bPlatformCircumvented = true;

           const FMessageModeration Moderation{Dto};
           TestEqual("Text harms", Moderation.TextHarms.Num(), 2);
           TestEqual("First text harm", Moderation.TextHarms[0], TEXT("HARM_HATE"));
           TestEqual("Image harms", Moderation.ImageHarms.Num(), 1);
           TestEqual("Blocklist", Moderation.BlocklistMatched, TEXT("profanity_en_2020_v1"));
           TestEqual("Semantic filter", Moderation.SemanticFilterMatched, TEXT("no self harm"));
           TestTrue("Platform circumvented", Moderation.bPlatformCircumvented);
       });
}

#endif    // WITH_DEV_AUTOMATION_TESTS
