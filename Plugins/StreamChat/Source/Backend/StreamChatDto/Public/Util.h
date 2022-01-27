// Copyright 2021 Stream.IO, Inc. All Rights Reserved.

#pragma once

/**
 * @brief Helpers for DTO conversion
 *
 * To use these, define either:
 * a converting constructor (https://en.cppreference.com/w/cpp/language/converting_constructor), or
 * a user-defined conversion function (https://en.cppreference.com/w/cpp/language/cast_operator)
 * @ingroup StreamChatDto
 */
namespace Util
{
/// Map from one type to an another using direct initialization
template <typename OutT, typename InT>
FORCEINLINE OutT Convert(const InT& Input)
{
    return static_cast<OutT>(Input);
}

/// Map from one type to an another using a constructor + additional arguments
template <typename OutT, typename InT, typename... TArgs>
FORCEINLINE OutT Convert(const InT& Input, TArgs... Args)
{
    return OutT{Input, Args...};
}

/// Map from an optional of one type to an optional of another using direct initialization
template <typename OutT, typename InT>
FORCEINLINE TOptional<OutT> Convert(const TOptional<InT>& Input)
{
    if (Input.IsSet())
    {
        return TOptional<OutT>{static_cast<OutT>(Input.GetValue())};
    }
    return {};
}

/// Map from an pointer of one type to an optional of another using direct initialization
template <typename OutT, typename InT>
FORCEINLINE TOptional<OutT> Convert(const InT* Input)
{
    if (Input != nullptr)
    {
        return TOptional<OutT>{static_cast<OutT>(*Input)};
    }
    return {};
}

/// Map from an optional array of one type to an optional array of another using direct initialization
template <typename OutT, typename InT>
FORCEINLINE TOptional<TArray<OutT>> Convert(const TOptional<TArray<InT>>& Input)
{
    if (Input.IsSet())
    {
        return TOptional<TArray<OutT>>{Util::Convert<OutT, InT>(Input.GetValue())};
    }
    return {};
}

/// Map from an array of one type to an array of another using its direct initialization
template <typename OutT, typename InT>
FORCEINLINE TArray<OutT> Convert(const TArray<InT>& Input)
{
    TArray<OutT> Output;
    Output.Reserve(Input.Num());
    for (const InT& Value : Input)
    {
        Output.Emplace(static_cast<OutT>(Value));
    }
    return Output;
}

/// Map from an array of one type to an array of another using a constructor and any other arguments
template <typename OutT, typename InT, typename... TArgs>
FORCEINLINE TArray<OutT> Convert(const TArray<InT>& Input, TArgs... Args)
{
    TArray<OutT> Output;
    Output.Reserve(Input.Num());
    for (const InT& Value : Input)
    {
        Output.Emplace(OutT{Value, Args...});
    }
    return Output;
}
}    // namespace Util
