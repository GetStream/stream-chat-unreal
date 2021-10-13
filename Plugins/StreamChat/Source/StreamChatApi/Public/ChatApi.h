#pragma once

#include "CoreMinimal.h"
#include "Dto/Request/FilterDto.h"
#include "Dto/Request/SortOptionDto.h"
#include "PaginationOptions.h"

struct FHttpResponse;
class FRequestBuilder;
struct FChannelsResponseDto;
struct FChannelStateResponseDto;
struct FChannelResponseDto;
struct FMessageResponseDto;
struct FMessageRequestDto;
struct FSortOption;
class FTokenManager;
class FHttpClient;

template <class T>
using TCallback = TFunction<void(const T&)>;

enum class EChannelFlags : uint8
{
    None = 0,

    // Refresh channel state
    State = 1 << 0,

    // Start watching the channel
    Watch = 1 << 1,

    // Fetch user presence info
    Presence = 1 << 2,
};
ENUM_CLASS_FLAGS(EChannelFlags);

class STREAMCHATAPI_API FChatApi
{
public:
    explicit FChatApi(const FString& InApiKey, const FString& InHost, const TSharedRef<FTokenManager>&);

    void GetOrCreateChannel(
        const FString& ChannelType,
        const FString& ConnectionId,
        const FString& ChannelId = {},
        EChannelFlags Flags = EChannelFlags::Watch,
        TCallback<FChannelStateResponseDto> Callback = {}) const;

    void SendNewMessage(
        const FString& ChannelType,
        const FString& ChannelId,
        const FMessageRequestDto& MessageRequest,
        bool bSkipPush = false,
        TCallback<FMessageResponseDto> Callback = {}) const;

    /**
     * Query channels with filter query
     * @param ConnectionId
     * @param Filter
     * @param SortOptions
     * @param MemberLimit
     * @param MessageLimit
     * @param Flags
     * @param PaginationOptions
     * @param Callback
     */
    void QueryChannels(
        const FString& ConnectionId,
        const TOptional<FFilterDto>& Filter = {},
        const TOptional<TArray<FSortOptionDto>>& SortOptions = {},
        TOptional<uint32> MemberLimit = {},
        TOptional<uint32> MessageLimit = {},
        EChannelFlags Flags = EChannelFlags::State | EChannelFlags::Watch,
        FPaginationOptions PaginationOptions = {},
        TCallback<FChannelsResponseDto> Callback = {});

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
