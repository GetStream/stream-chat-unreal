#include "RequestBuilder.h"

#include "GenericPlatform/GenericPlatformHttp.h"
#include "HttpClient.h"
#include "HttpModule.h"

namespace
{
FString ToString(const FStringFormatArg& Arg)
{
    switch (Arg.Type)
    {
        case FStringFormatArg::Int:
            return LexToString(Arg.IntValue);
        case FStringFormatArg::UInt:
            return LexToString(Arg.UIntValue);
        case FStringFormatArg::Double:
            return LexToString(Arg.DoubleValue);
        case FStringFormatArg::String:
            return FGenericPlatformHttp::UrlEncode(Arg.StringValue);
        case FStringFormatArg::StringLiteral:
            return FGenericPlatformHttp::UrlEncode(Arg.StringLiteralValue);
        default:
            return {};
    }
}
}    // namespace

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
        Request->SetHeader(Key, ToString(Value));
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
            Result.Append(ToString(It->Value));
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
    Client->OnRequestEvent.Broadcast(*this);
    UE_LOG(LogTemp, Log, TEXT("Sending HTTP request [Url=%s]"), *Request->GetURL());
    Request->OnProcessRequestComplete().BindLambda(
        [Client = Client, Callback](
            const FHttpRequestPtr Request, const FHttpResponsePtr Response, bool bConnectedSuccessfully)
        {
            const FHttpResponse HttpResponse{Response};

            if (HttpResponse.StatusCode >= 200 && HttpResponse.StatusCode < 300)
            {
                UE_LOG(
                    LogTemp,
                    Log,
                    TEXT("HTTP request succeeded [StatusCode=%d, Url=%s]"),
                    HttpResponse.StatusCode,
                    *Request->GetURL());
                Client->OnResponseEvent.Broadcast(HttpResponse);
            }
            else
            {
                UE_LOG(
                    LogTemp,
                    Error,
                    TEXT("HTTP request returned an error [StatusCode=%d, Url=%s]"),
                    HttpResponse.StatusCode,
                    *Request->GetURL());
                Client->OnErrorEvent.Broadcast(HttpResponse);
            }
            if (Callback)
            {
                Callback(HttpResponse);
            }
        });
    Request->ProcessRequest();
}
