// Copyright 2026 Stream.IO, Inc. All Rights Reserved.

#pragma once
#include "CoreMinimal.h"
#include "HttpResponse.h"
#include "Interfaces/IHttpRequest.h"
#include "QueryParameters.h"
#include "StreamJson.h"

class FHttpClient;

DECLARE_LOG_CATEGORY_EXTERN(LogHttpClient, Verbose, All);

template <class T>
struct TResponse
{
public:
    // Deserialize response if success
    explicit TResponse(const FHttpResponse& Response)
    {
        if (Response.IsSuccessful())
        {
            ResponseOrError.template Set<T>(Response.Json<T>());
        }
        else
        {
            ResponseOrError.template Set<FString>(TEXT("Error"));
        }
    }

    bool IsSuccessful() const
    {
        return ResponseOrError.template IsType<T>();
    }

    // Get a pointer to the held response, or nullptr if error
    const T* Get() const
    {
        return ResponseOrError.template TryGet<T>();
    }

    // Get a ref to the held response if you're SURE this isn't an error
    const T& GetRef() const
    {
        return ResponseOrError.template Get<T>();
    }

private:
    TVariant<T, FString> ResponseOrError;
};

class HTTPREQUESTS_API FRequestBuilder
{
public:
    FRequestBuilder() = delete;

    explicit FRequestBuilder(const TSharedRef<const FHttpClient>&, const FString& Verb, const FString& Url);

    /**
     * Sets optional header info on the request.
     * Calling with the same Key will overwrite any previous values
     * @param Headers Map of key-value pairs, e.g. `{{TEXT("X-Api-Key"), 1234}}`
     * @return Builder to continue creating a request
     */
    FRequestBuilder& Header(const FQueryParameters& Headers);

    FRequestBuilder& Body(const FString& Text);
    FRequestBuilder& Query(const FQueryParameters& Query);

    /**
     * Add a single file to the request as a multipart/form-data body.
     *
     * The body is assembled as bytes rather than a string, because file content is arbitrary binary
     * and would not survive a round trip through FString. Stream's upload endpoints expect exactly
     * one part, named `file`.
     *
     * @param FieldName Name of the form field, `file` for Stream's upload endpoints
     * @param FileName Reported to the server as the original filename, and used to guess the MIME type
     * @param Content Raw bytes of the file
     * @param ContentType MIME type. Guessed from the filename extension when left empty
     * @return Builder to continue creating a request
     */
    FRequestBuilder& Multipart(const FString& FieldName, const FString& FileName, const TArray<uint8>& Content, const FString& ContentType = {});

    /**
     * Assemble an RFC 7578 multipart body containing a single file part.
     *
     * Split out from Multipart() so it can be tested without a network round trip: the caller
     * supplies the boundary, which makes the output deterministic.
     */
    static TArray<uint8> BuildMultipartBody(
        const FString& Boundary,
        const FString& FieldName,
        const FString& FileName,
        const TArray<uint8>& Content,
        const FString& ContentType);

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
    FRequestBuilder& EmptyJson();

    /**
     * Send a HTTP request to the target URL, calling the callback when a response is received.
     *
     * @param Callback A function to be called when the response is received
     */
    void Send(TFunction<void(const FHttpResponse&)> Callback);

    /**
     * Send a HTTP request to the target URL, calling the callback when a response is received.
     * This convenience version will deserialize the body from JSON on success, returning it directly.
     *
     * @param Callback A function to be called when the response is received
     */
    template <class T>
    void Send(TFunction<void(const TResponse<T>&)> Callback);

    void Resend();

private:
    FRequestBuilder& Json(const FString& Json);
    void SendInternal();

    TSharedPtr<const FHttpClient> Client;
    FHttpRequestPtr Request;
    FString BaseUrl;
    FQueryParameters QueryParameters;
    TFunction<void(const FHttpResponse&)> RetainedCallback;
};

template <class T>
FRequestBuilder& FRequestBuilder::Json(const T& Struct, ENamingConvention NamingConvention)
{
    const FString JsonBody = Json::Serialize(Struct, NamingConvention);
    return Json(JsonBody);
}

template <class T>
void FRequestBuilder::Send(TFunction<void(const TResponse<T>&)> Callback)
{
    Send(
        [Callback](const FHttpResponse& Response)
        {
            if (Callback)
            {
                Callback(TResponse<T>(Response));
            }
        });
}
