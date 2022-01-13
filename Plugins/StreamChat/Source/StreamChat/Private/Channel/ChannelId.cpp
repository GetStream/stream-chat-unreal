// Copyright 2021 Stream.IO, Inc. All Rights Reserved.

#include "Channel/ChannelId.h"

#include "Algo/Transform.h"
#include "Channel/Message.h"
#include "Response/Channel/ChannelResponseDto.h"
#include "User/UserManager.h"

FChannelId::FChannelId() = default;

FChannelId::FChannelId(const FChannelResponseDto& Dto, UUserManager& UserManager)
    : Type{Dto.Type}
    , Id{Dto.Id}
    , Cid{Dto.Cid}
    , Config{Dto.Config}
    , MemberCount{Dto.MemberCount}
    , Cooldown{Dto.Cooldown}
    , CreatedAt{Dto.CreatedAt}
    , UpdatedAt{Dto.UpdatedAt}
    , DeletedAt{Dto.DeletedAt}
    , LastMessageAt{Dto.LastMessageAt}
    , CreatedBy{UserManager.UpsertUser(Dto.CreatedBy)}
    , bDisabled{Dto.bDisabled}
    , bFrozen{Dto.bFrozen}
    , bHidden{Dto.bHidden}
    , bMuted{Dto.bMuted}
    , MuteExpiresAt{Dto.MuteExpiresAt}
    , HideMessagesBefore{Dto.HideMessagesBefore}
    , OwnCapabilities{Dto.OwnCapabilities}
    , bAutoTranslationEnabled{Dto.bAutoTranslationEnabled}
    , AutoTranslationLanguage{Dto.AutoTranslationLanguage}
    , Team{Dto.Team}
    , Name{Dto.AdditionalFields.GetString(TEXT("name")).Get(TEXT(""))}
    , ImageUrl{Dto.AdditionalFields.GetString(TEXT("image")).Get(TEXT(""))}
{
    SetMembers(UserManager, Dto.Members);
}

TArray<FUserRef> FChannelId::GetOtherMemberUsers() const
{
    constexpr auto NotCurrentUser = [](const FMember& M)
    {
        return !M.User.IsCurrent();
    };
    constexpr auto ToUser = [](const FMember& M)
    {
        return M.User;
    };
    TArray<FUserRef> OtherUsers;
    Algo::TransformIf(Members, OtherUsers, NotCurrentUser, ToUser);
    return OtherUsers;
}

void FChannelId::SetMembers(UUserManager& UserManager, const TArray<FChannelMemberDto>& Dto)
{
    Algo::Transform(Dto, Members, [&](const FChannelMemberDto& MemberDto) { return FMember{UserManager, MemberDto}; });
}
