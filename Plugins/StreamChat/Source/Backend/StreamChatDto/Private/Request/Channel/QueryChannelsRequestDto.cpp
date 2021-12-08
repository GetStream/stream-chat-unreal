// Copyright 2021 Stream.IO, Inc. All Rights Reserved.

#include "Request/Channel/QueryChannelsRequestDto.h"

#include "StreamJsonField.h"

void FQueryChannelsRequestDto::SetLimit(const int32 Value)
{
    AdditionalFields.SetNumber(TEXT("limit"), Value);
}

void FQueryChannelsRequestDto::SetMemberLimit(const int32 Value)
{
    AdditionalFields.SetNumber(TEXT("member_limit"), Value);
}

void FQueryChannelsRequestDto::SetMessageLimit(const int32 Value)
{
    AdditionalFields.SetNumber(TEXT("message_limit"), Value);
}

void FQueryChannelsRequestDto::SetOffset(const int32 Value)
{
    AdditionalFields.SetNumber(TEXT("offset"), Value);
}
