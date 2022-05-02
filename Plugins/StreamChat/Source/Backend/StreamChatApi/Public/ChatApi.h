// Copyright 2022 Stream.IO, Inc. All Rights Reserved.

#pragma once

#include "ChannelFlags.h"
#include "CoreMinimal.h"
#include "Request/Channel/ChannelRequestDto.h"
#include "Request/Channel/MessagePaginationParamsRequestDto.h"
#include "Request/Channel/PaginationParamsRequestDto.h"
#include "Request/SortParamRequestDto.h"
#include "StreamJson.h"

class FHttpClient;
class FRequestBuilder;
class FTokenManager;
enum class EPushProvider : uint8;
struct FChannelResponseDto;
struct FChannelSortOption;
struct FChannelStateResponseDto;
struct FChannelsResponseDto;
struct FDeleteChannelResponseDto;
struct FEventResponseDto;
struct FFlagResponseDto;
struct FGuestResponseDto;
struct FHttpResponse;
struct FListDevicesResponseDto;
struct FMarkReadRequestDto;
struct FMarkReadResponseDto;
struct FMessageRequestDto;
struct FMessageResponseDto;
struct FMuteUserResponseDto;
struct FQueryBannedUsersResponseDto;
struct FReactionRequestDto;
struct FReactionResponseDto;
struct FResponseDto;
struct FSearchResponseDto;
struct FToken;
struct FUpdateChannelPartialResponseDto;
struct FUpdateChannelRequestDto;
struct FUpdateChannelResponseDto;
struct FUpdateUsersResponseDto;
struct FUserObjectRequestDto;
struct FUsersResponseDto;

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

#pragma region Moderation
    /** @name Moderation
     *  https://getstream.io/chat/docs/rest/#Moderation
     *  @{
     */

    /**
     * @brief Users can be banned from an app entirely or from a channel.
     * When a user is banned, they will not be allowed to post messages until the ban is
     * removed or expired but will be able to connect to Chat and to channels as before.
     * @param TargetUserId ID of user to ban
     * @param Type Channel type to ban user in (optional, ban is app-wide otherwise)
     * @param Id Channel ID to ban user in (optional, ban is app-wide otherwise)
     * @param Timeout Timeout of ban in minutes. User will be unbanned after this period of time (optional)
     * @param Reason Ban reason (optional)
     * @param bShadow Whether to perform shadow ban or not
     * @param bIpBan Whether to perform IP ban or not
     * @param Callback Called when response is received.
     */
    void BanUser(
        const FString& TargetUserId,
        const FString& Type = {},
        const FString& Id = {},
        TOptional<float> Timeout = {},
        const FString& Reason = {},
        bool bShadow = false,
        bool bIpBan = false,
        TCallback<FResponseDto> Callback = {}) const;

    /**
     * @brief Removes previously applied ban
     * @param TargetUserId ID of user to unban
     * @param Type Channel type to unban user in (optional, unban is app-wide otherwise)
     * @param Id Channel ID to unban user in (optional, unban is app-wide otherwise)
     * @param Callback Called when response is received.
     */
    void UnbanUser(const FString& TargetUserId, const FString& Type = {}, const FString& Id = {}, TCallback<FResponseDto> Callback = {}) const;

    /**
     * @brief Find and filter channel scoped or global user bans
     * @see https://getstream.io/chat/docs/unreal/moderation/#query-bans-endpoint
     * @param Filter Conditions to use to filter the banned users
     * @param SortOptions The sorting used for the banned users matching the filters.
     * Sorting is based on field and direction, multiple sorting options can be provided.
     * @param CreatedAtAfterOrEqual Paginate using the created_at field
     * @param CreatedAtAfter Paginate using the created_at field
     * @param CreatedAtBeforeOrEqual Paginate using the created_at field
     * @param CreatedAtBefore Paginate using the created_at field
     * @param Limit The number of banned users to return
     * @param Offset The pagination offset
     * @param Callback Called when response is received.
     */
    void QueryBannedUsers(
        const TSharedRef<FJsonObject>& Filter,
        const TArray<FSortParamRequestDto>& SortOptions = {},
        const TOptional<FDateTime> CreatedAtAfterOrEqual = {},
        const TOptional<FDateTime> CreatedAtAfter = {},
        const TOptional<FDateTime> CreatedAtBeforeOrEqual = {},
        const TOptional<FDateTime> CreatedAtBefore = {},
        TOptional<uint32> Limit = {},
        TOptional<uint32> Offset = {},
        TCallback<FQueryBannedUsersResponseDto> Callback = {}) const;

    /**
     * @brief Flag a user or message
     * @param TargetMessageId ID of message to flag (optional)
     * @param TargetUserId ID of user to flag (optional)
     * @param Callback Called when response is received.
     */
    void Flag(const FString& TargetMessageId = {}, const FString& TargetUserId = {}, TCallback<FFlagResponseDto> Callback = {}) const;

    /**
     * @brief Mute a user
     * @param TargetUserIds User IDs to mute
     * @param Timeout Duration of mute in minutes (optional)
     * @param Callback Called when response is received.
     */
    void MuteUser(const TArray<FString>& TargetUserIds, TOptional<float> Timeout = {}, TCallback<FMuteUserResponseDto> Callback = {}) const;

    /**
     * @brief Unmute a previously muted user
     * @param TargetUserIds User IDs to unmute
     * @param Callback Called when response is received.
     */
    void UnmuteUser(const TArray<FString>& TargetUserIds, TCallback<FResponseDto> Callback = {}) const;

