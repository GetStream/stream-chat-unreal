// Copyright 2026 Stream.IO, Inc. All Rights Reserved.

#include "Channel/Member.h"

#include "Response/Channel/ChannelMemberDto.h"
#include "User/UserManager.h"

FMember::FMember() = default;

FMember::FMember(const FUserRef& User) : User{User}
{
}

FMember::FMember(const FChannelMemberDto& Dto, UUserManager* UserManager)
    // The member payload of an event carries only user_id, where a channel state response carries
    // the whole user object. Falling back keeps the reference pointing at the right user either way.
    : User{Dto.User.Id.IsEmpty() ? UserManager->UpsertUser(Dto.UserId) : UserManager->UpsertUser(Dto.User)}
    , BanExpires{Dto.BanExpires}
    , bBanned{Dto.bBanned}
    , ChannelRole{Dto.ChannelRole}
    , CreatedAt{Dto.CreatedAt}
    , UpdatedAt{Dto.UpdatedAt}
    , bInvited{Dto.bInvited}
    , InviteAcceptedAt{Dto.InviteAcceptedAt}
    , InviteRejectedAt{Dto.InviteRejectedAt}
    , bIsModerator{Dto.bIsModerator}
    , bShadowBanned{Dto.bShadowBanned}
    , bNotificationsMuted{Dto.bNotificationsMuted}
    , ArchivedAt{Dto.ArchivedAt}
    , PinnedAt{Dto.PinnedAt}
    , ExtraData{Dto.AdditionalFields}
{
}

bool FMember::IsInvitePending() const
{
    return bInvited && !HasAcceptedInvite() && !HasRejectedInvite();
}

bool FMember::HasAcceptedInvite() const
{
    return InviteAcceptedAt.GetTicks() != 0;
}

bool FMember::HasRejectedInvite() const
{
    return InviteRejectedAt.GetTicks() != 0;
}

bool FMember::operator==(const FUserRef& Other) const
{
    return this->User == Other;
}

bool FMember::operator==(const FMember& Other) const
{
    return this->User == Other.User;
}

uint32 GetTypeHash(const FMember& M)
{
    return GetTypeHash(M.User);
}
