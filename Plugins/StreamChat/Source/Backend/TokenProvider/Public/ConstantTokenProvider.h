// Copyright 2022 Stream.IO, Inc. All Rights Reserved.

#pragma once

#include "ITokenProvider.h"
#include "Token.h"

class TOKENPROVIDER_API FConstantTokenProvider final : public ITokenProvider
{
public:
    explicit FConstantTokenProvider(const FToken& InToken);
    virtual FToken LoadToken(const FString&, bool) const override;

private:
    FToken Token;
};
