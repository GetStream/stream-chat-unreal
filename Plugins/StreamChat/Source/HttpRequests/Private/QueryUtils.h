#pragma once

#include "CoreMinimal.h"

namespace QueryUtils
{
FString ToString(const FStringFormatArg& Arg);

TMap<FString, FString> ParseQueryFromUrl(const FString& Url);

/**
 * Modify the query string of a URL.
 *
 * Modifies the input URL, adding the parameters provided. Existing parameters in the URL with same keys will be
 * overwritten.
 * @param Url Starting URL. May contain a query string already.
 * @param Parameters Parameters to add to the URL
 * @return A new URL with the query parameters added
 */
FString AddQueryToUrl(const FString& Url, const FStringFormatNamedArguments& Parameters);
}    // namespace QueryUtils
