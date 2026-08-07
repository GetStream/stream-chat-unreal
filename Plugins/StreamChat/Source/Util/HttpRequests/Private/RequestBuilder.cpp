// Copyright 2026 Stream.IO, Inc. All Rights Reserved.

#include "RequestBuilder.h"

#include "GenericPlatform/GenericPlatformHttp.h"
#include "HttpClient.h"
#include "HttpModule.h"
#include "Interfaces/IPluginManager.h"
#include "Misc/Guid.h"
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

namespace
{
/**
 * Make a string safe to embed in a Content-Disposition header parameter.
 *
 * A filename is attacker-controlled as far as this code is concerned: a quote would end the
 * parameter early and a CR or LF would let it inject further headers, so both are removed.
 */
FString SanitizeHeaderParameter(const FString& Value)
{
    FString Result = Value;
    Result.ReplaceInline(TEXT("\r"), TEXT(""), ESearchCase::CaseSensitive);
    Result.ReplaceInline(TEXT("\n"), TEXT(""), ESearchCase::CaseSensitive);
    Result.ReplaceInline(TEXT("\""), TEXT("'"), ESearchCase::CaseSensitive);
    return Result;
}

void AppendUtf8(TArray<uint8>& Out, const FString& Text)
{
    const FTCHARToUTF8 Converted{*Text};
    Out.Append(reinterpret_cast<const uint8*>(Converted.Get()), Converted.Length());
}
}    // namespace

TArray<uint8> FRequestBuilder::BuildMultipartBody(
    const FString& Boundary,
    const FString& FieldName,
    const FString& FileName,
    const TArray<uint8>& Content,
    const FString& ContentType)
{
    // CRLF is what RFC 7578 requires, on every platform. Building the body from bytes rather than
    // an FString keeps arbitrary binary content intact.
    const FString Preamble = FString::Printf(
        TEXT("--%s\r\nContent-Disposition: form-data; name=\"%s\"; filename=\"%s\"\r\nContent-Type: %s\r\n\r\n"),
        *Boundary,
        *SanitizeHeaderParameter(FieldName),
        *SanitizeHeaderParameter(FileName),
        *ContentType);
    const FString Epilogue = FString::Printf(TEXT("\r\n--%s--\r\n"), *Boundary);

    TArray<uint8> Body;
    Body.Reserve(Preamble.Len() + Content.Num() + Epilogue.Len());
    AppendUtf8(Body, Preamble);
    Body.Append(Content);
    AppendUtf8(Body, Epilogue);
    return Body;
}

FRequestBuilder& FRequestBuilder::Multipart(const FString& FieldName, const FString& FileName, const TArray<uint8>& Content, const FString& ContentType)
{
    // A GUID makes it implausible for the boundary to occur inside the file content, which would
    // otherwise truncate the upload.
    const FString Boundary = FString::Printf(TEXT("----StreamChatBoundary%s"), *FGuid::NewGuid().ToString(EGuidFormats::Digits));
    const FString MimeType = ContentType.IsEmpty() ? FGenericPlatformHttp::GetMimeType(FileName) : ContentType;

    Request->SetContent(BuildMultipartBody(Boundary, FieldName, FileName, Content, MimeType));
    Request->SetHeader(TEXT("Content-Type"), FString::Printf(TEXT("multipart/form-data; boundary=%s"), *Boundary));

    UE_LOG(LogHttpClient, Verbose, TEXT("Set multipart body [FileName=%s, ContentType=%s, Bytes=%d]"), *FileName, *MimeType, Content.Num());
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
