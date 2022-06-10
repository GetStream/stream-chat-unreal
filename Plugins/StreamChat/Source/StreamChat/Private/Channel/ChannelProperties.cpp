// Copyright 2022 Stream.IO, Inc. All Rights Reserved.

#include "Channel/ChannelProperties.h"

#include "Algo/Transform.h"
#include "Response/Channel/ChannelResponseDto.h"
#include "User/UserManager.h"
#include "Util.h"

FChannelProperties::FChannelProperties() = default;

FChannelProperties::FChannelProperties(const FChannelResponseDto& Dto, UUserManager* UserManager)
    : Type{Dto.Type}
    , Id{Dto.Id}
    , Cid{Dto.Cid}
    , Config{Dto.Config}
    , MemberCount{Dto.MemberCount}
    , Members{Util::Convert<FMember>(Dto.Members, UserManager)}
    , Cooldown{Dto.Cooldown}
    , CreatedAt{Dto.CreatedAt}
    , UpdatedAt{Dto.UpdatedAt}
    , DeletedAt{Dto.DeletedAt}
    , LastMessageAt{Dto.LastMessageAt}
    , CreatedBy{UserManager->UpsertUser(Dto.CreatedBy)}
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
}

FChannelProperties::FChannelProperties(const FChannelResponseDto& Dto, const TArray<FChannelMemberDto>& InMembers, UUserManager* UserManager)
    : FChannelProperties(Dto, UserManager)
{
    if (Members.Num() == 0 && InMembers.Num() > 0)
    {
        Members = Util::Convert<FMember>(InMembers, UserManager);
    }
}

FChannelProperties::FChannelProperties(const FString& Type, const FString& Id, const FString& Team) : Type{Type}, Id{Id}, Team{Team}
{
}

FChannelProperties::operator FChannelRequestDto() const
{
    FChannelRequestDto Dto{bAutoTranslationEnabled, AutoTranslationLanguage, bDisabled, bFrozen, {}, Team, ExtraData};
    Algo::Transform(Members, Dto.Members, [&](const FMember& M) { return M.User->Id; });

    return Dto;
}

TArray<FUserRef> FChannelProperties::GetOtherMemberUsers() const
{
    const auto NotCurrentUser = [](const FMember& M)
    {
        return !M.User.IsCurrent();
    };
    const auto ToUser = [](const FMember& M)
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

FChannelProperties& FChannelProperties::SetMembers(const TArray<FMember>& InMembers)
{
    Members = InMembers;
    MemberCount = Members.Num();
    return *this;
}

FChannelProperties& FChannelProperties::SetMembers(const TArray<FUserRef>& Users)
{
    Algo::Transform(Users, Members, [&](const FUserRef& UserRef) { return FMember{UserRef}; });
    MemberCount = Members.Num();
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

FMember* FChannelProperties::GetMember(const FUserRef& User)
{
    return Members.FindByKey(User);
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

void FChannelProperties::Merge(const FChannelResponseDto& Dto, UUserManager* UserManager)
{
    FChannelProperties NewProps{Dto, UserManager};
    // Retain members
    NewProps.Members = Members;
    *this = NewProps;
}

void FChannelProperties::Merge(const FChannelResponseDto& Dto, const TArray<FChannelMemberDto>& InMembers, UUserManager* UserManager)
{
    // Replace members
    const FChannelProperties NewProps{Dto, InMembers, UserManager};
    *this = NewProps;
}

void FChannelProperties::AppendMembers(const TArray<FChannelMemberDto>& InMembers, UUserManager* UserManager)
{
    // TODO probably not the most efficient way to do this
    TSet<FMember> NewMembers(Members);
    for (auto&& M : InMembers)
    {
        NewMembers.Add(Util::Convert<FMember>(M, UserManager));
    }
    SetMembers(NewMembers.Array());
}

void UChannelPropertiesBlueprintLibrary::SetMembers_UserId(FChannelProperties& ChannelProperties, const TArray<FString>& UserIds, FChannelProperties& Out)
{
    Out = ChannelProperties.SetMembers(UserIds);
}

void UChannelPropertiesBlueprintLibrary::SetMembers_User(FChannelProperties& ChannelProperties, const TArray<FUserRef>& Users, FChannelProperties& Out)
{
    Out = ChannelProperties.SetMembers(Users);
}
