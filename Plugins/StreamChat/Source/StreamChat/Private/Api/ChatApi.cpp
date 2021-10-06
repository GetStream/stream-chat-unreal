#include "ChatApi.h"

#include "Dto/Request/ChannelGetOrCreateRequest.h"
#include "Dto/Response/ChannelState.h"
#include "StreamChatSettings.h"
#include "Token/TokenManager.h"

FChatApi::FChatApi(const FString& InApiKey, const TSharedRef<FTokenManager>& InTokenManager)
    : TokenManager(InTokenManager)
    , ApiKey(InApiKey)
    , Client(MakeShared<FHttpClient>())
    , Scheme(TEXT("https"))
    , Host(GetDefault<UStreamChatSettings>()->Host)
{
    Client->OnRequestDelegate.AddRaw(this, &FChatApi::AddAuth);
    Client->OnErrorDelegate.AddStatic(&FChatApi::OnError);
}

void FChatApi::GetOrCreateChannel(
    const TFunction<void(const FChannelState&)> Callback,
    const FString& ChannelType,
    const FString& ChannelId,
    const bool bState,
    const bool bWatch,
    const bool bPresence) const
{
    const FString ChannelPath =
        ChannelId.IsEmpty() ? ChannelType : FString::Printf(TEXT("%s/%s"), *ChannelType, *ChannelId);
    const FString Path = FString::Printf(TEXT("channels/%s/query"), *ChannelPath);
    const FString Url = BuildUrl(Path);
    const FChannelGetOrCreateRequest Body{bWatch, bState, bPresence};
    Client->Post(Url).Json(Body).Send(Callback);
}

FString FChatApi::BuildUrl(const FString& Path) const
{
    checkf(Path[0] != TEXT('/'), TEXT("Don't prefix the path with a slash [Path=%s]"), *Path);
    return FString::Printf(TEXT("%s://%s/%s"), *Scheme, *Host, *Path);
}

void FChatApi::AddAuth(FRequestBuilder& Request) const
{
    const FString Token = TokenManager->LoadToken();
    Request
        .Header({
            {TEXT("stream-auth-type"), TEXT("jwt")},
            {TEXT("Authorization"), Token},
        })
        .Query({{TEXT("api_key"), ApiKey}});
}

void FChatApi::AddConnectionId(FRequestBuilder& Request) const
{
    Request.Query({{TEXT("connection_id"), ApiKey}});
}

void FChatApi::OnError(const FHttpResponse& Response)
{
    // TODO: Deserialize error JSON
    UE_LOG(LogTemp, Error, TEXT("API error response [Text=%s]"), *Response.Text);
}
