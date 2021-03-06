// Copyright 2022 Stream.IO, Inc. All Rights Reserved.

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
    for (const auto& Header : Headers)
    {
        Request->SetHeader(Header.Key, Header.Value.ToString());
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

FRequestBuilder& FRequestBuilder::EmptyJson()
{
    const FString EmptyObject = TEXT("{}");
    return Json(EmptyObject);
}

void FRequestBuilder::Send(const TFunction<void(const FHttpResponse&)> Callback)
{
    RetainedCallback = Callback;
    Client->OnRequestDelegate.Broadcast(*this);
    SendInternal();
    UE_LOG(LogHttpClient, Verbose, TEXT("Sent HTTP request [Verb=%s, Url=%s]"), *Request->GetVerb(), *Request->GetURL());
    for (const FString& Header : Request->GetAllHeaders())
    {
        UE_LOG(LogHttpClient, Verbose, TEXT("[Header=%s]"), *Header);
    }
}

void FRequestBuilder::Resend()
{
    SendInternal();
    UE_LOG(LogHttpClient, Log, TEXT("Resent HTTP request [Verb=%s, Url=%s]"), *Request->GetVerb(), *Request->GetURL());
}

FRequestBuilder& FRequestBuilder::Json(const FString& Json)
{
    Request->SetContentAsString(Json);
    Request->SetHeader(TEXT("Content-Type"), TEXT("application/json"));

    UE_LOG(LogHttpClient, Verbose, TEXT("Set request body [Json=%s]"), *Json);
    return *this;
}

void FRequestBuilder::SendInternal()
{
    Request->OnProcessRequestComplete().BindLambda(
        [RequestBuilder = *this](const FHttpRequestPtr OriginalRequest, const FHttpResponsePtr Response, bool bConnectedSuccessfully) mutable
        {
            const FHttpResponse HttpResponse{Response};
            if (HttpResponse.StatusCode >= 200 && HttpResponse.StatusCode < 300)
            {
                UE_LOG(
                    LogHttpClient,
                    Log,
                    TEXT("HTTP request succeeded [StatusCode=%d, Verb=%s, Url=%s]"),
                    HttpResponse.StatusCode,
                    *OriginalRequest->GetVerb(),
                    *OriginalRequest->GetURL());
            }
            else
            {
                UE_LOG(
                    LogHttpClient,
                    Error,
                    TEXT("HTTP request returned an error [StatusCode=%d, Verb=%s, Url=%s]"),
                    HttpResponse.StatusCode,
                    *OriginalRequest->GetVerb(),
                    *OriginalRequest->GetURL());
                RequestBuilder.Client->OnErrorDelegate.Broadcast(HttpResponse, RequestBuilder);
            }

            UE_LOG(LogHttpClient, Verbose, TEXT("HTTP response [Body=%s]"), *HttpResponse.Text);
            RequestBuilder.Client->OnResponseDelegate.Broadcast(HttpResponse);
            if (RequestBuilder.RetainedCallback)
            {
                RequestBuilder.RetainedCallback(HttpResponse);
            }
        });

    const FString Url = QueryUtils::AddQueryToUrl(BaseUrl, QueryParameters);
    Request->SetURL(Url);
    Request->ProcessRequest();
}
