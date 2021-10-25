#include "Request/Channel/ChannelGetOrCreateRequestDto.h"

#include "StreamJson.h"

void FChannelGetOrCreateRequestDto::SerializeExtra(const FChannelGetOrCreateRequestDto& Self, FJsonObject& JsonObject)
{
    Json::SerializeField(Self.Members, TEXT("members"), JsonObject);
    Json::SerializeField(Self.Messages, TEXT("messages"), JsonObject);
    Json::SerializeField(Self.Watchers, TEXT("watchers"), JsonObject);
}
