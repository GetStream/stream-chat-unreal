// Copyright 2022 Stream.IO, Inc. All Rights Reserved.

#include "Channel/MessagePaginationOptions.h"

FMessagePaginationOptions::operator FMessagePaginationParamsRequestDto() const
{
    FMessagePaginationParamsRequestDto Dto{static_cast<uint32>(FMath::Max(0, Limit)), Offset};
    if (CreatedAtAfter.GetTicks() > 0)
    {
        Dto.SetCreatedAtAfter(CreatedAtAfter);
    }
    if (CreatedAtAfterOrEqual.GetTicks() > 0)
    {
        Dto.SetCreatedAtAfter(CreatedAtAfterOrEqual);
    }
    if (CreatedAtBefore.GetTicks() > 0)
    {
        Dto.SetCreatedAtAfter(CreatedAtBefore);
    }
    if (CreatedAtBeforeOrEqual.GetTicks() > 0)
    {
        Dto.SetCreatedAtAfter(CreatedAtBeforeOrEqual);
    }
    if (!IdLt.IsEmpty())
    {
        Dto.SetIdLt(IdLt);
    }
    if (!IdLte.IsEmpty())
    {
        Dto.SetIdLt(IdLte);
    }
    if (!IdGt.IsEmpty())
    {
        Dto.SetIdLt(IdGt);
    }
    if (!IdGte.IsEmpty())
    {
        Dto.SetIdLt(IdGte);
    }
    return Dto;
}
