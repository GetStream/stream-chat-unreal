// Copyright 2022 Stream.IO, Inc. All Rights Reserved.

#include "Request/Moderation/MuteUserRequestDto.h"

void FMuteUserRequestDto::SetTimeout(const float Timeout)
{
    AdditionalFields.SetNumber("timeout", Timeout);
}
