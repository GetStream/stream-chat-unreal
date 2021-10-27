#include "QueryParameters.h"

#include "GenericPlatform/GenericPlatformHttp.h"

FQueryParameter::FQueryParameter(bool bValue) : Data{TInPlaceType<bool>(), bValue}
{
}

FQueryParameter::FQueryParameter(float Value) : Data{TInPlaceType<float>(), Value}
{
}

FQueryParameter::FQueryParameter(int32 Value) : Data{TInPlaceType<int32>(), Value}
{
}

FQueryParameter::FQueryParameter(const FString& Value) : Data{TInPlaceType<FString>(), Value}
{
}

FQueryParameter::FQueryParameter(const TCHAR* Value) : Data{TInPlaceType<FString>(), Value}
{
}

namespace
{
struct FQueryParameterToString
{
    FString operator()(const bool bValue) const
    {
        return LexToString(bValue);
    }

    FString operator()(const float Value) const
    {
        // Trim trailing zeroes. e.g. 0.5f => "0.5" instead of "0.500000"
        return FString::Printf(TEXT("%g"), Value);
    }

    FString operator()(const int32 Value) const
    {
        return LexToString(Value);
    }

    FString operator()(const FString& Value) const
    {
        return FGenericPlatformHttp::UrlEncode(Value);
    }
};
}    // namespace

FString FQueryParameter::ToString() const
{
    return Visit(FQueryParameterToString{}, Data);
}
