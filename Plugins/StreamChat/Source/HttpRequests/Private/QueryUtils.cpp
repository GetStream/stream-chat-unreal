#include "QueryUtils.h"

#include "GenericPlatform/GenericPlatformHttp.h"
#include "String/ParseTokens.h"

FString QueryUtils::ToString(const FStringFormatArg& Arg)
{
    switch (Arg.Type)
    {
        case FStringFormatArg::Int:
            return LexToString(Arg.IntValue);
        case FStringFormatArg::UInt:
            return LexToString(Arg.UIntValue);
        case FStringFormatArg::Double:
            return LexToString(Arg.DoubleValue);
        case FStringFormatArg::String:
            return FGenericPlatformHttp::UrlEncode(Arg.StringValue);
        case FStringFormatArg::StringLiteral:
            return FGenericPlatformHttp::UrlEncode(Arg.StringLiteralValue);
        default:
            return {};
    }
}

TMap<FString, FString> QueryUtils::ParseQuery(const FString& Url)
{
    int32 QuestionIndex, HashIndex;
    if (!Url.FindChar(TEXT('?'), QuestionIndex))
    {
        return {};
    }

    if (!Url.FindChar(TEXT('#'), HashIndex))
    {
        HashIndex = Url.Len();
    }

    TMap<FString, FString> Result;
    const FStringView Query = FStringView(Url).Mid(QuestionIndex + 1, HashIndex - QuestionIndex - 1);
    UE::String::ParseTokens(
        Query,
        TEXT("&"),
        [&Result](const FStringView Token)
        {
            if (int32 EqualsIndex; Token.FindChar(TEXT('='), EqualsIndex))
            {
                const FStringView Key = Token.Left(EqualsIndex);
                const FStringView Value = Token.RightChop(EqualsIndex + 1);
                const FString KeyDecoded = FGenericPlatformHttp::UrlDecode(Key);
                const FString ValueDecoded = FGenericPlatformHttp::UrlDecode(Value);
                Result.Emplace(KeyDecoded, ValueDecoded);
            }
        });

    return Result;
}
