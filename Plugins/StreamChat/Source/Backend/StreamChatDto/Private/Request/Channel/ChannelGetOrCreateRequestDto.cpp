// Copyright 2021 Stream.IO, Inc. All Rights Reserved.

#include "Request/Channel/ChannelGetOrCreateRequestDto.h"

#include "StreamJsonField.h"

void FChannelGetOrCreateRequestDto::SerializeExtra(const FChannelGetOrCreateRequestDto& Self, FJsonObject& JsonObject)
{
    JsonField::Serialize(Self.Members, TEXT("members"), JsonObject);
    JsonField::Serialize(Self.Messages, TEXT("messages"), JsonObject);
    JsonField::Serialize(Self.Watchers, TEXT("watchers"), JsonObject);
}
