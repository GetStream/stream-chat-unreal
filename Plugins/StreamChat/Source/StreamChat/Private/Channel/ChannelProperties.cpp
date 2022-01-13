// Copyright 2021 Stream.IO, Inc. All Rights Reserved.

#include "Channel/ChannelProperties.h"

#include "Algo/Transform.h"
#include "Response/Channel/ChannelResponseDto.h"
#include "User/UserManager.h"

FChannelProperties::FChannelProperties() = default;

FChannelProperties::FChannelProperties(const FChannelResponseDto& Dto, UUserManager& UserManager)
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

FChannelProperties::operator FChannelRequestDto() const
{
    FChannelRequestDto Dto{bAutoTranslationEnabled, AutoTranslationLanguage, bDisabled, bFrozen, {}, Team, ExtraData};
    Algo::Transform(Members, Dto.Members, [&](const FMember& M) { return M.User->Id; });

    return Dto;
}

FChannelProperties FChannelProperties::WithType(const FString& InType)
{
    FChannelProperties Props;
    Props.Type = InType;
    return Props;
}

TArray<FUserRef> FChannelProperties::GetOtherMemberUsers() const
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

FChannelProperties& FChannelProperties::SetMembers(const TArray<FString>& UserIds)
{
    Algo::Transform(UserIds, Members, [&](const FString& UserId) { return FMember{UUserManager::Get()->UpsertUser(UserId)}; });
    MemberCount = Members.Num();
    return *this;
}

FChannelProperties& FChannelProperties::SetMembers(const TArray<FUserRef>& Users)
{
    Algo::Transform(Users, Members, [&](const FUserRef& UserRef) { return FMember{UserRef}; });
    MemberCount = Members.Num();
    return *this;
}

FChannelProperties& FChannelProperties::SetId(const FString& InId)
{
    Id = InId;
    return *this;
}

TOptional<FString> FChannelProperties::GetName() const
{
    return ExtraData.GetString(TEXT("name"));
}

TOptional<FString> FChannelProperties::GetImageUrl() const
{
    return ExtraData.GetString(TEXT("image"));
}

FChannelProperties& FChannelProperties::SetName(const FString& Value)
{
    ExtraData.SetString(TEXT("name"), Value);
    return *this;
}

FChannelProperties& FChannelProperties::SetImageUrl(const FString& Value)
{
    ExtraData.SetString(TEXT("image"), Value);
    return *this;
}

void FChannelProperties::SetMembers(UUserManager& UserManager, const TArray<FChannelMemberDto>& Dto)
{
    Algo::Transform(Dto, Members, [&](const FChannelMemberDto& MemberDto) { return FMember{UserManager, MemberDto}; });
}

void UChannelPropertiesBlueprintLibrary::SetMembers_UserId(FChannelProperties& ChannelProperties, const TArray<FString>& UserIds, FChannelProperties& Out)
{
    Out = ChannelProperties.SetMembers(UserIds);
}

void UChannelPropertiesBlueprintLibrary::SetMembers_User(FChannelProperties& ChannelProperties, const TArray<FUserRef>& Users, FChannelProperties& Out)
{
    Out = ChannelProperties.SetMembers(Users);
}
