// Copyright 2022 Stream.IO, Inc. All Rights Reserved.

#include "Channel/MessagePaginationOptions.h"

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