///@}
#pragma endregion Moderation

#pragma region Channels
    /** @name Channels
     *  https://getstream.io/chat/docs/rest/#Channels
     *  @{
     */

    /**
     * @brief Get messages, members or other channel fields. Creates the channel if not yet created.
     * @param ChannelType Name of built-in or custom channel type (e.g. messaging, team, livestream)
     * @param ConnectionId Websocket connection ID to interact with.
     * @param ChannelId A unique identifier for the channel.
     * @param Flags Additional actions to perform, like watch, or fetch presence. @see EChannelFlags
     * @param Data Properties to set on the channel
     * @param MessagePagination Pagination details for returned messages.
     * @see https://getstream.io/chat/docs/unreal/channel_pagination/
     * @param MemberPagination Pagination details for returned members.
     * @param WatcherPagination Pagination details for returned watchers/
     * @param Callback Called when response is received.
     */
    void QueryChannel(
        const FString& ChannelType,
        const FString& ConnectionId,
        EChannelFlags Flags = EChannelFlags::State,
        const FChannelRequestDto& Data = {},
        const TOptional<FString>& ChannelId = {},
        const TOptional<FMessagePaginationParamsRequestDto> MessagePagination = {},
        const TOptional<FPaginationParamsRequestDto> MemberPagination = {},
        const TOptional<FPaginationParamsRequestDto> WatcherPagination = {},
        TCallback<FChannelStateResponseDto> Callback = {}) const;

    /**
     * @brief Delete a channel. Messages are soft deleted.
     * @param ChannelType Name of built-in or custom channel type (e.g. messaging, team, livestream)
     * @param ChannelId A unique identifier for the channel
     * @param Callback Called when response is received.
     */
    void DeleteChannel(const FString& ChannelType, const FString& ChannelId, TCallback<FDeleteChannelResponseDto> Callback = {}) const;

    /**
     * @brief Updates certain fields of the channel
     * @param ChannelType Name of built-in or custom channel type (e.g. messaging, team, livestream)
     * @param ChannelId A unique identifier for the channel
     * @param Set Sets new field values
     * @param Unset Array of field names to unset
     * @param Callback Called when response is received.
     */
    void PartialUpdateChannel(
        const FString& ChannelType,
        const FString& ChannelId,
        const TSharedRef<FJsonObject>& Set,
        const TArray<FString>& Unset,
        TCallback<FUpdateChannelPartialResponseDto> Callback = {}) const;

    /**
     * @brief Change channel data. Any data that is present on the channel and not included in a full update will be deleted.
     * @param ChannelType Name of built-in or custom channel type (e.g. messaging, team, livestream)
     * @param ChannelId A unique identifier for the channel
     * @param Data Properties to set on the channel
     * @param Callback Called when response is received.
     */
    void UpdateChannel(
        const FString& ChannelType,
        const FString& ChannelId,
        const FUpdateChannelRequestDto& Data,
        TCallback<FUpdateChannelResponseDto> Callback = {}) const;

    /**
     * @brief Hiding a channel will remove it from query channel requests for that user until a new message is added.
     * Hiding a channel is only available to members of that channel.
     * @param ChannelType Name of built-in or custom channel type (e.g. messaging, team, livestream)
     * @param ChannelId A unique identifier for the channel
     * @param bClearHistory Clear the entire message history of that channel for the user
     * @param Callback Called when response is received.
     */
    void HideChannel(const FString& ChannelType, const FString& ChannelId, bool bClearHistory = false, TCallback<FResponseDto> Callback = {}) const;

    /**
     * @brief Un-hide a channel, so it will again appear in query channel requests.
     * @param ChannelType Name of built-in or custom channel type (e.g. messaging, team, livestream)
     * @param ChannelId A unique identifier for the channel
     * @param Callback Called when response is received.
     */
    void ShowChannel(const FString& ChannelType, const FString& ChannelId, TCallback<FResponseDto> Callback = {}) const;

    /**
     * @brief Query channels with filter query
     * @param ConnectionId Websocket connection ID to interact with.
     * @param Filter The query filters to use. You can query on any of the custom fields you've defined on the Channel.
     * You can also filter other built-in channel fields,
     * @see https://getstream.io/chat/docs/unreal/query_channels/#channel-queryable-built-in-fields
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
        const FString& ConnectionId,
        EChannelFlags Flags = EChannelFlags::State | EChannelFlags::Watch,
        const TOptional<TSharedRef<FJsonObject>>& Filter = {},
        const TArray<FSortParamRequestDto>& SortOptions = {},
        TOptional<uint32> MemberLimit = {},
        TOptional<uint32> MessageLimit = {},
        TOptional<uint32> Limit = {},
        TOptional<uint32> Offset = {},
        TCallback<FChannelsResponseDto> Callback = {}) const;

    /**
     * @brief Search all messages
     *
     * ChannelFilter is required, and a minimum of either a query or message filter
     *
     * @see https://getstream.io/chat/docs/unreal/search/
     * @param Query Search phrase
     * @param ChannelFilter Channel filter conditions
     * @param MessageFilter Message filter conditions
     * @param Sort Sort parameters. Cannot be  used with non-zero offset.
     * @param MessageLimit Number of messages to return
     * @param Offset Pagination offset. Cannot be used with sort or next
     * @param Next Pagination parameter. Cannot be used with non-zero offset
     * @param Callback Called when response is received
     */
    void SearchMessages(
        const TSharedRef<FJsonObject>& ChannelFilter,
        const TOptional<FString>& Query = {},
        const TOptional<TSharedRef<FJsonObject>>& MessageFilter = {},
        const TArray<FSortParamRequestDto>& Sort = {},
        TOptional<uint32> MessageLimit = {},
        TOptional<uint32> Offset = {},
        TOptional<FString> Next = {},
        TCallback<FSearchResponseDto> Callback = {}) const;

    /**
     * @brief Mark all messages of all channels as read
     * @param Callback Called when response is received
     */
    void MarkChannelsRead(TCallback<FMarkReadResponseDto> Callback = {}) const;

    /**
     * @brief Mark messages of a channel as read
     * @param ChannelType Name of built-in or custom channel type (e.g. messaging, team, livestream)
     * @param ChannelId A unique identifier for the channel
     * @param MessageId (optional) ID of the message that is considered last read by client
     * @param Callback Called when response is received
     */
    void MarkChannelRead(
        const FString& ChannelType,
        const FString& ChannelId,
        const TOptional<FString>& MessageId = {},
        TCallback<FMarkReadResponseDto> Callback = {}) const;

