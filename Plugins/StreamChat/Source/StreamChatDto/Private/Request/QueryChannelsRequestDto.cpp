﻿#include "Request/QueryChannelsRequestDto.h"

#include "StreamJson.h"

void FQueryChannelsRequestDto::SerializeExtra(const FQueryChannelsRequestDto& Self, FJsonObject& JsonObject)
{
    Json::SerializeField(Self.MemberLimit, TEXT("member_limit"), JsonObject);
    Json::SerializeField(Self.MessageLimit, TEXT("message_limit"), JsonObject);
}