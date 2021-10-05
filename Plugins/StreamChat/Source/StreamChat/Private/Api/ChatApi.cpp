#include "ChatApi.h"

#include "Dto/Request/ChannelGetOrCreateRequest.h"
#include "Dto/Response/ChannelState.h"
#include "HttpRequests/Public/HttpRequests.h"
#include "StreamChatSettings.h"

namespace
{
template <class T>
void SendWithAuth(const FRequestBuilder& Request, TFunction<void(T)> Callback)
{
	Request.Send(
		[Callback](const FHttpResponse& Response)
		{
			if (Response.StatusCode > 200 && Response.StatusCode < 300)
			{
				Callback(Response.Json<T>());
			}
			else
			{
				// TODO: Deserialize error JSON
				UE_LOG(LogTemp, Error, TEXT("API returned error (%d): %s"), Response.StatusCode, *Response.Text);
			}
		});
}
}	 // namespace

FChatApi::FChatApi(const FString& InApiKey) : ApiKey(InApiKey), Scheme(TEXT("https")), Host(GetDefault<UStreamChatSettings>()->Host)
{
}

void FChatApi::GetOrCreateChannel(TFunction<void(FChannelState)> Callback,
	const FString& ChannelType,
	bool bState,
	bool bWatch,
	bool bPresence,
	const FString& ChannelId)
{
	const FString ChannelPath = ChannelId.IsEmpty() ? ChannelType : FString::Printf(TEXT("%s/%s"), *ChannelType, *ChannelId);
	const FString Path = FString::Printf(TEXT("/channels/%s/query"), *ChannelPath);
	const FString Url = BuildUrl(Path);
	const FChannelGetOrCreateRequest Body{TEXT(""), bWatch, bState, bPresence};
	SendWithAuth(HttpRequests::Post(Url).Query({{TEXT("api_key"), ApiKey}}).Json(Body), Callback);
}

FString FChatApi::BuildUrl(const FString& Path) const
{
	return FString::Printf(TEXT("%s://%s/%s"), *Scheme, *Host, *Path);
}
