// Copyright 2021 Stream.IO, Inc. All Rights Reserved.

#include "Request/Channel/ChannelGetOrCreateRequestDto.h"

#include "StreamJsonField.h"

void FChannelGetOrCreateRequestDto::SetMembers(const FPaginationParamsRequestDto& Value)
{
    return AdditionalFields.Set(TEXT("members"), Value);
}

void FChannelGetOrCreateRequestDto::SetMessages(const FMessagePaginationParamsRequestDto& Value)
{
    return AdditionalFields.Set(TEXT("messages"), Value);
}

void FChannelGetOrCreateRequestDto::SetWatchers(const FPaginationParamsRequestDto& Value)
{
    return AdditionalFields.Set(TEXT("watchers"), Value);
}
