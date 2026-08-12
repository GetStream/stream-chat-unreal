// Copyright 2026 Stream.IO, Inc. All Rights Reserved.

#include "Channel/Message.h"

#include "Misc/AutomationTest.h"
#include "Request/Message/MessageRequestDto.h"
#include "Response/Message/MessageDto.h"
#include "StreamJson.h"
#include "User/User.h"
#include "User/UserManager.h"

#if WITH_DEV_AUTOMATION_TESTS

BEGIN_DEFINE_SPEC(FMessageSpec, "StreamChat.Message", EAutomationTestFlags::ProductFilter | EAutomationTestFlags_ApplicationContextMask)
END_DEFINE_SPEC(FMessageSpec)

void FMessageSpec::Define()
{
    Describe(
        "Reading a message off the wire",
        [this]
        {
            It("should read the fields the API adds to a message",
               [this]
               {
                   const FString Json = R"(
{
    "id": "message-1",
    "cid": "messaging:fun",
    "text": "/giphy hello",
    "command": "giphy",
    "type": "regular",
    "user": {"id": "author"},
    "created_at": "2026-08-12T09:30:00Z",
    "quoted_message_id": "message-0",
    "pinned": true,
    "pinned_at": "2026-08-12T09:31:00Z",
    "pin_expires": "2026-08-13T09:31:00Z",
    "pinned_by": {"id": "moderator"}
}
)";
                   const FMessage Message{Json::Deserialize<FMessageDto>(Json), UUserManager::Get()};

                   TestEqual("Id", Message.Id, TEXT("message-1"));
                   TestEqual("Cid", Message.Cid, TEXT("messaging:fun"));
                   TestEqual("Command", Message.Command, TEXT("giphy"));
                   TestEqual("Quoted message", Message.QuotedMessageId, TEXT("message-0"));
                   TestTrue("Quoting", Message.IsQuoting());
                   TestTrue("Pinned", Message.bPinned);
                   TestEqual("Pinned at", Message.PinnedAt, FDateTime{2026, 8, 12, 9, 31, 0});
                   TestEqual("Pin expires", Message.PinExpires, FDateTime{2026, 8, 13, 9, 31, 0});
                   TestEqual("Pinned by", Message.PinnedBy->Id, TEXT("moderator"));
               });

            It("should leave the pin fields empty on an ordinary message",
               [this]
               {
                   const FString Json = R"({"id": "message-1", "text": "hi", "user": {"id": "author"}})";
                   const FMessage Message{Json::Deserialize<FMessageDto>(Json), UUserManager::Get()};

                   TestFalse("Not pinned", Message.bPinned);
                   TestEqual("Pinned at", Message.PinnedAt.GetTicks(), static_cast<int64>(0));
                   TestEqual("Pin expires", Message.PinExpires.GetTicks(), static_cast<int64>(0));
                   TestFalse("Nobody pinned it", Message.PinnedBy.IsValid());
                   TestFalse("Not quoting", Message.IsQuoting());
                   TestEqual("No command", Message.Command, TEXT(""));
               });
        });

    Describe(
        "Translations",
        [this]
        {
            It("should split the i18n object into translations and the original language",
               [this]
               {
                   // The API flattens both into one object, keying each translation by <code>_text
                   const FString Json = R"(
{
    "id": "message-1",
    "text": "hello",
    "i18n": {
        "language": "en",
        "fr_text": "bonjour",
        "it_text": "ciao"
    }
}
)";
                   const FMessage Message{Json::Deserialize<FMessageDto>(Json), UUserManager::Get()};

                   TestEqual("Original language", Message.OriginalLanguage, TEXT("en"));
                   TestEqual("Translations", Message.Translations.Num(), 2);
                   TestEqual("French", Message.GetTranslation(TEXT("fr")), TEXT("bonjour"));
                   TestEqual("Italian", Message.GetTranslation(TEXT("it")), TEXT("ciao"));
               });

            It("should fall back to the original text for a language it wasn't translated into",
               [this]
               {
                   const FString Json = R"({"id": "message-1", "text": "hello", "i18n": {"language": "en", "fr_text": "bonjour"}})";
                   const FMessage Message{Json::Deserialize<FMessageDto>(Json), UUserManager::Get()};

                   TestEqual("German falls back", Message.GetTranslation(TEXT("de")), TEXT("hello"));
               });

            It("should report no translations for a message from an app without auto-translation",
               [this]
               {
                   const FString Json = R"({"id": "message-1", "text": "hello"})";
                   const FMessage Message{Json::Deserialize<FMessageDto>(Json), UUserManager::Get()};

                   TestEqual("No translations", Message.Translations.Num(), 0);
                   TestEqual("No original language", Message.OriginalLanguage, TEXT(""));
                   TestEqual("Falls back to the text", Message.GetTranslation(TEXT("fr")), TEXT("hello"));
               });
        });

    Describe(
        "Sending a message",
        [this]
        {
            It("should send the fields the caller is allowed to set",
               [this]
               {
                   UUserManager* UserManager = UUserManager::Get();
                   FMessage Message{TEXT("hello")};
                   Message.Id = TEXT("message-1");
                   Message.MentionedUsers = {UserManager->UpsertUser(TEXT("mentioned-1")), UserManager->UpsertUser(TEXT("mentioned-2"))};
                   Message.bPinned = true;
                   Message.PinExpires = FDateTime{2026, 8, 13, 9, 31, 0};
                   Message.QuotedMessageId = TEXT("message-0");

                   const FString Json = Json::Serialize(Message.ToRequestDto(TEXT("messaging:fun")));
                   TestTrue("Cid", Json.Contains(TEXT(R"("cid":"messaging:fun")")));
                   TestTrue("Mentioned users", Json.Contains(TEXT(R"("mentioned_users":["mentioned-1","mentioned-2"])")));
                   TestTrue("Pinned", Json.Contains(TEXT(R"("pinned":true)")));
                   TestTrue("Pin expires", Json.Contains(TEXT(R"("pin_expires":"2026-08-13T09:31:00.000Z")")));
                   TestTrue("Quoted message", Json.Contains(TEXT(R"("quoted_message_id":"message-0")")));
               });

            It("should leave out what an ordinary message doesn't set",
               [this]
               {
                   // The backend reads an empty pin_expires as a date, not as "no expiry", so it has
                   // to be missing from the request rather than zeroed
                   const FMessage Message{TEXT("hello")};

                   const FString Json = Json::Serialize(Message.ToRequestDto(TEXT("messaging:fun")));
                   TestTrue("Not pinned", Json.Contains(TEXT(R"("pinned":false)")));
                   TestFalse("No pin expiry", Json.Contains(TEXT("pin_expires")));
                   TestFalse("No quoted message", Json.Contains(TEXT("quoted_message_id")));
                   TestFalse("No mentioned users", Json.Contains(TEXT("mentioned_users")));
               });

            It("should round-trip a quoted, pinned message through the API's own message shape",
               [this]
               {
                   FMessage Message{TEXT("hello")};
                   Message.QuotedMessageId = TEXT("message-0");
                   Message.bPinned = true;

                   // The response carries the same keys back, so what was sent has to be readable again
                   const FString Json = Json::Serialize(Message.ToRequestDto(TEXT("messaging:fun")));
                   const FMessage RoundTripped{Json::Deserialize<FMessageDto>(Json), UUserManager::Get()};

                   TestEqual("Text", RoundTripped.Text, Message.Text);
                   TestEqual("Quoted message", RoundTripped.QuotedMessageId, TEXT("message-0"));
                   TestTrue("Pinned", RoundTripped.bPinned);
                   TestEqual("Cid", RoundTripped.Cid, TEXT("messaging:fun"));
               });
        });
}

#endif    // WITH_DEV_AUTOMATION_TESTS
