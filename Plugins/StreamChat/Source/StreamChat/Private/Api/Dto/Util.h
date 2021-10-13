#pragma once

namespace Util
{

/// Map from an optional of one type to an optional of another using its constructor
template <typename OutT, typename InT>
FORCEINLINE TOptional<OutT> Convert(const TOptional<InT>& Input)
{
    if (Input.IsSet())
    {
        return TOptional<OutT>{OutT{Input.GetValue()}};
    }
    return {};
}

/// Map from an optional array of one type to an optional array of another using its constructor
template <typename OutT, typename InT>
FORCEINLINE TOptional<TArray<OutT>> Convert(const TOptional<TArray<InT>>& Input)
{
    if (Input.IsSet())
    {
        return TOptional<TArray<OutT>>{Convert<OutT>(Input.GetValue())};
    }
    return {};
}

/// Map from an array of one type to an array of another using its constructor
template <typename OutT, typename InT>
FORCEINLINE TArray<OutT> Convert(const TArray<InT>& Input)
{
    TArray<OutT> Output;
    Output.Reserve(Input.Num());
    for (const InT& Value : Input)
    {
        Output.Emplace(OutT{Value});
    }
    return Output;
}
}    // namespace Util
