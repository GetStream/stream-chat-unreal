// Copyright 2022 Stream.IO, Inc. All Rights Reserved.

#include "User/UserPaginationOptions.h"

FUserPaginationOptions::FUserPaginationOptions(const int32 Limit, const int32 Offset) : FPaginationOptions{Limit, Offset}
{
}

FUserPaginationOptions::operator FPaginationParamsRequestDto() const
{
    return FPaginationParamsRequestDto{
        GetLimitForDto(),
        GetOffsetForDto(),
        IdGt,
        IdGte,
        IdLt,
        IdLte,
    };
}
