#pragma once

#include "CoreMinimal.h"
#include "ITokenProvider.h"

class STREAMCHATAPI_API FTokenManager
{
public:
    void SetTokenProvider(TUniquePtr<ITokenProvider> InTokenProvider);
    void Reset();
    FString LoadToken() const;

private:
    TUniquePtr<ITokenProvider> TokenProvider;
};
