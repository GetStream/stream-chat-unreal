// Copyright Stream.IO, Inc. All Rights Reserved.

#include "Response/Message/MessageResponseDto.h"

#include "Detail/ExtraFields.h"
#include "Dom/JsonObject.h"

void FMessageResponseDto::DeserializeExtra(const FJsonObject& JsonObject, FMessageResponseDto& Dto)
{
    ExtraFields::InvokeDeserializeExtra(*JsonObject.GetObjectField(TEXT("message")), Dto.Message);
}
