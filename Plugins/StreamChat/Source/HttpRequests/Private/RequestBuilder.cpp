#include "RequestBuilder.h"

#include "HttpClient.h"
#include "HttpModule.h"
#include "QueryUtils.h"
#include "String/ParseTokens.h"

DEFINE_LOG_CATEGORY(LogHttpClient);

FRequestBuilder::FRequestBuilder(const TSharedRef<const FHttpClient>& InClient, const FString& Verb, const FString& Url)
    : Client(InClient)
{
    Request = FHttpModule::Get().CreateRequest();
    Request->SetURL(Url);
    Request->SetVerb(Verb);

    // Set some reasonable defaults
    Request->SetHeader(TEXT("User-Agent"), TEXT("X-UnrealEngine-Agent"));
}

FRequestBuilder& FRequestBuilder::Header(const FStringFormatNamedArguments& Headers)
{
    for (auto [Key, Value] : Headers)
    {
        Request->SetHeader(Key, QueryUtils::ToString(Value));
    }
    return *this;
}

FRequestBuilder& FRequestBuilder::Body(const FString& Text)
{
    Request->SetContentAsString(Text);
    return *this;
}

FRequestBuilder& FRequestBuilder::Query(const FStringFormatNamedArguments& Query)
{
    FString Result = Request->GetURL();
    if (Query.Num() > 0)
    {
        Result += TEXT("?");
        for (auto It = Query.CreateConstIterator(); It;)
        {
            Result += FString::Printf(TEXT("%s="), *It->Key);
            Result.Append(QueryUtils::ToString(It->Value));
            if (++It)
            {
                Result += TEXT("&");
            }
        }
    }

    Request->SetURL(Result);

    return *this;
}

void FRequestBuilder::Send(TFunction<void(const FHttpResponse&)> Callback)
{
    Client->OnRequestDelegate.Broadcast(*this);
    Request->OnProcessRequestComplete().BindLambda(
        [Client = Client, Callback](
            const FHttpRequestPtr Request, const FHttpResponsePtr Response, bool bConnectedSuccessfully)
        {
            const FHttpResponse HttpResponse{Response};

            if (HttpResponse.StatusCode >= 200 && HttpResponse.StatusCode < 300)
            {
                UE_LOG(
                    LogHttpClient,
                    Log,
                    TEXT("HTTP request succeeded [StatusCode=%d, Url=%s]"),
                    HttpResponse.StatusCode,
                    *Request->GetURL());
                Client->OnResponseDelegate.Broadcast(HttpResponse);
            }
            else
            {
                UE_LOG(
                    LogHttpClient,
                    Error,
                    TEXT("HTTP request returned an error [StatusCode=%d, Url=%s]"),
                    HttpResponse.StatusCode,
                    *Request->GetURL());
                Client->OnErrorDelegate.Broadcast(HttpResponse);
            }
            if (Callback)
            {
                Callback(HttpResponse);
            }
        });
    Request->ProcessRequest();
    UE_LOG(LogHttpClient, Log, TEXT("Sent HTTP request [Url=%s]"), *Request->GetURL());
}

FRequestBuilder& FRequestBuilder::Json(const FString& Json)
{
    Request->SetContentAsString(Json);
    Request->SetHeader(TEXT("Content-Type"), TEXT("application/json"));

    UE_LOG(LogHttpClient, Log, TEXT("Set request body [Json=%s]"), *Json);
    return *this;
}
