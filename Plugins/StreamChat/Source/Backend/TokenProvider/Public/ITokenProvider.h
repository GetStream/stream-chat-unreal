// Copyright 2021 Stream.IO, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Token.h"

class TOKENPROVIDER_API ITokenProvider
{
public:
    virtual ~ITokenProvider() = default;
    virtual FToken LoadToken(const FString& UserId, bool bRefresh = false) const = 0;
};
