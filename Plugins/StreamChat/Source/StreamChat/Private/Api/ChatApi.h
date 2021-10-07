#pragma once
#include "Dto/Response/ChannelStateDto.h"
#include "HttpClient.h"
#include "RequestBuilder.h"

class FTokenManager;

class FChatApi
{
public:
    explicit FChatApi(const FString& InApiKey, const TSharedRef<FTokenManager>&);

    // TODO don't use bool flags
    void GetOrCreateChannel(
        TFunction<void(const FChannelStateDto&)> Callback,
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
