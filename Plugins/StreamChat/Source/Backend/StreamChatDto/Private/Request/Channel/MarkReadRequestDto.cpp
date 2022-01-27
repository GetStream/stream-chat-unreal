// Copyright 2021 Stream.IO, Inc. All Rights Reserved.

#include "Request/Channel/MarkReadRequestDto.h"

void FMarkReadRequestDto::SetMessageId(const FString& Value)
{
    if (!Value.IsEmpty())
    {
        AdditionalFields.SetString(TEXT("message_id"), Value);
    }
}
