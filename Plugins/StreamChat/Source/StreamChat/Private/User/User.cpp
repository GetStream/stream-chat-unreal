// Copyright 2021 Stream.IO, Inc. All Rights Reserved.

#include "User/User.h"

#include "OwnUserDto.h"
#include "UserObjectDto.h"

FUser::FUser() = default;

FUser::FUser(const FUserObjectDto& Dto)
    : Id{Dto.Id}
    , bOnline{Dto.bOnline}
    , bInvisible{Dto.bInvisible}
    , CreatedAt(Dto.CreatedAt)
    , UpdatedAt{Dto.UpdatedAt}
    , LastActive{Dto.LastActive}
    , Name{Dto.Name}
    , Image{Dto.Image}
{
}

FUser::FUser(const FOwnUserDto& Dto)
    : Id{Dto.Id}
    , bOnline{Dto.bOnline}
    , bInvisible{Dto.bInvisible}
    , CreatedAt(Dto.CreatedAt)
    , UpdatedAt{Dto.UpdatedAt}
    , LastActive{Dto.LastActive}
    , TotalUnreadCount(Dto.TotalUnreadCount)
    , UnreadChannels(Dto.UnreadChannels)
    , Name{Dto.Name}
    , Image{Dto.Image}
{
}

FUser::operator FUserObjectDto() const
{
    return {Id, bOnline, CreatedAt, UpdatedAt, LastActive, bInvisible, Name, Image};
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

FString UUserBlueprintLibrary::GenerateUserId(const FString& Name)
{
    FString UserId = Name;
    for (auto Iter = UserId.CreateIterator(); Iter; ++Iter)
    {
        if (FChar::IsDigit(*Iter))
        {
            continue;
        }
        if (FChar::IsLower(*Iter))
        {
            continue;
        }
        if (*Iter == TEXT('@') || *Iter == TEXT('_') || *Iter == TEXT('-') || *Iter == TEXT('\0'))
        {
            continue;
        }
        if (FChar::IsUpper(*Iter))
        {
            *Iter = FChar::ToLower(*Iter);
            continue;
        }
        *Iter = TEXT('_');
    }
    return UserId;
}
