// Copyright Stream.IO, Inc. All Rights Reserved.

#include "HttpClient.h"

FRequestBuilder FHttpClient::Get(const FString& Url) const
{
    return FRequestBuilder{AsShared(), TEXT("GET"), Url};
}

FRequestBuilder FHttpClient::Post(const FString& Url) const
{
    return FRequestBuilder(AsShared(), TEXT("POST"), Url);
}

FRequestBuilder FHttpClient::Put(const FString& Url) const
{
    return FRequestBuilder(AsShared(), TEXT("PUT"), Url);
}

FRequestBuilder FHttpClient::Patch(const FString& Url) const
{
    return FRequestBuilder(AsShared(), TEXT("PATCH"), Url);
}

FRequestBuilder FHttpClient::Delete(const FString& Url) const
{
    return FRequestBuilder(AsShared(), TEXT("DELETE"), Url);
}
