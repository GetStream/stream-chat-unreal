#include "RequestBuilder.h"

#include "GenericPlatform/GenericPlatformHttp.h"
#include "HttpClient.h"
#include "HttpModule.h"

FRequestBuilder::FRequestBuilder(const TSharedRef<const FHttpClient>& InClient, const FString& Verb, const FString& Url)
    : Client(InClient)
{
    Request = FHttpModule::Get().CreateRequest();
    Request->SetURL(Url);
    Request->SetVerb(Verb);

    // Set some reasonable defaults
    Request->SetHeader(TEXT("User-Agent"), TEXT("X-UnrealEngine-Agent"));
}

FRequestBuilder& FRequestBuilder::Body(const FString& Text)
{
    Request->SetContentAsString(Text);
    return *this;
}

// Stolen from Misc/StringFormatter.cpp
void AppendToString(const FStringFormatArg& Arg, FString& StringToAppendTo)
{
    switch (Arg.Type)
    {
        case FStringFormatArg::Int:
            StringToAppendTo.Append(LexToString(Arg.IntValue));
            break;
        case FStringFormatArg::UInt:
            StringToAppendTo.Append(LexToString(Arg.UIntValue));
            break;
        case FStringFormatArg::Double:
            StringToAppendTo.Append(LexToString(Arg.DoubleValue));
            break;
        case FStringFormatArg::String:
        {
            const FString EncodedString = FGenericPlatformHttp::UrlEncode(Arg.StringValue);
            StringToAppendTo.AppendChars(*EncodedString, EncodedString.Len());
            break;
        }
        case FStringFormatArg::StringLiteral:
        {
            const FString EncodedString = FGenericPlatformHttp::UrlEncode(Arg.StringLiteralValue);
            StringToAppendTo.AppendChars(*EncodedString, EncodedString.Len());
            break;
        }
    }
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
            AppendToString(It->Value, Result);
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
                Client->OnResponseDelegate.Broadcast(HttpResponse);
            }
            else
            {
                UE_LOG(
                    LogTemp,
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
}
