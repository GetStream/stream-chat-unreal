#pragma once
#include "Dto/Response/ChannelState.h"
#include "HttpClient.h"
#include "RequestBuilder.h"

class FTokenManager;

class FChatApi
{
public:
    explicit FChatApi(const FString& InApiKey, const TSharedRef<FTokenManager>&);
    void GetOrCreateChannel(
        TFunction<void(const FChannelState&)> Callback,
        const FString& ChannelType,
        const FString& ConnectionId,
        const FString& ChannelId = {},
        bool bState = true,
        bool bWatch = false,
        bool bPresence = false) const;

private:
    FString BuildUrl(const FString& Path) const;

    void AddAuth(FRequestBuilder&) const;
    static void OnError(const FHttpResponse&);

    TSharedPtr<FTokenManager> TokenManager;
    FString ApiKey;

    TSharedPtr<FHttpClient> Client;
    FString Scheme;
    FString Host;
};
