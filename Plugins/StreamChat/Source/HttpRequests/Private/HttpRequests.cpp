#include "HttpRequests.h"

FRequestBuilder HttpRequests::Get(const FString& Url)
{
	return FRequestBuilder(TEXT("GET"), Url);
}

FRequestBuilder HttpRequests::Post(const FString& Url)
{
	return FRequestBuilder(TEXT("POST"), Url);
}

FRequestBuilder HttpRequests::Put(const FString& Url)
{
	return FRequestBuilder(TEXT("PUT"), Url);
}

FRequestBuilder HttpRequests::Patch(const FString& Url)
{
	return FRequestBuilder(TEXT("PATCH"), Url);
}

FRequestBuilder HttpRequests::Delete(const FString& Url)
{
	return FRequestBuilder(TEXT("DELETE"), Url);
}
