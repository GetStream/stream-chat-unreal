#pragma once

#include "CoreMinimal.h"
#include "RequestBuilder.h"

DECLARE_EVENT_OneParam(FHttpClient, FOnRequestEvent, FRequestBuilder&);
DECLARE_EVENT_OneParam(FHttpClient, FOnResponseEvent, const FHttpResponse&);

class HTTPREQUESTS_API FHttpClient : public TSharedFromThis<FHttpClient>
{
public:
    FRequestBuilder Get(const FString& Url) const;
    FRequestBuilder Post(const FString& Url) const;
    FRequestBuilder Put(const FString& Url) const;
    FRequestBuilder Patch(const FString& Url) const;
    FRequestBuilder Delete(const FString& Url) const;

    /**
     * Event which will be executed before each request is sent.
     */
    FOnRequestEvent OnRequestEvent;
    /**
     * Event which will be executed on each request success.
     */
    FOnResponseEvent OnResponseEvent;
    /**
     * Event which will be executed on each request failure.
     */
    FOnResponseEvent OnErrorEvent;
};
