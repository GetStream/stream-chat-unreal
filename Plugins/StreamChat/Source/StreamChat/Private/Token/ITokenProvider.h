﻿#pragma once

class ITokenProvider
{
public:
    virtual ~ITokenProvider() = default;
    virtual FString LoadToken() const = 0;
};