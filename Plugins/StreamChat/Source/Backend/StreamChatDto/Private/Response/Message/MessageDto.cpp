// Copyright Stream.IO, Inc. All Rights Reserved.

#include "Response/Message/MessageDto.h"

#include "StreamJsonField.h"

void FMessageDto::DeserializeExtra(const FJsonObject& JsonObject, FMessageDto& Self)
{
    JsonField::Deserialize(JsonObject, TEXT("deleted_at"), Self.DeletedAt);
}
