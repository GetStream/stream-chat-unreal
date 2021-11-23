// Copyright Stream.IO, Inc. All Rights Reserved.

#pragma once

#include "ITokenProvider.h"

class TOKENPROVIDER_API FConstantTokenProvider final : public ITokenProvider
{
public:
    explicit FConstantTokenProvider(const FString& InToken);
    virtual FString LoadToken(const FString&, bool) const override;

private:
    FString Token;
};
