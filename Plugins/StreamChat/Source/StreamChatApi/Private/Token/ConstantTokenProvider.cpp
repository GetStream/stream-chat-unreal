#include "Token/ConstantTokenProvider.h"

FConstantTokenProvider::FConstantTokenProvider(const FString& InToken) : Token(InToken)
{
}

FString FConstantTokenProvider::LoadToken(const FString&, bool) const
{
    return Token;
}
