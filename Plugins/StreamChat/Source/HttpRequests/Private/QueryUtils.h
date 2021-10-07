#pragma once

namespace QueryUtils
{
FString ToString(const FStringFormatArg& Arg);

TMap<FString, FString> ParseQuery(const FString& Url);
}    // namespace QueryUtils
