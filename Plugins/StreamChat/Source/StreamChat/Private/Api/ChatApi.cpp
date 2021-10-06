#include "ChatApi.h"

#include "Dto/Request/ChannelGetOrCreateRequest.h"
#include "Dto/Response/ChannelState.h"
#include "StreamChatSettings.h"

FChatApi::FChatApi(const FString& InApiKey)
    : Client(MakeShared<FHttpClient>())
    , ApiKey(InApiKey)
    , Scheme(TEXT("https"))
    , Host(GetDefault<UStreamChatSettings>()->Host)
{
    Client->OnRequestDelegate.AddRaw(this, &FChatApi::AddAuth);
    Client->OnErrorDelegate.AddStatic(&FChatApi::OnError);
}

void FChatApi::GetOrCreateChannel(
    const TFunction<void(const FChannelState&)> Callback,
    const FString& ChannelType,
    const bool bState,
    const bool bWatch,
    const bool bPresence,
    const FString& ChannelId) const
{
    const FString ChannelPath =
        ChannelId.IsEmpty() ? ChannelType : FString::Printf(TEXT("%s/%s"), *ChannelType, *ChannelId);
    const FString Path = FString::Printf(TEXT("/channels/%s/query"), *ChannelPath);
    const FString Url = BuildUrl(Path);
    const FChannelGetOrCreateRequest Body{TEXT(""), bWatch, bState, bPresence};
    Client->Post(Url).Json(Body).Send(Callback);
}

FString FChatApi::BuildUrl(const FString& Path) const
{
    return FString::Printf(TEXT("%s://%s/%s"), *Scheme, *Host, *Path);
}

void FChatApi::AddAuth(FRequestBuilder& Request) const
{
    Request.Query({{TEXT("api_key"), ApiKey}});
}

void FChatApi::OnError(const FHttpResponse& Response)
{
    // TODO: Deserialize error JSON
    UE_LOG(LogTemp, Error, TEXT("API returned error [Text=%s]"), Response.StatusCode, *Response.Text);
}
