#pragma once

#include "CoreMinimal.h"

class STREAMCHATAPI_API ITokenProvider
{
public:
    virtual ~ITokenProvider() = default;
    virtual FString LoadToken(const FString& UserId, bool bRefresh = false) const = 0;
};
