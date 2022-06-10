// Copyright 2022 Stream.IO, Inc. All Rights Reserved.

#include "PaginationOptions.h"

FPaginationOptions::FPaginationOptions(const int32 Limit, const int32 Offset) : Limit{Limit}, Offset{Offset}
{
}

TOptional<uint32> FPaginationOptions::GetLimitAsOptional() const
{
    return Limit >= 0 ? static_cast<uint32>(Limit) : TOptional<uint32>{};
}

TOptional<uint32> FPaginationOptions::GetOffsetAsOptional() const
{
    return Offset >= 0 ? static_cast<uint32>(Offset) : TOptional<uint32>{};
}

uint32 FPaginationOptions::GetLimitForDto() const
{
    return Limit >= 0 ? static_cast<uint32>(Limit) : TNumericLimits<uint32>::Max();
}

uint32 FPaginationOptions::GetOffsetForDto() const
{
    return Offset >= 0 ? static_cast<uint32>(Offset) : TNumericLimits<uint32>::Max();
}
