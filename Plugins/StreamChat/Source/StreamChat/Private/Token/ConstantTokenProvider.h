#pragma once
#include "ITokenProvider.h"

class FConstantTokenProvider final : public ITokenProvider
{
public:
	explicit FConstantTokenProvider(const FString& InToken);
	virtual FString LoadToken() const override;

private:
	FString Token;
};
