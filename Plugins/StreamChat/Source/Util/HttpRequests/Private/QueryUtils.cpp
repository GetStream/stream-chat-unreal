// Copyright 2021 Stream.IO, Inc. All Rights Reserved.

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
        Path = FStringView(Url);
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

FString QueryUtils::AddQueryToUrl(const FString& Url, const FQueryParameters& Parameters)
{
    if (Parameters.Num() == 0)
    {
        return Url;
    }

    FStringView Path, Query, Fragment;
    SplitUrl(Url, Path, Query, Fragment);

    TMap<FString, FString> ExistingParams = ParseQueryString(Query);

    // Merge parameters
    ExistingParams.Reserve(ExistingParams.Num() + Parameters.Num());
    for (auto& Pair : Parameters)
    {
        ExistingParams.Emplace(Pair.Key, Pair.Value.ToString(true));
    }

    FString Result;
    Result.Reserve(Url.Len());
    Result.Append(Path);

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
