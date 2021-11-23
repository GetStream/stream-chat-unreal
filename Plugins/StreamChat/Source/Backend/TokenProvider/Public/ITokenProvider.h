// Copyright Stream.IO, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"

class TOKENPROVIDER_API ITokenProvider
{
public:
    virtual ~ITokenProvider() = default;
    virtual FString LoadToken(const FString& UserId, bool bRefresh = false) const = 0;
};
