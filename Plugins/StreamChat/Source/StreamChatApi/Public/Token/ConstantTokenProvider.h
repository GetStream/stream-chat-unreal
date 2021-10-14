#pragma once

#include "Token/ITokenProvider.h"

class STREAMCHATAPI_API FConstantTokenProvider final : public ITokenProvider
{
public:
    explicit FConstantTokenProvider(const FString& InToken);
    virtual FString LoadToken() const override;

private:
    FString Token;
};
