// Copyright 2021 Stream.IO, Inc. All Rights Reserved.

#include "ConstantTokenProvider.h"

FConstantTokenProvider::FConstantTokenProvider(const FString& InToken) : Token(InToken)
{
}

FString FConstantTokenProvider::LoadToken(const FString&, bool) const
{
    return Token;
}
