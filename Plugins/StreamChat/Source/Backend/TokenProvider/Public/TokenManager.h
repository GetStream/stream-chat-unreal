// Copyright Stream.IO, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "ITokenProvider.h"

class TOKENPROVIDER_API FTokenManager
{
public:
    void SetTokenProvider(TUniquePtr<ITokenProvider> InTokenProvider, const FString& InUserId);
    void Reset();
    FString LoadToken(bool bRefresh = false) const;

private:
    TUniquePtr<ITokenProvider> TokenProvider;
    FString UserId;
};
