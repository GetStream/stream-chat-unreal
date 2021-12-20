// Copyright 2021 Stream.IO, Inc. All Rights Reserved.

#include "RequestBuilder.h"

#include "HttpClient.h"
#include "HttpModule.h"
#include "Interfaces/IPluginManager.h"
#include "QueryParameters.h"
#include "QueryUtils.h"
#include "String/ParseTokens.h"

DEFINE_LOG_CATEGORY(LogHttpClient);

FRequestBuilder::FRequestBuilder(const TSharedRef<const FHttpClient>& InClient, const FString& Verb, const FString& Url) : Client(InClient), BaseUrl(Url)
{
    Request = FHttpModule::Get().CreateRequest();
    Request->SetVerb(Verb);

    // Set some reasonable defaults
    Request->SetHeader(TEXT("User-Agent"), TEXT("X-UnrealEngine-Agent"));
    static FString StreamClient;
    if (StreamClient.IsEmpty())
    {
        const TSharedPtr<IPlugin> Plugin = IPluginManager::Get().FindPlugin(TEXT("StreamChat"));
        const FString& Version = Plugin->GetDescriptor().VersionName;
        StreamClient = FString::Printf(TEXT("stream-chat-unreal-%s"), *Version);
    }
    Request->SetHeader(TEXT("X-Stream-Client"), StreamClient);
}

FRequestBuilder& FRequestBuilder::Header(const FQueryParameters& Headers)
{
    for (auto [Key, Value] : Headers)
    {
        Request->SetHeader(Key, Value.ToString());
    }
    return *this;
}

FRequestBuilder& FRequestBuilder::Body(const FString& Text)
{
    Request->SetContentAsString(Text);
    return *this;
}

FRequestBuilder& FRequestBuilder::Query(const FQueryParameters& Query)
{
    QueryParameters.Append(Query);

    return *this;
}

void FRequestBuilder::Send(const TFunction<void(const FHttpResponse&)> Callback)
{
    RetainedCallback = Callback;
    Client->OnRequestDelegate.Broadcast(*this);
    SendInternal();
    UE_LOG(LogHttpClient, Log, TEXT("Sent HTTP request [Url=%s]"), *Request->GetURL());
    for (const FString& Header : Request->GetAllHeaders())
    {
        UE_LOG(LogHttpClient, Log, TEXT("[Header=%s]"), *Header);
    }
}

void FRequestBuilder::Resend()
{
    SendInternal();
    UE_LOG(LogHttpClient, Log, TEXT("Resent HTTP request [Url=%s]"), *Request->GetURL());
}

FRequestBuilder& FRequestBuilder::Json(const FString& Json)
{
    Request->SetContentAsString(Json);
    Request->SetHeader(TEXT("Content-Type"), TEXT("application/json"));

    UE_LOG(LogHttpClient, Log, TEXT("Set request body [Json=%s]"), *Json);
    return *this;
}

void FRequestBuilder::SendInternal()
{
    Request->OnProcessRequestComplete().BindLambda(
        [RequestBuilder = *this](const FHttpRequestPtr OriginalRequest, const FHttpResponsePtr Response, bool bConnectedSuccessfully) mutable
        {
            if (const FHttpResponse HttpResponse{Response}; HttpResponse.StatusCode >= 200 && HttpResponse.StatusCode < 300)
            {
                UE_LOG(LogHttpClient, Log, TEXT("HTTP request succeeded [StatusCode=%d, Url=%s]"), HttpResponse.StatusCode, *OriginalRequest->GetURL());
                UE_LOG(LogHttpClient, VeryVerbose, TEXT("HTTP response [Body=%s]"), *HttpResponse.Text);
                RequestBuilder.Client->OnResponseDelegate.Broadcast(HttpResponse);
                if (RequestBuilder.RetainedCallback)
                {
                    RequestBuilder.RetainedCallback(HttpResponse);
                }
            }
            else
            {
                UE_LOG(
                    LogHttpClient, Error, TEXT("HTTP request returned an error [StatusCode=%d, Url=%s]"), HttpResponse.StatusCode, *OriginalRequest->GetURL());
                RequestBuilder.Client->OnErrorDelegate.Broadcast(HttpResponse, RequestBuilder);
            }
        });

    const FString Url = QueryUtils::AddQueryToUrl(BaseUrl, QueryParameters);
    Request->SetURL(Url);
    Request->ProcessRequest();
}
