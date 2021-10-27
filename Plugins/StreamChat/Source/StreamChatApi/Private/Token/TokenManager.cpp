#include "Token/TokenManager.h"

FTokenManager::FTokenManager(const FString& UserId, TUniquePtr<ITokenProvider>&& TokenProvider)
    : UserId(UserId), TokenProvider(MoveTemp(TokenProvider))
{
}

FString FTokenManager::LoadToken(const bool bRefresh) const
{
    return TokenProvider->LoadToken(bRefresh);
}
