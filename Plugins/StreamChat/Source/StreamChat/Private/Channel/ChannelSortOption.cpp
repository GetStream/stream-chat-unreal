// Copyright 2021 Stream.IO, Inc. All Rights Reserved.

#include "Channel/ChannelSortOption.h"

#include "Request/SortParamRequestDto.h"
#include "StreamJson.h"

FChannelSortOption::operator FSortParamRequestDto() const
{
    return {
        Json::Serialize(Field),
        Direction == ESortDirection::Ascending ? 1 : -1,
    };
}

FMessageSortOption::operator FSortParamRequestDto() const
{
    return {
        Field == EMessageSortField::Custom ? CustomField.ToString() : Json::Serialize(Field),
        Direction == ESortDirection::Ascending ? 1 : -1,
    };
}

FUserSortOption::operator FSortParamRequestDto() const
{
    return {
        Field == EUserSortField::Custom ? CustomField.ToString() : Json::Serialize(Field),
        Direction == ESortDirection::Ascending ? 1 : -1,
    };
}
