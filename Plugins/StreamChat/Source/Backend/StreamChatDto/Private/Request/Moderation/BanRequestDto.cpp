// Copyright 2022 Stream.IO, Inc. All Rights Reserved.

#include "Request/Moderation/BanRequestDto.h"

void FBanRequestDto::SetTimeout(const float Timeout)
{
    AdditionalFields.SetNumber("timeout", Timeout);
}
