#include "ConstantTokenProvider.h"

FConstantTokenProvider::FConstantTokenProvider(const FString& InToken) : Token(InToken)
{
}

FString FConstantTokenProvider::LoadToken() const
{
	return Token;
}
