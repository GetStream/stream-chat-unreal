// Copyright 2021 Stream.IO, Inc. All Rights Reserved.

#pragma once

#include "ChannelFlags.h"
#include "CoreMinimal.h"
#include "JsonObjectWrapper.h"
#include "Request/Channel/ChannelRequestDto.h"
#include "Request/Channel/MessagePaginationParamsRequestDto.h"
#include "Request/Channel/PaginationParamsRequestDto.h"
#include "Request/SortParamRequestDto.h"
#include "Response/Event/EventResponseDto.h"
#include "Response/Message/SearchResponseDto.h"
#include "Response/Reaction/ReactionResponseDto.h"
#include "StreamJson.h"

class FHttpClient;
class FRequestBuilder;
class FTokenManager;
struct FChannelResponseDto;
struct FChannelStateResponseDto;
struct FChannelsResponseDto;
struct FHttpResponse;
struct FMessageRequestDto;
struct FMessageResponseDto;
struct FReactionRequestDto;
struct FChannelSortOption;

template <class T>
using TCallback = TFunction<void(const T&)>;

/**
 * @brief All Stream Chat REST API requests can be made via this object
 * @see https://getstream.io/chat/docs/rest/
 * @ingroup StreamChatApi
 */
class STREAMCHATAPI_API FChatApi : public TSharedFromThis<FChatApi>
{
public:
    static TSharedRef<FChatApi> Create(const FString& InApiKey, const FString& InHost, const TSharedPtr<FTokenManager>&);

    /**
     * @brief Get messages, members or other channel fields. Creates the channel if not yet created.
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
     * @brief Send a message to the given @param ChannelId of the given @param ChannelType .
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
     * @brief Update the given message.
     * @param MessageRequest Details of the message to update
     * @param Callback Called when response is received
     */
    void UpdateMessage(const FMessageRequestDto& MessageRequest, TCallback<FMessageResponseDto> Callback = {}) const;

    /**
     * @brief Delete the message with the given ID
     * @param Id ID of the message to be deleted
     * @param bHard Message is removed from the channel, all replies and all reactions are recursively deleted
     * @param Callback Called when response is received
     */
    void DeleteMessage(const FString& Id, bool bHard = false, TCallback<FMessageResponseDto> Callback = {}) const;

    /**
     * @brief Send a reaction for the given MessageId
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
     * @brief Delete a reaction with the given Type from the message with the given MessageId
     * @param MessageId ID of the message which has been reacted to
     * @param Type Type of reaction to remove
     * @param Callback Called when response is received
     */
    void DeleteReaction(const FString& MessageId, const FName& Type, TCallback<FReactionResponseDto> Callback = {}) const;

    /**
     * @brief Send a custom or built-in event on this channel
     * @tparam TEvent The event to send
     * @param ChannelType Name of built-in or custom channel type (e.g. messaging, team, livestream)
     * @param ChannelId The unique identifier of the channel
     * @param Callback Called when response is received
     */
    template <class TEvent>
    void SendChannelEvent(const FString& ChannelType, const FString& ChannelId, const TEvent&, TCallback<FEventResponseDto> Callback = {});

    /**
     * @brief Query channels with filter query
     * @param ConnectionId Websocket connection ID to interact with.
     * @param Filter The query filters to use. You can query on any of the custom fields you've defined on the Channel.
     * You can also filter other built-in channel fields,
     * @see https://getstream.io/chat/docs/other-rest/query_channels/#channel-queryable-built-in-fields
     * @param SortOptions The sorting used for the channels matching the filters.
     * Sorting is based on field and direction, multiple sorting options can be provided.
     * @param MemberLimit How many members should be included for each channel (Max 100)
     * @param MessageLimit How many messages should be included to each channel (Max 300)
     * @param Limit The number of channels to return (max is 30, optional)
     * @param Offset The pagination offset (max is 1000, optional)
     * @param Flags Additional actions to perform, like watch, or fetch presence. @see EChannelFlags
     * @param Callback Called when response is received
     */
    void QueryChannels(
        TCallback<FChannelsResponseDto> Callback,
        const FString& ConnectionId,
        EChannelFlags Flags = EChannelFlags::State | EChannelFlags::Watch,
        const TOptional<TSharedRef<FJsonObject>>& Filter = {},
        const TArray<FSortParamRequestDto>& SortOptions = {},
        TOptional<uint32> MemberLimit = {},
        TOptional<uint32> MessageLimit = {},
        TOptional<uint32> Limit = {},
        TOptional<uint32> Offset = {}) const;

    /**
     * @brief Search all messages
     *
     * ChannelFilter is required, and a minimum of either a query or message filter
     *
     * @see https://getstream.io/chat/docs/other-rest/search/
     * @param Callback Called when response is received
     * @param Query Search phrase
     * @param ChannelFilter Channel filter conditions
     * @param MessageFilter Message filter conditions
     * @param Sort Sort parameters. Cannot be  used with non-zero offset.
     * @param MessageLimit Number of messages to return
     * @param Offset Pagination offset. Cannot be used with sort or next
     * @param Next Pagination parameter. Cannot be used with non-zero offset
     */
    void SearchMessages(
        TCallback<FSearchResponseDto> Callback,
        const TSharedRef<FJsonObject>& ChannelFilter,
        const TOptional<FString>& Query = {},
        const TOptional<TSharedRef<FJsonObject>>& MessageFilter = {},
        const TArray<FSortParamRequestDto>& Sort = {},
        TOptional<uint32> MessageLimit = {},
        TOptional<uint32> Offset = {},
        TOptional<FString> Next = {}) const;

private:
    explicit FChatApi(const FString& InApiKey, const FString& InHost, const TSharedPtr<FTokenManager>&);
    void SendChannelEventInternal(
        const FString& ChannelType,
        const FString& ChannelId,
        const TSharedRef<FJsonObject>& Event,
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
void FChatApi::SendChannelEvent(const FString& ChannelType, const FString& ChannelId, const TEvent& Event, const TCallback<FEventResponseDto> Callback)
{
    static_assert(TIsSame<decltype(TEvent::StaticType), FName>::Value, "TEvent must have a FName member 'StaticType' to use with SendChannelEvent.");
    SendChannelEventInternal(ChannelType, ChannelId, JsonObject::UStructToJsonObject<TEvent>(Event), Callback);
}
