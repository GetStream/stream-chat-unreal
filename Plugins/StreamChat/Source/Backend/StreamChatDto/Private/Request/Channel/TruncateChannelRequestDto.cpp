// Copyright 2022 Stream.IO, Inc. All Rights Reserved.

#include "Request/Channel/TruncateChannelRequestDto.h"

void FTruncateChannelRequestDto::SetMessage(const FMessageRequestDto& Message)
{
    AdditionalFields.Set(TEXT("message"), Message);
}
