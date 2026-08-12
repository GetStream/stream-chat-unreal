// Copyright 2026 Stream.IO, Inc. All Rights Reserved.

#include "Channel/ChatChannel.h"
#include "CoreMinimal.h"
#include "Moderation/Ban.h"
#include "Moderation/UserBlock.h"
#include "StreamChatClientComponent.h"
#include "User/UserManager.h"

namespace ModerationTools
{
UStreamChatClientComponent* Client;
UChatChannel* Channel;
FMessage Message;
FUserRef User;

// https://getstream.io/chat/docs/unreal/moderation/?language=unreal#flag
void Flag()
{
    Client->FlagMessage(Message);

    // Give the moderators a reason, and any extra context your app has
    Client->FlagMessage(Message, TEXT("spam"), {{TEXT("origin"), TEXT("in-game report dialog")}});

    Client->FlagUser(User, TEXT("impersonation"));

    // Withdraw a flag raised by mistake
    Client->UnflagMessage(Message);
    Client->UnflagUser(User);
}

// https://getstream.io/chat/docs/unreal/moderation/?language=unreal#block
void Block()
{
    // Hides every 1-on-1 channel shared with that user, and stops their events and push notifications
    Client->BlockUser(User);

    Client->UnblockUser(User);

    // Nothing else populates FOwnUser::BlockedUserIds, so call this once after connecting if you want
    // to know whether a user is blocked without asking the API again
    Client->GetBlockedUsers(
        [](const TArray<FUserBlock>& Blocks)
        {
            for (const FUserBlock& Block : Blocks)
            {
                UE_LOG(LogTemp, Log, TEXT("Blocked %s at %s"), *Block.BlockedUser->Id, *Block.CreatedAt.ToString());
            }
        });

    const bool bBlocked = UUserManager::Get()->GetCurrentUser().HasBlockedUser(User);
    UE_LOG(LogTemp, Log, TEXT("Blocked: %d"), bBlocked);
}

// https://getstream.io/moderation/docs/guides/bounce-message/
void BouncedMessages()
{
    // Moderation can bounce a message back to its author instead of publishing it. The bounced message
    // is only ever delivered to the author, and is not stored, so it lives in this client's message
    // list alone. FMessage::Moderation carries the verdict.
    if (Message.IsBounced())
    {
        UE_LOG(LogTemp, Log, TEXT("Bounced, original text was: %s"), *Message.Moderation.OriginalText);

        // Let the author rephrase it. Editing a bounced message sends it afresh rather than updating
        // one on the server, because there is nothing there to update.
        FMessage Rephrased = Message;
        Rephrased.Text = TEXT("Something more polite");
        Channel->ResendMessage(Rephrased);

        // Or discard it. Deleting a bounced message only removes it locally.
        Channel->DeleteMessage(Message);
    }

    // The other verdicts. Remove means the message was taken out of the channel, and Flag means it was
    // published but sent to the dashboard for a moderator to look at.
    switch (Message.Moderation.Action)
    {
        case EMessageModerationAction::Bounce:
        case EMessageModerationAction::Remove:
        case EMessageModerationAction::Flag:
            break;
        case EMessageModerationAction::Other:
            // An action added to the API since this version of the SDK
            UE_LOG(LogTemp, Log, TEXT("Unrecognised moderation action: %s"), *Message.Moderation.RawAction);
            break;
        case EMessageModerationAction::None:
            break;
    }
}

// https://getstream.io/chat/docs/unreal/moderation/?language=unreal#mutes
void Mutes()
{
    Client->MuteUser(User);

    Client->UnmuteUser(User);
}

// https://getstream.io/chat/docs/unreal/moderation/?language=unreal#ban
void Ban()
{
    // ban a user for 60 minutes from all channel
    Client->BanUser(User, FTimespan::FromMinutes(60.), {TEXT("Banned for one hour")});

    // ban a user and their IP address for 24 hours
    Client->BanUser(User, FTimespan::FromHours(24.), {TEXT("Please come back tomorrow")}, true);

    // ban a user from a specific channel
    Channel->BanMember(User, {}, {TEXT("Profanity is not allowed here")});

    // remove ban from channel
    Channel->UnbanMember(User);

    // remove global ban
    Client->UnbanUser(User);
}

// https://getstream.io/chat/docs/unreal/moderation/?language=unreal#query-banned-users
void QueryBannedUsers()
{
    // Retrieve the list of banned users
    Client->QueryUsers(
        FFilter::Equal(TEXT("banned"), true),
        {},      // Sort options
        true,    // Presence
        {},      // Pagination options
        [](const TArray<FUserRef> Users)
        {
            // Do something with Users
        });

    // Query for banned members from one channel
    Client->QueryBannedUsers(
        FFilter::Equal(TEXT("channel_cid"), TEXT("ChannelType:ChannelId")),
        {},    // Sort options
        {},    // Pagination options
        [](const TArray<FBan> Bans)
        {
            // Do something with Bans
        });
}

// https://getstream.io/chat/docs/unreal/moderation/?language=unreal#query-bans-endpoint
void QueryBansEndpoint()
{
    // Get the bans for channel livestream:123 in reverse chronological order of banning
    Client->QueryBannedUsers(
        FFilter::Equal(TEXT("channel_cid"), TEXT("livestream:123")),
        {{EBanSortField::CreatedAt, ESortDirection::Descending}},
        {},
        [](const TArray<FBan> Bans)
        {
            // Do something with Bans
        });

    // Get the page of bans which were created before or equal to the current time for the same channel
    FBanPaginationOptions PaginationOptions;
    PaginationOptions.CreatedAtBeforeOrEqual = FDateTime::UtcNow();
    Client->QueryBannedUsers(
        FFilter::Equal(TEXT("channel_cid"), TEXT("livestream:123")),
        {{EBanSortField::CreatedAt, ESortDirection::Descending}},
        {PaginationOptions},
        [](const TArray<FBan> Bans)
        {
            // Do something with Bans
        });
}

// https://getstream.io/chat/docs/unreal/moderation/?language=unreal#shadow-ban
void ShadowBan()
{
    // shadow ban a user from all channels
    Client->ShadowBanUser(User);

    // shadow ban a user from a channel
    Channel->ShadowBanMember(User);

    // remove shadow ban from channel
    Channel->ShadowUnbanMember(User);

    // remove global shadow ban
    Client->ShadowUnbanUser(User);
}
}    // namespace ModerationTools
