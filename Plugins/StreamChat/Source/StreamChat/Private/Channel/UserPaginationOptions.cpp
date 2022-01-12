// Copyright 2021 Stream.IO, Inc. All Rights Reserved.

#include "Channel/UserPaginationOptions.h"

FUserPaginationOptions::operator FPaginationParamsRequestDto() const
{
    return FPaginationParamsRequestDto{static_cast<uint32>(FMath::Max(0, Limit)), Offset};
}
