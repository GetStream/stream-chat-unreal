#include "Request/Channel/QueryChannelsRequestDto.h"

#include "StreamJson.h"

void FQueryChannelsRequestDto::SerializeExtra(const FQueryChannelsRequestDto& Self, FJsonObject& JsonObject)
{
    Json::SerializeField(Self.MemberLimit, TEXT("member_limit"), JsonObject);
    Json::SerializeField(Self.MessageLimit, TEXT("message_limit"), JsonObject);
    Json::SerializeField(Self.Limit, TEXT("limit"), JsonObject);
    Json::SerializeField(Self.Offset, TEXT("offset"), JsonObject);
}
