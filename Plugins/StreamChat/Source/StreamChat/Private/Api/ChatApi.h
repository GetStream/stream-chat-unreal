#pragma once
#include "HttpClient.h"
#include "RequestBuilder.h"

struct FChannelStateResponseDto;
struct FMessageResponseDto;
struct FMessageRequestDto;
class FTokenManager;

enum class EChannelCreationFlags : uint8
{
    None = 0,

    // Refresh channel state
    State = 1 << 0,

    // Start watching the channel
    Watch = 1 << 1,

    // Fetch user presence info
    Presence = 1 << 2,
};
ENUM_CLASS_FLAGS(EChannelCreationFlags);

class FChatApi
{
public:
    explicit FChatApi(const FString& InApiKey, const TSharedRef<FTokenManager>&);

    void GetOrCreateChannel(
        const FString& ChannelType,
        const FString& ConnectionId,
        const FString& ChannelId = {},
        EChannelCreationFlags Flags = EChannelCreationFlags::Watch,
        TFunction<void(const FChannelStateResponseDto&)> Callback = {}) const;

    void SendNewMessage(
        const FString& ChannelType,
        const FString& ChannelId,
        const FMessageRequestDto& MessageRequest,
        bool bSkipPush = false,
        TFunction<void(const FMessageResponseDto&)> Callback = {}) const;

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
