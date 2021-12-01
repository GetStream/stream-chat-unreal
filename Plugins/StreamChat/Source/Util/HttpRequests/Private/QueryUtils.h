// Copyright 2021 Stream.IO, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "QueryParameters.h"

namespace QueryUtils
{
TMap<FString, FString> ParseQueryFromUrl(const FString& Url);

/**
 * @brief Modify the query string of a URL.
 *
 * Modifies the input URL, adding the parameters provided. Existing parameters in the URL with same keys will be
 * overwritten.
 * @param Url Starting URL. May contain a query string already.
 * @param Parameters Parameters to add to the URL
 * @return A new URL with the query parameters added
 */
FString AddQueryToUrl(const FString& Url, const FQueryParameters& Parameters);
}    // namespace QueryUtils
