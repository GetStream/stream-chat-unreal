// Copyright 2021 Stream.IO, Inc. All Rights Reserved.

#include "Channel/ChannelProperties.h"

#include "Algo/Transform.h"
#include "Response/Channel/ChannelResponseDto.h"
#include "User/UserManager.h"

FChannelProperties::FChannelProperties() = default;

FChannelProperties::FChannelProperties(const FChannelStateResponseFieldsDto& Dto, UUserManager& UserManager)
    : Type{Dto.Channel.Type}
    , Id{Dto.Channel.Id}
    , Cid{Dto.Channel.Cid}
    , Config{Dto.Channel.Config}
    , MemberCount{Dto.Channel.MemberCount}
    , Cooldown{Dto.Channel.Cooldown}
    , CreatedAt{Dto.Channel.CreatedAt}
    , UpdatedAt{Dto.Channel.UpdatedAt}
    , DeletedAt{Dto.Channel.DeletedAt}
    , LastMessageAt{Dto.Channel.LastMessageAt}
    , CreatedBy{UserManager.UpsertUser(Dto.Channel.CreatedBy)}
    , bDisabled{Dto.Channel.bDisabled}
    , bFrozen{Dto.Channel.bFrozen}
    , bHidden{Dto.Channel.bHidden}
    , bMuted{Dto.Channel.bMuted}
    , MuteExpiresAt{Dto.Channel.MuteExpiresAt}
    , HideMessagesBefore{Dto.Channel.HideMessagesBefore}
    , OwnCapabilities{Dto.Channel.OwnCapabilities}
    , bAutoTranslationEnabled{Dto.Channel.bAutoTranslationEnabled}
    , AutoTranslationLanguage{Dto.Channel.AutoTranslationLanguage}
    , Team{Dto.Channel.Team}
    , ExtraData(Dto.Channel.AdditionalFields)
{
    // For whatever reason the Dto.Channel.Members field is always empty. Use the outer field instead
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