///@}
#pragma endregion Channels

#pragma region Messages
    /** @name Messages
     *  https://getstream.io/chat/docs/rest/#Messages
     *  @{
     */

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

///@}
#pragma endregion Messages

#pragma region Events
    /** @name Events
     *  https://getstream.io/chat/docs/rest/#Events
     *  @{
     */

    /**
     * @brief Send a custom or built-in event on this channel
     * @tparam TEvent The event to send
     * @param ChannelType Name of built-in or custom channel type (e.g. messaging, team, livestream)
     * @param ChannelId The unique identifier of the channel
     * @param Callback Called when response is received
     */
    template <class TEvent>
    void SendChannelEvent(const FString& ChannelType, const FString& ChannelId, const TEvent&, TCallback<FEventResponseDto> Callback = {});

///@}
#pragma endregion Events

#pragma region Files
/** @name Files
 *  https://getstream.io/chat/docs/rest/#Files
 *  @{
 */

///@}
#pragma endregion Files

#pragma region Users
    /** @name Users
     *  https://getstream.io/chat/docs/rest/#Users
     *  @{
     */

    /**
     * @brief Search for users and see if they are online/offline
     * @param ConnectionId Websocket connection ID to interact with.
     * @param bPresence Get updates when the user goes offline/online
     * @param Filter Conditions to use to filter the users, @see https://getstream.io/chat/docs/unreal/query_users
     * @param SortOptions The sorting used for the users matching the filters.
     * Sorting is based on field and direction, multiple sorting options can be provided.
     * @param Limit The number of users to return
     * @param Offset The pagination offset
     * @param Callback Called when response is received
     */
    void QueryUsers(
        const FString& ConnectionId,
        bool bPresence = true,
        const TOptional<TSharedRef<FJsonObject>>& Filter = {},
        const TArray<FSortParamRequestDto>& SortOptions = {},
        TOptional<uint32> Limit = {},
        TOptional<uint32> Offset = {},
        TCallback<FUsersResponseDto> Callback = {}) const;

    void CreateGuest(const FUserObjectRequestDto& User, TCallback<FGuestResponseDto> Callback = {}) const;

