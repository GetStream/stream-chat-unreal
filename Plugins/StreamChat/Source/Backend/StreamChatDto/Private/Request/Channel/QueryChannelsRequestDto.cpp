// Copyright Stream.IO, Inc. All Rights Reserved.

#include "Request/Channel/QueryChannelsRequestDto.h"

#include "StreamJsonField.h"

void FQueryChannelsRequestDto::SerializeExtra(const FQueryChannelsRequestDto& Self, FJsonObject& JsonObject)
{
    JsonField::Serialize(Self.MemberLimit, TEXT("member_limit"), JsonObject);
    JsonField::Serialize(Self.MessageLimit, TEXT("message_limit"), JsonObject);
    JsonField::Serialize(Self.Limit, TEXT("limit"), JsonObject);
    JsonField::Serialize(Self.Offset, TEXT("offset"), JsonObject);
}
