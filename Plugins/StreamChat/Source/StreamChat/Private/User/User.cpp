// Copyright 2022 Stream.IO, Inc. All Rights Reserved.

#include "User/User.h"

#include "Algo/Transform.h"
#include "OwnUserDto.h"
#include "Request/User/UserObjectRequestDto.h"
#include "UserObjectDto.h"
#include "Util.h"

FUser::FUser() = default;

FUser::FUser(const FUserObjectDto& Dto)
    : Id{Dto.Id}
    , bOnline{Dto.bOnline}
    , bInvisible{Dto.bInvisible}
    , CreatedAt(Dto.CreatedAt)
    , UpdatedAt{Dto.UpdatedAt}
    , DeactivatedAt{Dto.DeactivatedAt}
    , DeletedAt{Dto.DeletedAt}
    , LastActive{Dto.LastActive}
    , BanExpires{Dto.BanExpires}
    , bBanned{Dto.bBanned}
    , Language{Dto.Language}
    , RevokeTokensIssuedBefore{Dto.RevokeTokensIssuedBefore}
    , Role{Dto.Role}
    , Teams{Dto.Teams}
    , Name{Dto.AdditionalFields.GetString(TEXT("name")).Get(TEXT(""))}
    , Image{Dto.AdditionalFields.GetString(TEXT("image")).Get(TEXT(""))}
{
}

FUser::FUser(const FOwnUserDto& Dto, UUserManager* UserManager)
    : Id{Dto.Id}
    , bOnline{Dto.bOnline}
    , bInvisible{Dto.bInvisible}
    , CreatedAt(Dto.CreatedAt)
    , UpdatedAt{Dto.UpdatedAt}
    , LastActive{Dto.LastActive}
    , UnreadCount(Dto.UnreadCount)
    , TotalUnreadCount(Dto.TotalUnreadCount)
    , UnreadChannels(Dto.UnreadChannels)
    , MutedUsers{Util::Convert<FMutedUser>(Dto.Mutes, UserManager)}
    , BanExpires{Dto.BanExpires}
    , bBanned{Dto.bBanned}
    , Language{Dto.Language}
    , RevokeTokensIssuedBefore{Dto.RevokeTokensIssuedBefore}
    , Role{Dto.Role}
    , Teams{Dto.Teams}
    , Name{Dto.AdditionalFields.GetString(TEXT("name")).Get(TEXT(""))}
    , Image{Dto.AdditionalFields.GetString(TEXT("image")).Get(TEXT(""))}
{
}

FUser::operator FUserDto() const
{
    FAdditionalFields AdditionalFields;
    if (!Name.IsEmpty())
    {
        AdditionalFields.SetString(TEXT("name"), Name);
    }
    if (!Image.IsEmpty())
    {
        AdditionalFields.SetString(TEXT("image"), Image);
    }
    return FUserDto{
        Id,
        BanExpires,
        bBanned,
        bInvisible,
        Language,
        RevokeTokensIssuedBefore,
        Role,
        Teams,
        MoveTemp(AdditionalFields),
    };
}

FUser::operator FUserObjectDto() const
{
    return FUserObjectDto{FUserDto{*this}, CreatedAt, UpdatedAt, DeactivatedAt, DeletedAt, LastActive, bOnline};
}

FUser::operator FUserObjectRequestDto() const
{
    return FUserObjectRequestDto{FUserDto{*this}};
}

void FUser::Update(const FUser& User)
{
    // TODO Devices
    // TODO Channel mutes
    bBanned = User.bBanned;
    bOnline = User.bOnline;
    bInvisible = User.bInvisible;
    MutedUsers = User.MutedUsers;
    UnreadCount = User.UnreadCount;
    TotalUnreadCount = User.TotalUnreadCount;

    if (User.CreatedAt.GetTicks() != 0)
    {
        CreatedAt = User.CreatedAt;
    }
    if (User.UpdatedAt.GetTicks() != 0)
    {
        UpdatedAt = User.UpdatedAt;
    }
    if (User.LastActive.GetTicks() != 0)
    {
        LastActive = User.LastActive;
    }
    if (!User.Role.IsEmpty())
    {
        Role = User.Role;
    }
    if (!User.Language.IsEmpty())
    {
        Language = User.Language;
    }
    if (!User.Name.IsEmpty())
    {
        Name = User.Name;
    }
    if (!User.Image.IsEmpty())
    {
        Image = User.Image;
    }
}

FUser::FUser(const FString& InId) : Id(InId)
{
}

FString FUser::GetInitials(const int32 Limit) const
{
    FString Initials;
    Initials.Reserve(Limit);
    int32 Pos = 0;
    while (Pos < Name.Len())
    {
        while (Pos < Name.Len() && FChar::IsWhitespace((*Name)[Pos]))
        {
            Pos++;
        }
        Initials.AppendChar(Name[Pos]);
        if (Initials.Len() >= Limit)
        {
            break;
        }
        while (Pos < Name.Len() && !FChar::IsWhitespace((*Name)[Pos]))
        {
            Pos++;
        }
    }
    Initials.ToUpperInline();
    return Initials;
}

bool UUserBlueprintLibrary::GenerateUserId(const FString& Name, FString& OutUserId)
{
    bool bHadValidCharacter = false;
    bool bPrevAlphaNum = false;
    OutUserId.Empty(Name.Len());
    for (auto Iter = Name.CreateConstIterator(); Iter; ++Iter)
    {
        if (OutUserId.Len() >= 32 || *Iter == TEXT('\0'))
        {
            break;
        }
        if (FChar::IsDigit(*Iter))
        {
            bHadValidCharacter = true;
            bPrevAlphaNum = true;
            OutUserId.AppendChar(*Iter);
            continue;
        }
        if (FChar::IsLower(*Iter))
        {
            bHadValidCharacter = true;
            bPrevAlphaNum = true;
            OutUserId.AppendChar(*Iter);
            continue;
        }
        if (FChar::IsUpper(*Iter))
        {
            OutUserId.AppendChar(FChar::ToLower(*Iter));
            bHadValidCharacter = true;
            bPrevAlphaNum = true;
            continue;
        }
        if (bPrevAlphaNum)
        {
            OutUserId.AppendChar(TEXT('_'));
        }
        else
        {
            // Ignore
        }
        bPrevAlphaNum = false;
    }
    return bHadValidCharacter;
}

bool UUserBlueprintLibrary::SanitizeName(const FString& Name, FString& SanitizedName)
{
    bool bHadValidCharacter = false;
    bool bPrevAlphaNum = false;
    SanitizedName.Empty(Name.Len());
    for (auto Iter = Name.CreateConstIterator(); Iter; ++Iter)
    {
        if (FChar::IsAlnum(*Iter))
        {
            bHadValidCharacter = true;
            bPrevAlphaNum = true;
            SanitizedName.AppendChar(*Iter);
            continue;
        }
        if (FChar::IsWhitespace(*Iter))
        {
            if (bPrevAlphaNum)
            {
                SanitizedName.AppendChar(TEXT(' '));
            }
            else
            {
                // Skip
            }
        }
        bPrevAlphaNum = false;
    }
    return bHadValidCharacter;
}
