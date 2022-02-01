// Copyright 2021 Stream.IO, Inc. All Rights Reserved.

#include "TokenManager.h"

void FTokenManager::SetTokenProvider(TUniquePtr<ITokenProvider> InTokenProvider, const FString& InUserId)
{
    TokenProvider = MoveTemp(InTokenProvider);
    UserId = InUserId;
}

void FTokenManager::Reset()
{
    TokenProvider.Reset();
}

FToken FTokenManager::LoadToken(const bool bRefresh) const
{
    return TokenProvider->LoadToken(UserId, bRefresh);
}
