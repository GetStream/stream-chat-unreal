// Copyright 2022 Stream.IO, Inc. All Rights Reserved.

#include "User/UserPaginationOptions.h"

FUserPaginationOptions::operator FPaginationParamsRequestDto() const
{
    FPaginationParamsRequestDto Dto{static_cast<uint32>(FMath::Max(0, Limit)), Offset};
    if (!IdGt.IsEmpty())
    {
        Dto.SetIdGt(IdGt);
    }
    if (!IdGte.IsEmpty())
    {
        Dto.SetIdGte(IdGte);
    }
    if (!IdLt.IsEmpty())
    {
        Dto.SetIdLt(IdLt);
    }
    if (!IdLte.IsEmpty())
    {
        Dto.SetIdLte(IdLte);
    }
    return Dto;
}
