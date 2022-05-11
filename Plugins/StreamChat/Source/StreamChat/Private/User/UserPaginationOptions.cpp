// Copyright 2022 Stream.IO, Inc. All Rights Reserved.

#include "User/UserPaginationOptions.h"

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
