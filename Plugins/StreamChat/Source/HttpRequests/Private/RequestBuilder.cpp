#include "RequestBuilder.h"

#include "GenericPlatform/GenericPlatformHttp.h"
#include "HttpModule.h"

FRequestBuilder::FRequestBuilder(const FString& Verb, const FString& Url)
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

void FRequestBuilder::Send(const TFunction<void(FHttpResponse)> Callback)
{
	Request->OnProcessRequestComplete().BindLambda(
		[Callback](FHttpRequestPtr Request, const FHttpResponsePtr Response, bool bConnectedSuccessfully)
		{
			const FHttpResponse HttpResponse{Response};
			Callback(HttpResponse);
		});
	Request->ProcessRequest();
}
