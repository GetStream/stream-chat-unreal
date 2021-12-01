// Copyright 2021 Stream.IO, Inc. All Rights Reserved.

#include "Event/Abstract/MessageEvent.h"

#include "Detail/ExtraFields.h"
#include "Dom/JsonObject.h"

void FMessageEvent::DeserializeExtra(const FJsonObject& JsonObject, FMessageEvent& Event)
{
    ExtraFields::InvokeDeserializeExtra(*JsonObject.GetObjectField(TEXT("message")), Event.Message);
}
