// Copyright 2022 Stream.IO, Inc. All Rights Reserved.

#include "User/UserPaginationOptions.h"

FUserPaginationOptions::operator FPaginationParamsRequestDto() const
{
    check(Limit >= 0);
    check(Offset >= 0);
    return FPaginationParamsRequestDto{
        static_cast<uint32>(Limit),
        static_cast<uint32>(Offset),
        IdGt,
        IdGte,
        IdLt,
        IdLte,
    };
}
