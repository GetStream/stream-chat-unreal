// Copyright 2022 Stream.IO, Inc. All Rights Reserved.

#include "Channel/MessagePaginationOptions.h"

FMessagePaginationOptions::operator FMessagePaginationParamsRequestDto() const
{
    check(Limit >= 0);
    check(Offset >= 0);
    return FMessagePaginationParamsRequestDto{
        static_cast<uint32>(Limit),
        static_cast<uint32>(Offset),
        CreatedAtAfter,
        CreatedAtAfterOrEqual,
        CreatedAtBefore,
        CreatedAtBeforeOrEqual,
        IdGt,
        IdGte,
        IdLt,
        IdLte};
}