///@}
#pragma endregion Users

#pragma region Devices
    /** @name Devices
     *  https://getstream.io/chat/docs/rest/#Devices
     *  @{
     */

    /**
     * @brief Registering a device associates it with a user and tells the push provider to send new message notifications to the device.
     * @param DeviceId Device-specific identifier
     * @param PushProvider Which provider to use for push notifications
     * @param Callback Called when response is received
     */
    void AddDevice(const FString& DeviceId, EPushProvider PushProvider, TCallback<FResponseDto> Callback = {}) const;

    /**
     * @brief Unregistering a device removes the device from the user and stops further new message notifications.
     * @param DeviceId Device-specific identifier
     * @param Callback Called when response is received
     */
    void RemoveDevice(const FString& DeviceId, TCallback<FResponseDto> Callback = {}) const;

    /**
     * @brief Returns a list of all available devices
     * @param Callback Called when response is received
     */
    void ListDevices(TCallback<FListDevicesResponseDto> Callback = {}) const;

///@}
#pragma endregion Devices

private:
    explicit FChatApi(const FString& InApiKey, const FString& InHost, const TSharedPtr<FTokenManager>&);
    void SendChannelEventInternal(
        const FString& ChannelType,
        const FString& ChannelId,
        const TSharedRef<FJsonObject>& Event,
        TCallback<FEventResponseDto> Callback = {}) const;

    FString BuildUrl(const FString& Path) const;

    void AddAuth(FRequestBuilder&, const FToken& Token) const;
    void OnRequest(FRequestBuilder&) const;
    void OnError(const FHttpResponse&, FRequestBuilder&) const;

    TSharedPtr<FTokenManager> TokenManager;
    FString ApiKey;

    TSharedPtr<FHttpClient> Client;
    FString Scheme;
    FString Host;
};

template <class TEvent>
void FChatApi::SendChannelEvent(const FString& ChannelType, const FString& ChannelId, const TEvent& Event, const TCallback<FEventResponseDto> Callback)
{
    static_assert(
        TIsSame<decltype(TEvent::StaticType()), decltype(TEvent::StaticType())>::Value,
        "TEvent must have a static method 'StaticType' which returns a FName to use with SendChannelEvent.");
    SendChannelEventInternal(ChannelType, ChannelId, JsonObject::UStructToJsonObject<TEvent>(Event), Callback);
}
