// Copyright 2022 Stream.IO, Inc. All Rights Reserved.

#include "Response/ErrorResponseDto.h"

bool FErrorResponseDto::IsTokenExpired() const
{
    return Code == 40;
}
