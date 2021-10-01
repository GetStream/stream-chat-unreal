#pragma once

#include "CoreMinimal.h"
#include "RequestBuilder.h"

namespace HttpRequests
{
FRequestBuilder Get(const FString& Url);
FRequestBuilder Post(const FString& Url);
FRequestBuilder Put(const FString& Url);
FRequestBuilder Patch(const FString& Url);
FRequestBuilder Delete(const FString& Url);
}	 // namespace HttpRequests
