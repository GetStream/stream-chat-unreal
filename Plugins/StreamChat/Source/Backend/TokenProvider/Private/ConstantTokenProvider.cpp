// Copyright 2022 Stream.IO, Inc. All Rights Reserved.

#include "ConstantTokenProvider.h"

FConstantTokenProvider::FConstantTokenProvider(const FToken& InToken) : Token(InToken)
{
}

FToken FConstantTokenProvider::LoadToken(const FString&, bool) const
{
    return Token;
}
