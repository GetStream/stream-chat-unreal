#pragma once

#include "ChannelFlags.h"
#include "CoreMinimal.h"
#include "JsonObjectWrapper.h"
#include "PaginationOptions.h"
#include "Request/Channel/ChannelRequestDto.h"
#include "Request/Channel/MessagePaginationParamsRequestDto.h"
#include "Request/Channel/PaginationParamsRequestDto.h"
#include "Request/Channel/SortParamRequestDto.h"
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

class STREAMCHATAPI_API FChatApi : public TSharedFromThis<FChatApi>
{
public:
    static TSharedRef<FChatApi> Create(
        const FString& InApiKey,
        const FString& InHost,
        const TSharedPtr<FTokenManager>&);

    /**
     * Get messages, members or other channel fields.
     * Creates the channel if not yet created.
     * @param Callback Called when response is received.
     * @param ChannelType Name of built-in or custom channel type (e.g. messaging, team, livestream)
     * @param ConnectionId Websocket connection ID to interact with.
     * @param ChannelId A unique identifier for the channel.
     * @param Flags Additional actions to perform, like watch, or fetch presence. @see EChannelFlags
     * @param MessagePagination Pagination details for returned messages.
     * @see https://getstream.io/chat/docs/other-rest/channel_pagination/
     * @param MemberPagination Pagination details for returned members.
     * @param WatcherPagination Pagination details for returned watchers/
     */
    void QueryChannel(
        TCallback<FChannelStateResponseDto> Callback,
        const FString& ChannelType,
        const FString& ConnectionId,
        EChannelFlags Flags = EChannelFlags::State,
        const FChannelRequestDto& Data = {},
        const TOptional<FString>& ChannelId = {},
        const TOptional<FMessagePaginationParamsRequestDto> MessagePagination = {},
        const TOptional<FPaginationParamsRequestDto> MemberPagination = {},
        const TOptional<FPaginationParamsRequestDto> WatcherPagination = {}) const;

    /**
     * Send a message to the given ChannelId of give ChannelType.
     * @param ChannelType Name of built-in or custom channel type (e.g. messaging, team, livestream)
     * @param ChannelId A unique identifier for the channel
     * @param MessageRequest Details of the message to be sent
     * @param bSkipPush Do not send a mobile push notification
     * @param Callback Called when response is received
     */
    void SendNewMessage(
        const FString& ChannelType,
        const FString& ChannelId,
        const FMessageRequestDto& MessageRequest,
        bool bSkipPush = false,
        TCallback<FMessageResponseDto> Callback = {}) const;

    /**
     * Update the given message.
     * @param MessageRequest Details of the message to update
     * @param Callback Called when response is received
     */
    void UpdateMessage(const FMessageRequestDto& MessageRequest, TCallback<FMessageResponseDto> Callback = {}) const;

    /**
     * Delete the message with the given Id
     * @param Id ID of the message to be deleted
     * @param bHard Message is removed from the channel, all replies and all reactions are recursively deleted
     * @param Callback Called when response is received
     */
    void DeleteMessage(const FString& Id, bool bHard = false, TCallback<FMessageResponseDto> Callback = {}) const;

    /**
     * Send a reaction for the given MessageId
     * @param MessageId ID of the message to react to
     * @param ReactionRequest Details of the reaction to be sent
     * @param bEnforceUnique If true, new reaction will replace all reactions the user has (if any) on this message
     * @param bSkipPush Do not send a mobile push notification
     * @param Callback Called when response is received
     */
    void SendReaction(
        const FString& MessageId,
        const FReactionRequestDto& ReactionRequest,
        bool bEnforceUnique = true,
        bool bSkipPush = false,
        TCallback<FReactionResponseDto> Callback = {}) const;

    /**
     * Delete a reaction with the given Type from the message with the given MessageId
     * @param MessageId ID of the message which has been reacted to
     * @param Type Type of reaction to remove
     * @param Callback Called when response is received
     */
    void DeleteReaction(const FString& MessageId, const FName& Type, TCallback<FReactionResponseDto> Callback = {})
        const;

    /**
     * Send a custom or built-in event on this channel
     * @tparam TEvent The event to send
     * @param ChannelType Name of built-in or custom channel type (e.g. messaging, team, livestream)
     * @param ChannelId The unique identifier of the channel
     * @param Callback Called when response is received
     */
    template <class TEvent>
    void SendChannelEvent(
        const FString& ChannelType,
        const FString& ChannelId,
        const TEvent&,
        TCallback<FEventResponseDto> Callback = {});

    /**
     * Query channels with filter query
     * @param ConnectionId Websocket connection ID to interact with.
     * @param Filter The query filters to use. You can query on any of the custom fields you've defined on the Channel.
     * You can also filter other built-in channel fields, see
     * https://getstream.io/chat/docs/other-rest/query_channels/#channel-queryable-built-in-fields
     * @param SortOptions The sorting used for the channels matching the filters.
     * Sorting is based on field and direction, multiple sorting options can be provided.
     * @param MemberLimit How many members should be included for each channel (Max 100)
     * @param MessageLimit How many messages should be included to each channel (Max 300)
     * @param Flags Additional actions to perform, like watch, or fetch presence. @see EChannelFlags
     * @param PaginationOptions Limit and offset
     * @param Callback Called when response is received
     */
    void QueryChannels(
        TCallback<FChannelsResponseDto> Callback,
        const FString& ConnectionId,
        EChannelFlags Flags = EChannelFlags::State | EChannelFlags::Watch,
        const TOptional<FJsonObjectWrapper>& Filter = {},
        const TArray<FSortParamRequestDto>& SortOptions = {},
        TOptional<uint32> MemberLimit = {},
        TOptional<uint32> MessageLimit = {},
        FPaginationOptions PaginationOptions = {}) const;

private:
    explicit FChatApi(const FString& InApiKey, const FString& InHost, const TSharedPtr<FTokenManager>&);
    void SendChannelEvent(
        const FString& ChannelType,
        const FString& ChannelId,
        const UStruct* EventStructDefinition,
        const void* EventStruct,
        TCallback<FEventResponseDto> Callback = {}) const;

    FString BuildUrl(const FString& Path) const;

    void AddAuth(FRequestBuilder&, const FString& Token) const;
    void OnRequest(FRequestBuilder&) const;
    void OnError(const FHttpResponse&, FRequestBuilder&);

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
