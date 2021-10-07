#include "QueryUtils.h"

#include "GenericPlatform/GenericPlatformHttp.h"
#include "String/ParseTokens.h"

namespace
{
void SplitUrl(const FString& Url, FStringView& Path, FStringView& Query, FStringView& Fragment)
{
    int32 QuestionIndex, HashIndex;
    if (!Url.FindChar(TEXT('?'), QuestionIndex))
    {
        return;
    }

    if (!Url.FindChar(TEXT('#'), HashIndex))
    {
        HashIndex = Url.Len();
    }

    Path = FStringView(Url).Left(QuestionIndex);
    Query = FStringView(Url).Mid(QuestionIndex + 1, HashIndex - QuestionIndex - 1);
    Fragment = FStringView(Url).RightChop(HashIndex + 1);
}

TMap<FString, FString> ParseQueryString(const FStringView& QueryString)
{
    TMap<FString, FString> Result;
    UE::String::ParseTokens(
        QueryString,
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
}    // namespace

FString QueryUtils::ToString(const FStringFormatArg& Arg)
{
    switch (Arg.Type)
    {
        case FStringFormatArg::Int:
            return LexToString(Arg.IntValue);
        case FStringFormatArg::UInt:
            return LexToString(Arg.UIntValue);
        case FStringFormatArg::Double:
            // Trim trailing zeroes. e.g. 0.5f => "0.5" instead of "0.500000"
            return FString::Printf(TEXT("%g"), Arg.DoubleValue);
        case FStringFormatArg::String:
            return FGenericPlatformHttp::UrlEncode(Arg.StringValue);
        case FStringFormatArg::StringLiteral:
            return FGenericPlatformHttp::UrlEncode(Arg.StringLiteralValue);
        default:
            return {};
    }
}

TMap<FString, FString> QueryUtils::ParseQueryFromUrl(const FString& Url)
{
    FStringView Path, Query, Fragment;
    SplitUrl(Url, Path, Query, Fragment);
    if (Query.IsEmpty())
    {
        return {};
    }

    return ParseQueryString(Query);
}

FString QueryUtils::AddQueryToUrl(const FString& Url, const FStringFormatNamedArguments& Parameters)
{
    FStringView Path, Query, Fragment;
    SplitUrl(Url, Path, Query, Fragment);
    if (Query.IsEmpty())
    {
        return Url;
    }

    TMap<FString, FString> ExistingParams = ParseQueryString(Query);

    // Merge parameters
    ExistingParams.Reserve(ExistingParams.Num() + Parameters.Num());
    for (auto& Pair : Parameters)
    {
        ExistingParams.Emplace(Pair.Key, ToString(Pair.Value));
    }

    FString Result(Path);
    Result.Reserve(Url.Len());

    Result.AppendChar(TEXT('?'));
    for (auto It = ExistingParams.CreateConstIterator(); It;)
    {
        Result += FString::Printf(TEXT("%s=%s"), *It->Key, *It->Value);
        if (++It)
        {
            Result.AppendChar(TEXT('&'));
        }
    }

    if (!Fragment.IsEmpty())
    {
        Result.AppendChar(TEXT('#'));
        Result += Fragment;
    }

    return Result;
}
