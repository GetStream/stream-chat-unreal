// Copyright 2022 Stream.IO, Inc. All Rights Reserved.

#include "Channel/MessagePaginationOptions.h"

FMessagePaginationOptions::FMessagePaginationOptions(const int32 Limit, const int32 Offset) : FUserPaginationOptions{Limit, Offset}
{
}

FMessagePaginationOptions::operator FMessagePaginationParamsRequestDto() const
{
    return FMessagePaginationParamsRequestDto{
        GetLimitForDto(),
        GetOffsetForDto(),
        CreatedAtAfter,
        CreatedAtAfterOrEqual,
        CreatedAtBefore,
        CreatedAtBeforeOrEqual,
        IdGt,
        IdGte,
        IdLt,
        IdLte,
    };
}
