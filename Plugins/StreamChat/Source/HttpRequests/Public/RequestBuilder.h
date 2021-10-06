#pragma once
#include "CoreMinimal.h"
#include "HttpResponse.h"
#include "Interfaces/IHttpRequest.h"
#include "StreamJson.h"

class FHttpClient;

class HTTPREQUESTS_API FRequestBuilder
{
public:
    FRequestBuilder() = delete;
    FRequestBuilder(const FRequestBuilder&) = delete;
    void operator=(const FRequestBuilder&) = delete;

    explicit FRequestBuilder(const TSharedRef<const FHttpClient>&, const FString& Verb, const FString& Url);

    FRequestBuilder& Body(const FString& Text);
    FRequestBuilder& Query(const FStringFormatNamedArguments& Query);

    /**
     * Add a body to the request formatted as JSON
     *
     * @tparam T Input struct type
     * @param Struct Will be serialized as JSON in the body of the request
     * @param NamingConvention JSON keys and values will be formatted according to this naming convention (snake_case by
     * default)
     * @return Builder to continue creating a request
     */
    template <class T>
    FRequestBuilder& Json(const T& Struct, ENamingConvention NamingConvention = ENamingConvention::SnakeCase);

    /**
     * Send a HTTP request to the target URL, calling the callback when a response is received.
     *
     * @param Callback A function to be called when the response is received
     */
    void Send(TFunction<void(const FHttpResponse&)> Callback);

    /**
     * Send a HTTP request to the target URL, calling the callback when a response is received, deserializing the body
     * as JSON on success.
     *
     * @param Callback A function to be called when the response is received
     */
    template <class T>
    void Send(TFunction<void(const T&)> Callback);

private:
    TSharedPtr<const FHttpClient> Client;
    FHttpRequestPtr Request;
};

template <class T>
FRequestBuilder& FRequestBuilder::Json(const T& Struct, ENamingConvention NamingConvention)
{
    const FString JsonBody = Json::Serialize(Struct, NamingConvention);
    Request->SetContentAsString(JsonBody);
    Request->SetHeader(TEXT("Content-Type"), TEXT("application/json"));
    return *this;
}

template <class T>
void FRequestBuilder::Send(TFunction<void(const T&)> Callback)
{
    Send(
        [Callback](const FHttpResponse& Response)
        {
            if (Callback)
            {
                Callback(Response.Json<T>());
            }
        });
}
