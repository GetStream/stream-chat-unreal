// Copyright 2021 Stream.IO, Inc. All Rights Reserved.

#include "Channel/ChannelId.h"

#include "Algo/Transform.h"
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
    , ExtraData(Dto.AdditionalFields)
{
    SetMembers(UserManager, Dto.Members);
}

FChannelId::operator FChannelRequestDto() const
{
    FChannelRequestDto Dto{bAutoTranslationEnabled, AutoTranslationLanguage, bDisabled, bFrozen, {}, Team, ExtraData};
    Algo::Transform(Members, Dto.Members, [&](const FMember& M) { return M.User->Id; });

    return Dto;
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

void FChannelId::SetMembers(const TArray<FString>& UserIds)
{
    Algo::Transform(UserIds, Members, [&](const FString& UserId) { return FMember{UUserManager::Get()->UpsertUser(UserId)}; });
    MemberCount = Members.Num();
}

void FChannelId::SetMembers(const TArray<FUserRef>& Users)
{
    Algo::Transform(Users, Members, [&](const FUserRef& UserRef) { return FMember{UserRef}; });
    MemberCount = Members.Num();
}

TOptional<FString> FChannelId::GetName() const
{
    return ExtraData.GetString(TEXT("name"));
}

TOptional<FString> FChannelId::GetImageUrl() const
{
    return ExtraData.GetString(TEXT("image"));
}

void FChannelId::SetMembers(UUserManager& UserManager, const TArray<FChannelMemberDto>& Dto)
{
    Algo::Transform(Dto, Members, [&](const FChannelMemberDto& MemberDto) { return FMember{UserManager, MemberDto}; });
}

void UChannelIdBlueprintLibrary::SetMembers_UserId(FChannelId& ChannelId, const TArray<FString>& UserIds, FChannelId& Out)
{
    ChannelId.SetMembers(UserIds);
    Out = ChannelId;
}

void UChannelIdBlueprintLibrary::SetMembers_User(FChannelId& ChannelId, const TArray<FUserRef>& Users, FChannelId& Out)
{
    ChannelId.SetMembers(Users);
    Out = ChannelId;
}
