#pragma once
#include "Dto/Response/ChannelState.h"
#include "HttpClient.h"
#include "RequestBuilder.h"

class FChatApi
{
public:
    explicit FChatApi(const FString& InApiKey);
    void GetOrCreateChannel(
        TFunction<void(const FChannelState&)> Callback,
        const FString& ChannelType,
        bool bState = true,
        bool bWatch = false,
        bool bPresence = false,
        const FString& ChannelId = {}) const;

private:
    FString BuildUrl(const FString& Path) const;

    void AddAuth(FRequestBuilder&) const;
    static void OnError(const FHttpResponse&);

    TSharedPtr<FHttpClient> Client;
    FString ApiKey;
    FString Scheme;
    FString Host;
};
