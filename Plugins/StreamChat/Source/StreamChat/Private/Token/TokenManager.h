#pragma once
#include "ITokenProvider.h"

class FTokenManager
{
public:
    void SetTokenProvider(TUniquePtr<ITokenProvider> InTokenProvider);
    void Reset();
    FString LoadToken() const;

private:
    TUniquePtr<ITokenProvider> TokenProvider;
};
