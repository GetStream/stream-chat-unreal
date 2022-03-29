// Copyright 2022 Stream.IO, Inc. All Rights Reserved.

#include "Request/PaginatedRequest.h"

FPaginatedRequest::FPaginatedRequest(const TOptional<uint32>& Limit, const TOptional<uint32>& Offset, const FJsonObjectWrapper& Filter)
    : FilterConditions{Filter}
{
    if (Limit.IsSet())
    {
        SetLimit(Limit.GetValue());
    }
    if (Offset.IsSet())
    {
        SetOffset(Offset.GetValue());
    }
}

void FPaginatedRequest::SetLimit(const uint32 Value)
{
    Pagination.SetNumber(TEXT("limit"), Value);
}

void FPaginatedRequest::SetOffset(const uint32 Value)
{
    Pagination.SetNumber(TEXT("offset"), Value);
}
