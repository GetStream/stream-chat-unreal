#pragma once

#include "CoreMinimal.h"
#include "ITokenProvider.h"

class STREAMCHATAPI_API FTokenManager
{
public:
    explicit FTokenManager(const FString& UserId, TUniquePtr<ITokenProvider>&& TokenProvider);

    FString LoadToken(bool bRefresh = false) const;

private:
    FString UserId;
    TUniquePtr<ITokenProvider> TokenProvider;
};
