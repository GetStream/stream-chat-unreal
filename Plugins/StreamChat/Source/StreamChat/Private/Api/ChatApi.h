#pragma once
#include "Dto/Response/ChannelStateDto.h"
#include "HttpClient.h"
#include "RequestBuilder.h"

class FTokenManager;

enum class EChannelCreationFlags : uint8
{
    // Refresh channel state
    State = 0x00,

    // Start watching the channel
    Watch = 1 << 0,

    // Fetch user presence info
    Presence = 1 << 1,
};
ENUM_CLASS_FLAGS(EChannelCreationFlags);

class FChatApi
{
public:
    explicit FChatApi(const FString& InApiKey, const TSharedRef<FTokenManager>&);

    void GetOrCreateChannel(
        TFunction<void(const FChannelStateDto&)> Callback,
        const FString& ChannelType,
        const FString& ConnectionId,
        const FString& ChannelId = {},
        EChannelCreationFlags Flags = EChannelCreationFlags::Watch) const;

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
