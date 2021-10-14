#pragma once

#include "CoreMinimal.h"

class STREAMCHATAPI_API ITokenProvider
{
public:
    virtual ~ITokenProvider() = default;
    virtual FString LoadToken() const = 0;
};
