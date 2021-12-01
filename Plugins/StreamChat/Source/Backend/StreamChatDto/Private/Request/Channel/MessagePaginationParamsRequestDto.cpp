// Copyright 2021 Stream.IO, Inc. All Rights Reserved.

#include "Request/Channel/MessagePaginationParamsRequestDto.h"

#include "StreamJsonField.h"

void FMessagePaginationParamsRequestDto::SerializeExtra(const FMessagePaginationParamsRequestDto& Self, FJsonObject& JsonObject)
{
    JsonField::Serialize(Self.IdLt, TEXT("id_lt"), JsonObject);
    JsonField::Serialize(Self.IdLte, TEXT("id_lte"), JsonObject);
    JsonField::Serialize(Self.IdGt, TEXT("id_gt"), JsonObject);
    JsonField::Serialize(Self.IdGte, TEXT("id_gte"), JsonObject);
    JsonField::Serialize(Self.CreatedAtAfter, TEXT("created_at_after"), JsonObject);
    JsonField::Serialize(Self.CreatedAtAfterOrEqual, TEXT("created_at_after_or_equal"), JsonObject);
    JsonField::Serialize(Self.CreatedAtBefore, TEXT("created_at_before"), JsonObject);
    JsonField::Serialize(Self.CreatedAtBeforeOrEqual, TEXT("created_at_before_or_equal"), JsonObject);
}
