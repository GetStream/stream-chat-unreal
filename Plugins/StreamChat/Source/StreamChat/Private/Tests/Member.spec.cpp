// Copyright 2026 Stream.IO, Inc. All Rights Reserved.

#include "Channel/Member.h"

#include "Misc/AutomationTest.h"
#include "Response/Channel/ChannelMemberDto.h"
#include "StreamJson.h"
#include "User/User.h"
#include "User/UserManager.h"

#if WITH_DEV_AUTOMATION_TESTS

BEGIN_DEFINE_SPEC(FMemberSpec, "StreamChat.Member", EAutomationTestFlags::ProductFilter | EAutomationTestFlags_ApplicationContextMask)
END_DEFINE_SPEC(FMemberSpec)

void FMemberSpec::Define()
{
    Describe(
        "Reading a member off the wire",
        [this]
        {
            It("should read every field of the membership",
               [this]
               {
                   const FString Json = R"(
{
    "user_id": "member-user",
    "user": {"id": "member-user", "name": "Member"},
    "channel_role": "channel_moderator",
    "created_at": "2026-08-01T10:00:00Z",
    "updated_at": "2026-08-02T11:00:00Z",
    "invited": true,
    "invite_accepted_at": "2026-08-01T10:05:00Z",
    "is_moderator": true,
    "banned": true,
    "ban_expires": "2026-09-01T00:00:00Z",
    "shadow_banned": true,
    "notifications_muted": true,
    "archived_at": "2026-08-03T12:00:00Z",
    "pinned_at": "2026-08-04T13:00:00Z",
    "clan_rank": "captain"
}
)";
                   const FChannelMemberDto Dto = Json::Deserialize<FChannelMemberDto>(Json);
                   const FMember Member{Dto, UUserManager::Get()};

                   TestEqual("User", Member.User->Id, TEXT("member-user"));
                   TestEqual("Channel role", Member.ChannelRole, TEXT("channel_moderator"));
                   TestEqual("Created at", Member.CreatedAt, FDateTime{2026, 8, 1, 10, 0, 0});
                   TestEqual("Updated at", Member.UpdatedAt, FDateTime{2026, 8, 2, 11, 0, 0});
                   TestTrue("Invited", Member.bInvited);
                   TestEqual("Invite accepted at", Member.InviteAcceptedAt, FDateTime{2026, 8, 1, 10, 5, 0});
                   TestTrue("Moderator", Member.bIsModerator);
                   TestTrue("Banned", Member.bBanned);
                   TestEqual("Ban expires", Member.BanExpires, FDateTime{2026, 9, 1, 0, 0, 0});
                   TestTrue("Shadow banned", Member.bShadowBanned);
                   TestTrue("Notifications muted", Member.bNotificationsMuted);
                   TestEqual("Archived at", Member.ArchivedAt, FDateTime{2026, 8, 3, 12, 0, 0});
                   TestEqual("Pinned at", Member.PinnedAt, FDateTime{2026, 8, 4, 13, 0, 0});
                   TestEqual("Custom field", Member.ExtraData.GetString(TEXT("clan_rank")).Get(TEXT("")), TEXT("captain"));
               });

            It("should leave the dates zeroed when the API sent them as null",
               [this]
               {
                   // Which is how the API says "never happened" for every one of these
                   const FString Json = R"(
{
    "user": {"id": "member-user"},
    "invite_accepted_at": null,
    "invite_rejected_at": null,
    "archived_at": null,
    "pinned_at": null,
    "ban_expires": null
}
)";
                   const FMember Member{Json::Deserialize<FChannelMemberDto>(Json), UUserManager::Get()};

                   TestEqual("Invite accepted at", Member.InviteAcceptedAt.GetTicks(), static_cast<int64>(0));
                   TestEqual("Invite rejected at", Member.InviteRejectedAt.GetTicks(), static_cast<int64>(0));
                   TestEqual("Archived at", Member.ArchivedAt.GetTicks(), static_cast<int64>(0));
                   TestEqual("Pinned at", Member.PinnedAt.GetTicks(), static_cast<int64>(0));
                   TestEqual("Ban expires", Member.BanExpires.GetTicks(), static_cast<int64>(0));
                   TestFalse("Not banned", Member.bBanned);
                   TestFalse("Not invited", Member.bInvited);
                   TestFalse("Not a moderator", Member.bIsModerator);
               });

            It("should still identify the user when only a user_id was sent",
               [this]
               {
                   // Which is all a member gets in some event payloads
                   FChannelMemberDto Dto;
                   Dto.UserId = TEXT("member-user");

                   const FMember Member{Dto, UUserManager::Get()};
                   TestTrue("User is valid", Member.User.IsValid());
                   TestEqual("User", Member.User->Id, TEXT("member-user"));
               });
        });

    Describe(
        "Invitations",
        [this]
        {
            It("should report an unanswered invite as pending",
               [this]
               {
                   FChannelMemberDto Dto;
                   Dto.User.Id = TEXT("member-user");
                   Dto.bInvited = true;

                   const FMember Member{Dto, UUserManager::Get()};
                   TestTrue("Pending", Member.IsInvitePending());
                   TestFalse("Not accepted", Member.HasAcceptedInvite());
                   TestFalse("Not rejected", Member.HasRejectedInvite());
               });

            It("should report an accepted invite as answered",
               [this]
               {
                   FChannelMemberDto Dto;
                   Dto.User.Id = TEXT("member-user");
                   Dto.bInvited = true;
                   Dto.InviteAcceptedAt = FDateTime{2026, 8, 1};

                   const FMember Member{Dto, UUserManager::Get()};
                   TestFalse("Not pending", Member.IsInvitePending());
                   TestTrue("Accepted", Member.HasAcceptedInvite());
               });

            It("should report a rejected invite as answered",
               [this]
               {
                   FChannelMemberDto Dto;
                   Dto.User.Id = TEXT("member-user");
                   Dto.bInvited = true;
                   Dto.InviteRejectedAt = FDateTime{2026, 8, 1};

                   const FMember Member{Dto, UUserManager::Get()};
                   TestFalse("Not pending", Member.IsInvitePending());
                   TestTrue("Rejected", Member.HasRejectedInvite());
               });

            It("should not report a member who was added rather than invited as pending",
               [this]
               {
                   FChannelMemberDto Dto;
                   Dto.User.Id = TEXT("member-user");

                   const FMember Member{Dto, UUserManager::Get()};
                   TestFalse("Not pending", Member.IsInvitePending());
               });
        });

    It("should compare members by the user they refer to",
       [this]
       {
           // Two payloads for one user are one member, however much of the membership each carried
           FChannelMemberDto Dto;
           Dto.User.Id = TEXT("member-user");
           FChannelMemberDto Updated;
           Updated.User.Id = TEXT("member-user");
           Updated.bBanned = true;

           const FMember Member{Dto, UUserManager::Get()};
           const FMember UpdatedMember{Updated, UUserManager::Get()};
           TestTrue("Same member", Member == UpdatedMember);
           TestTrue("Same user", Member == UpdatedMember.User);
       });
}

#endif    // WITH_DEV_AUTOMATION_TESTS
