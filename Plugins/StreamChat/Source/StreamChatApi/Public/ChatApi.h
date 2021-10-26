#pragma once

#include "CoreMinimal.h"
#include "JsonObjectWrapper.h"
#include "PaginationOptions.h"
#include "Request/Channel/MessagePaginationParamsRequestDto.h"
#include "Request/Channel/PaginationParamsRequestDto.h"
#include "Request/Channel/SortOptionDto.h"
#include "Response/Event/EventResponseDto.h"
#include "Response/Reaction/ReactionResponseDto.h"

class FHttpClient;
class FRequestBuilder;
class FTokenManager;
struct FChannelEvent;
struct FChannelResponseDto;
struct FChannelStateResponseDto;
struct FChannelsResponseDto;
struct FHttpResponse;
struct FMessageRequestDto;
struct FMessageResponseDto;
struct FReactionRequestDto;
struct FSortOption;

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

    /**
     * Get messages, members or other channel fields.
     * Creates the channel if not yet created.
     * @param Callback Called when response is received.
     * @param ChannelType Name of built-in or custom channel type (e.g. messaging, team, livestream)
     * @param ConnectionId Websocket connection ID to interact with.
     * @param ChannelId A unique identifier for the channel.
     * @param Flags Additional actions to perform, like watch, or fetch presence. @see EChannelFlags
     */
    void QueryChannel(
        TCallback<FChannelStateResponseDto> Callback,
        const FString& ChannelType,
        const FString& ConnectionId,
        const FString& ChannelId = {},
        EChannelFlags Flags = EChannelFlags::State,
        const TOptional<FMessagePaginationParamsRequestDto> MessagePagination = {},
        const TOptional<FPaginationParamsRequestDto> MemberPagination = {},
        const TOptional<FPaginationParamsRequestDto> WatcherPagination = {}) const;

    void SendNewMessage(
        const FString& ChannelType,
        const FString& ChannelId,
        const FMessageRequestDto& MessageRequest,
        bool bSkipPush = false,
        TCallback<FMessageResponseDto> Callback = {}) const;

    void UpdateMessage(const FMessageRequestDto& MessageRequest, TCallback<FMessageResponseDto> Callback = {}) const;
    void DeleteMessage(const FString& Id, bool bHard = false, TCallback<FMessageResponseDto> Callback = {}) const;

    void SendReaction(
        const FString& MessageId,
        const FReactionRequestDto& ReactionRequest,
        bool bEnforceUnique = true,
        bool bSkipPush = false,
        TCallback<FReactionResponseDto> Callback = {}) const;

    void DeleteReaction(const FString& MessageId, const FName& Type, TCallback<FReactionResponseDto> Callback = {})
        const;

    template <class TEvent>
    void SendChannelEvent(
        const FString& ChannelType,
        const FString& ChannelId,
        const TEvent&,
        TCallback<FEventResponseDto> Callback = {});

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
        TCallback<FChannelsResponseDto> Callback,
        const FString& ConnectionId,
        EChannelFlags Flags = EChannelFlags::State | EChannelFlags::Watch,
        const TOptional<FJsonObjectWrapper>& Filter = {},
        const TArray<FSortOptionDto>& SortOptions = {},
        TOptional<uint32> MemberLimit = {},
        TOptional<uint32> MessageLimit = {},
        FPaginationOptions PaginationOptions = {}) const;

private:
    void SendChannelEvent(
        const FString& ChannelType,
        const FString& ChannelId,
        const UStruct* EventStructDefinition,
        const void* EventStruct,
        TCallback<FEventResponseDto> Callback = {}) const;

    FString BuildUrl(const FString& Path) const;

    void AddAuth(FRequestBuilder&) const;
    static void OnError(const FHttpResponse&);

    TSharedPtr<FTokenManager> TokenManager;
    FString ApiKey;

    TSharedPtr<FHttpClient> Client;
    FString Scheme;
    FString Host;
};

template <class TEvent>
void FChatApi::SendChannelEvent(
    const FString& ChannelType,
    const FString& ChannelId,
    const TEvent& Event,
    TCallback<FEventResponseDto> Callback)
{
    SendChannelEvent(ChannelType, ChannelId, TEvent::StaticStruct(), &Event, Callback);
}
