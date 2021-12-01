// Copyright 2021 Stream.IO, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "RequestBuilder.h"

DECLARE_MULTICAST_DELEGATE_OneParam(FOnRequestDelegate, FRequestBuilder&);
DECLARE_MULTICAST_DELEGATE_OneParam(FOnResponseDelegate, const FHttpResponse&);
DECLARE_MULTICAST_DELEGATE_TwoParams(FOnErrorDelegate, const FHttpResponse&, FRequestBuilder&);

class HTTPREQUESTS_API FHttpClient : public TSharedFromThis<FHttpClient>
{
public:
    FRequestBuilder Get(const FString& Url) const;
    FRequestBuilder Post(const FString& Url) const;
    FRequestBuilder Put(const FString& Url) const;
    FRequestBuilder Patch(const FString& Url) const;
    FRequestBuilder Delete(const FString& Url) const;

    /**
     * Delegate which will be executed before each request is sent.
     */
    FOnRequestDelegate OnRequestDelegate;

    /**
     * Delegate which will be executed on each request success.
     */
    FOnResponseDelegate OnResponseDelegate;

    /**
     * Delegate which will be executed on each request failure.
     */
    FOnErrorDelegate OnErrorDelegate;
};
