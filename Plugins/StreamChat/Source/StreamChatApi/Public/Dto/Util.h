#pragma once

/**
 * To use these, define either:
 * a converting constructor (https://en.cppreference.com/w/cpp/language/converting_constructor), or
 * a user-defined conversion function (https://en.cppreference.com/w/cpp/language/cast_operator)
 */
namespace Util
{

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

/// Map from an optional array of one type to an optional array of another using direct initialization
template <typename OutT, typename InT>
FORCEINLINE TOptional<TArray<OutT>> Convert(const TOptional<TArray<InT>>& Input)
{
    if (Input.IsSet())
    {
        return TOptional<TArray<OutT>>{Util::Convert<OutT>(Input.GetValue())};
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
}    // namespace Util
