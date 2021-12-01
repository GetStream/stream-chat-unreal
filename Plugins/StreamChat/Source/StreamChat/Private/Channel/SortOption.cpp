// Copyright 2021 Stream.IO, Inc. All Rights Reserved.

#include "Channel/SortOption.h"

#include "Request/Channel/SortParamRequestDto.h"
#include "StreamJson.h"

FSortOption::operator FSortParamRequestDto() const
{
    return {
        Json::Serialize(Field),
        Direction == ESortDirection::Ascending ? 1 : -1,
    };
}
