#pragma once

#include "CoreMinimal.h"
#include "RequestBuilder.h"

namespace HttpRequests
{
FRequestBuilder HTTPREQUESTS_API Get(const FString& Url);
FRequestBuilder HTTPREQUESTS_API Post(const FString& Url);
FRequestBuilder HTTPREQUESTS_API Put(const FString& Url);
FRequestBuilder HTTPREQUESTS_API Patch(const FString& Url);
FRequestBuilder HTTPREQUESTS_API Delete(const FString& Url);
}	 // namespace HttpRequests
