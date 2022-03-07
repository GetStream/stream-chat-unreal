// Copyright 2022 Stream.IO, Inc. All Rights Reserved.

#include "Response/Message/MessageDto.h"

TOptional<FDateTime> FMessageDto::GetDeletedAt() const
{
    return AdditionalFields.Get<FDateTime>(TEXT("deleted_at"));
}
