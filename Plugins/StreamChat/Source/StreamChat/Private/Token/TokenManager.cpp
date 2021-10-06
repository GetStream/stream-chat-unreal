#include "TokenManager.h"

void FTokenManager::SetTokenProvider(TUniquePtr<ITokenProvider> InTokenProvider)
{
    TokenProvider = MoveTemp(InTokenProvider);
}

void FTokenManager::Reset()
{
    TokenProvider.Reset();
}

FString FTokenManager::LoadToken() const
{
    return TokenProvider->LoadToken();
}
