// Copyright 2022 Stream.IO, Inc. All Rights Reserved.

#pragma once

#include "Channel/ChannelProperties.h"
#include "Channel/ChannelSortOption.h"
#include "Channel/ChatChannel.h"
#include "Channel/Filter.h"
#include "ChannelFlags.h"
#include "ChatSocketEvents.h"
#include "Components/ActorComponent.h"
#include "CoreMinimal.h"
#include "Engine/LatentActionManager.h"
#include "Event/Notification/NotificationMutesUpdatedEvent.h"
#include "IChatSocket.h"
#include "Moderation/BanPaginationOptions.h"
#include "PaginationOptions.h"
#include "User/OwnUser.h"
#include "User/User.h"

#include "StreamChatClientComponent.generated.h"

struct FBan;
class FChatApi;
class FTokenManager;
class ITokenProvider;
class UChatChannel;
struct FUserRef;
struct FChannelStateResponseFieldsDto;
struct FConnectionRecoveredEvent;
struct FMessage;
struct FDevice;
struct FUserPresenceChangedEvent;

/**
 * @brief A component which allows for connecting to the Stream Chat API.
 * Should be added to a client-side actor, e.g. HUD.
 * @ingroup StreamChat
 */
UCLASS(ClassGroup = (Custom), meta = (BlueprintSpawnableComponent))
class STREAMCHAT_API UStreamChatClientComponent final : public UActorComponent
{
    GENERATED_BODY()

public:
    /// Initialize the component. Shouldn't be called directly.
    UStreamChatClientComponent();

    /**
     * @brief Create a connection to the API for the given user and using a custom token provider.
     * @param User Generally only the user id is required
     * @param TokenProvider Can be used to asynchronously generate tokens from your own backend
     * @param Callback Called when a response is received from the API
     * @return Latest info if the logged in user
     */
    void ConnectUser(const FUser& User, TUniquePtr<ITokenProvider> TokenProvider, TFunction<void(const FOwnUser&)> Callback = {});

    /**
     * @brief Create a connection to the API for the given user and credentials.
     * @param User Generally only the user id is required
     * @param Token A JWT token for the given user
     * @param Callback Called when a response is received from the API
     * @return Latest info if the logged in user
     */
    void ConnectUser(const FUser& User, const FString& Token, TFunction<void(const FOwnUser&)> Callback = {});

    /**
     * @brief Create a connection to the API anonymously
     * @param Callback Called when a response is received from the API
     * @return Anonymous user info
     */
    void ConnectAnonymousUser(TFunction<void(const FOwnUser&)> Callback = {});

    /**
     * @brief Create a connection to the API by creating a guest account
     * @param User Generally only the user id is required
     * @param Callback Called when a response is received from the API
     * @return Anonymous user info
     */
    void ConnectGuestUser(const FUser& User, TFunction<void(const FOwnUser&)> Callback = {});

    /**
     * @brief Close the connection to the API and resets any state
     */
    UFUNCTION(BlueprintCallable, Category = "Stream Chat|Client")
    void DisconnectUser();

    /**
     * @brief Create or update one or more users
     */
    UFUNCTION(BlueprintCallable, Category = "Stream Chat|Client")
    void UpsertUsers(const TArray<FUser>& Users);

    /// Create a null chat channel for starting a new chat/group
    UFUNCTION(BlueprintCallable, Category = "Stream Chat|Client")
    UChatChannel* NewChat();
    /// Remove any null chat channels used for starting a new chat/group
    UFUNCTION(BlueprintCallable, Category = "Stream Chat|Client")
    void CancelNewChat();

    /**
     * @brief Query the API for all channels which match the given filter. Will also automatically watch all channels.
     * @param Callback Called when a response is received from the API
     * @param Filter The query filters to use. You can query on any of the custom fields you've defined on the Channel.
     * As a minimum, the filter should be something like: { members: { $in: [userID] } }
     * @param SortOptions The sorting used for the channels matching the filters. Sorting is based on field and
     * direction, and multiple sorting options can be provided.
     * @param Flags Get state, get presence and/or watch
     * @param PaginationOptions Pagination offset and limit
     * @return An array of channel objects which can be used to interact with the channels
     */
    void QueryChannels(
        TOptional<FFilter> Filter = {},
        const TArray<FChannelSortOption>& SortOptions = {},
        const EChannelFlags Flags = EChannelFlags::State | EChannelFlags::Watch,
        const FPaginationOptions& PaginationOptions = {},
        TFunction<void(const TArray<UChatChannel*>&)> Callback = {});

    /**
     * @brief Create a channel if it does not exist yet (if this user has the right permissions).
     * @param ChannelProperties Properties of the channel to create
     * @param Callback Called when a response is received from the API
     * @return A channel object which can be used to interact with the channel
     */
    void CreateChannel(const FChannelProperties& ChannelProperties, TFunction<void(UChatChannel*)> Callback);

    /**
     * @brief Create a channel if it does not exist yet (if this user has the right permissions), get data about the channel (including members, watchers and
     * messages) and subscribe to future updates
     * @param ChannelProperties Properties of the channel to watch
     * @param Callback Called when a response is received from the API
     * @return A channel object which can be used to interact with the channel
     */
    void WatchChannel(const FChannelProperties& ChannelProperties, TFunction<void(UChatChannel*)> Callback);

    /**
     * @brief Create a channel if it does not exist yet (if this user has the right permissions).
     * Includes options to get data about the channel (including members, watchers and messages), subscribe to future updates.
     * @param Flags Get state, get presence and/or watch
     * @param ChannelProperties Properties of the channel to query
     * @param Callback Called when a response is received from the API
     * @return A channel object which can be used to interact with the channel
     */
    void QueryChannel(const FChannelProperties& ChannelProperties, const EChannelFlags Flags, TFunction<void(UChatChannel*)> Callback = {});

    /**
     * @brief Fetch more channels from the server.
     *
     * @attention Some messages must already have been fetched for this to do anything.
     * @param Limit Number of channels returned is limited by this value. Maximum 30.
     * @param Callback Called when a response is received from the backend
     */
    void QueryAdditionalChannels(int32 Limit = 10, TFunction<void()> Callback = {});

    /**
     * @brief Search for users and see if they are online/offline
     * @param bPresence Get updates when the user goes offline/online
     * @param Filter Conditions to use to filter the users, @see https://getstream.io/chat/docs/unreal/query_users
     * @param Sort The sorting used for the users matching the filters.
     * Sorting is based on field and direction, multiple sorting options can be provided.
     * @param PaginationOptions The number of users to return and pagination offset
     * @param Callback Called when response is received
     */
    void QueryUsers(
        const FFilter& Filter = {},
        const TArray<FUserSortOption>& Sort = {},
        bool bPresence = true,
        const TOptional<FPaginationOptions> PaginationOptions = {},
        TFunction<void(const TArray<FUserRef>&)> Callback = {}) const;

    /**
     * @brief Search all messages
     * A channel filter is required.
     * All other parameters are optional, but a minimum of either a query or message filter is additionally required
     *
     * @param Callback Called when response is received
     * @param Query Search phrase
     * @param ChannelFilter Channel filter conditions
     * @param MessageFilter Message filter conditions
     * @param Sort Sort parameters. Cannot be  used with non-zero offset.
     * @param MessageLimit Number of messages to return
     */
    void SearchMessages(
        const FFilter& ChannelFilter = {},
        const TOptional<FString>& Query = {},
        const TOptional<FFilter>& MessageFilter = {},
        const TArray<FMessageSortOption>& Sort = {},
        TOptional<uint32> MessageLimit = {},
        TFunction<void(const TArray<FMessage>&)> Callback = {}) const;

    /**
     * @brief Mark all channels as read for the current user
     */
    UFUNCTION(BlueprintCallable, BlueprintPure = false, Category = "Stream Chat|Client")
    void MarkAllRead() const;

    UFUNCTION(BlueprintPure, Category = "Stream Chat|Client")
    const TArray<UChatChannel*>& GetChannels() const;

    UFUNCTION(BlueprintPure, Category = "Stream Chat|Client")
    static FString DevToken(const FString& UserId);

    /**
     * @brief Registering a device associates it with a user and tells the push provider to send new message notifications to the device.
     * @param DeviceId Device-specific identifier
     * @param PushProvider Which provider to use for push notifications
     */
    UFUNCTION(BlueprintCallable, Category = "Stream Chat|Client")
    void AddDevice(const FString& DeviceId, EPushProvider PushProvider) const;

    /**
     * @brief Unregistering a device removes the device from the user and stops further new message notifications.
     * @param DeviceId Device-specific identifier
     */
    UFUNCTION(BlueprintCallable, Category = "Stream Chat|Client")
    void RemoveDevice(const FString& DeviceId) const;

    /**
     * @brief Returns a list of all available devices
     * @param Callback Called when response is received
     */
    void ListDevices(TFunction<void(TArray<FDevice>)> Callback) const;

    UPROPERTY(EditAnywhere, Config, Category = "Stream Chat", meta = (DisplayName = "API Key"))
    FString ApiKey;

    DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FChannelsUpdatedDelegate, const TArray<UChatChannel*>&, Channels);
    /// Fired when any of the channels we have locally change
    UPROPERTY(BlueprintAssignable, Category = "Stream Chat|Channel")
    FChannelsUpdatedDelegate ChannelsUpdated;

private:
    // Called when the game starts
    virtual void BeginPlay() override;

    void ConnectUserInternal(const FUser& User, TFunction<void(const FOwnUser&)> Callback);
    UChatChannel* CreateChannelObject(const FChannelStateResponseFieldsDto&);

    void OnConnectionRecovered(const FConnectionRecoveredEvent&);
    void OnUserPresenceChanged(const FUserPresenceChangedEvent&);
    void OnNewMessage(const FMessageNewEvent&);
    void OnMutesUpdated(const FNotificationMutesUpdatedEvent&);

    void AddChannels(const TArray<UChatChannel*>& InChannels);

    TSharedPtr<FTokenManager> TokenManager;
    TSharedPtr<FChatApi> Api;

    UPROPERTY(Transient)
    TArray<UChatChannel*> Channels;

#pragma region Blueprint
    /** @name Blueprint
     *  @{
     */

public:
    /**
     * Create a connection to the API for the given user and credentials.
     * @param User Generally only the user id is required
     * @param Token A JWT token for the given user
     * @param OutUser Latest info of the logged in user
     */
    UFUNCTION(BlueprintCallable, Category = "Stream Chat|Client", meta = (Latent, WorldContext = WorldContextObject, LatentInfo = LatentInfo))
    void ConnectUser(const FUser& User, const FString& Token, const UObject* WorldContextObject, FLatentActionInfo LatentInfo, FOwnUser& OutUser);

    /**
     * Create a connection to the API anonymously
     * @param OutUser Anonymous user info
     */
    UFUNCTION(BlueprintCallable, Category = "Stream Chat|Client", meta = (Latent, WorldContext = WorldContextObject, LatentInfo = LatentInfo))
    void ConnectAnonymousUser(const UObject* WorldContextObject, FLatentActionInfo LatentInfo, FOwnUser& OutUser);

    /**
     * Create a connection to the API by creating a guest account
     * @param User Generally only the user id is required
     * @param OutUser Guest user info
     */
    UFUNCTION(BlueprintCallable, Category = "Stream Chat|Client", meta = (Latent, WorldContext = WorldContextObject, LatentInfo = LatentInfo))
    void ConnectGuestUser(const FUser& User, const UObject* WorldContextObject, FLatentActionInfo LatentInfo, FOwnUser& OutUser);

    /**
     * Query the API for all channels which match the given filter. Will also automatically watch all channels.
     *
     * @param Filter The query filters to use. You can query on any of the custom fields you've defined on the Channel.
     * As a minimum, the filter should be something like: { members: { $in: [userID] } }
     * @param SortOptions The sorting used for the channels matching the filters. Sorting is based on field and
     * direction, and multiple sorting options can be provided.
     * @param Flags Get state, get presence and/or watch
     * @param OutChannels An array of channel objects which can be used to interact with the channels
     */
    UFUNCTION(BlueprintCallable, Category = "Stream Chat|Client", meta = (Latent, WorldContext = WorldContextObject, LatentInfo = LatentInfo))
    void QueryChannels(
        FFilter Filter,
        const TArray<FChannelSortOption>& SortOptions,
        const UObject* WorldContextObject,
        FLatentActionInfo LatentInfo,
        TArray<UChatChannel*>& OutChannels,
        UPARAM(meta = (Bitmask, BitmaskEnum = EChannelFlags)) const int32 Flags = 3);

    /**
    * Create a channel if it does not exist yet (if this user has the right permissions), get data about the channel
    (including members, watchers and messages) and subscribe to future updates

     * @param ChannelProperties Properties of the channel to watch
     * @param OutChannel Object which can be used to interact with the channel
     */
    UFUNCTION(BlueprintCallable, Category = "Stream Chat|Client", meta = (Latent, WorldContext = WorldContextObject, LatentInfo = LatentInfo, AdvancedDisplay = Id))
    void WatchChannel(const FChannelProperties& ChannelProperties, const UObject* WorldContextObject, FLatentActionInfo LatentInfo, UChatChannel*& OutChannel);

    ///@}
#pragma endregion Blueprint

#pragma region Events
    /** @name Events
     *  @{
     */

public:
    template <class TEvent>
    using TEventMulticastDelegate = TMulticastDelegate<void(const TEvent& Event)>;
    template <class TEvent>
    using TEventDelegate = typename TEventMulticastDelegate<TEvent>::FDelegate;
    template <class TEvent, class UserClass>
    using TEventDelegateUObjectMethodPtr = typename TEventDelegate<TEvent>::template TUObjectMethodDelegate<UserClass>::FMethodPtr;
    template <class TEvent, class UserClass>
    using TEventDelegateSpMethodPtr = typename TEventDelegate<TEvent>::template TSPMethodDelegate<UserClass>::FMethodPtr;

    /// Subscribe to a client event using your own delegate object
    template <class TEvent>
    FDelegateHandle On(TEventDelegate<TEvent> Callback);

    /**
     * Subscribe to a client event using a UObject-based member function delegate.
     * @tparam TEvent Event type
     * @param Obj UObject with given method
     * @param Method Method to call when event occurs. Should have signature: void OnMyEvent(const TEvent& Event)
     * @return A handle which can be used to unsubscribe from the event
     */
    template <class TEvent, class UserClass>
    typename TEnableIf<TIsDerivedFrom<UserClass, UObject>::IsDerived, FDelegateHandle>::Type On(
        UserClass* Obj,
        TEventDelegateUObjectMethodPtr<TEvent, UserClass> Method);

    /**
     * Subscribe to a client event using a shared pointer-based (fast, not thread-safe) member function delegate.
     * @tparam TEvent Event type
     * @param Obj Object with given method
     * @param Method Method to call when event occurs. Should have signature: void OnMyEvent(const TEvent& Event)
     * @return A handle which can be used to unsubscribe from the event
     */
    template <class TEvent, class UserClass>
    typename TEnableIf<!TIsDerivedFrom<UserClass, UObject>::IsDerived, FDelegateHandle>::Type On(
        UserClass* Obj,
        TEventDelegateSpMethodPtr<TEvent, UserClass> Method);

    /**
     * Subscribe to a client event using a C++ lambda
     * @tparam TEvent Event type
     * @param Functor Lambda to execute when event occurs. Should have signature similar to: [](const TEvent&
     * Event){}
     * @param Vars Additional variables to pass to the lambda
     * @return A handle which can be used to unsubscribe from the event
     */
    template <class TEvent, typename FunctorType, typename... VarTypes>
    typename TEnableIf<TIsInvocable<FunctorType, const TEvent&, VarTypes...>::Value, FDelegateHandle>::Type On(FunctorType&& Functor, VarTypes... Vars);

    template <class TEvent>
    bool Unsubscribe(FDelegateHandle) const;

private:
    TSharedPtr<IChatSocket> Socket;

    ///@}
#pragma endregion Events

#pragma region Moderation
    /** @name Moderation
     *  @{
     */

public:
    /**
     * @brief Ban a user from app entirely
     * When a user is banned, they will not be allowed to post messages until the ban is
     * removed or expired but will be able to connect to Chat and to channels as before.
     * @param User User to ban
     * @param Timeout Timeout of ban. User will be unbanned after this period of time (unlimited if zero)
     * @param Reason Ban reason (optional)
     * @param bIpBan Whether to perform IP ban or not
     */
    UFUNCTION(BlueprintCallable, BlueprintPure = false, Category = "Stream Chat|Client|Moderation", DisplayName = "Ban User")
    void BanUserBP(const FUserRef& User, FTimespan Timeout, const FString& Reason, bool bIpBan) const;
    void BanUser(const FUserRef& User, const TOptional<FTimespan>& Timeout = {}, const TOptional<FString>& Reason = {}, bool bIpBan = false) const;

    /**
     * @brief Remove previously applied app-wide ban
     * @param User User to unban
     */
    UFUNCTION(BlueprintCallable, Category = "Stream Chat|Client|Moderation")
    void UnbanUser(const FUserRef& User) const;

    /**
     * @brief Shadow ban a user from app entirely
     * When a user is banned, they will not be allowed to post messages until the ban is
     * removed or expired but will be able to connect to Chat and to channels as before.
     * @param User User to ban
     * @param Timeout Timeout of ban. User will be unbanned after this period of time (unlimited if zero)
     */
    UFUNCTION(BlueprintCallable, BlueprintPure = false, Category = "Stream Chat|Client|Moderation", DisplayName = "Shadow Ban User")
    void ShadowBanUserBP(const FUserRef& User, FTimespan Timeout) const;
    void ShadowBanUser(const FUserRef& User, const TOptional<FTimespan>& Timeout = {}) const;

    /**
     * @brief Remove previously applied app-wide shadow ban
     * @param User User to unban
     */
    UFUNCTION(BlueprintCallable, Category = "Stream Chat|Client|Moderation")
    void ShadowUnbanUser(const FUserRef& User) const;

    /**
     * @brief Find and filter channel-scoped or global user bans
     * @see https://getstream.io/chat/docs/unreal/moderation/#query-bans-endpoint
     * @param Filter Conditions to use to filter the banned users
     * @param SortOptions The sorting used for the banned users matching the filters.
     * Sorting is based on field and direction, multiple sorting options can be provided.
     * @param PaginationOptions Pagination for bans can be done in two ways: using offset/limit or using the "created_at" field.
     * @param Callback Called when response is received.
     */
    void QueryBannedUsers(
        const FFilter& Filter,
        const TArray<FBanSortOption>& SortOptions = {},
        const TOptional<FBanPaginationOptions> PaginationOptions = {},
        const TFunction<void(const TArray<FBan>&)> Callback = {});

    /**
     * @brief Flag message for moderation
     *
     * @param Message A reference to an existing message
     */
    UFUNCTION(BlueprintCallable, BlueprintPure = false, Category = "Stream Chat|Client")
    void FlagMessage(const FMessage& Message) const;

    /**
     * @brief Flag user for moderation
     *
     * @param User A reference to a user
     */
    UFUNCTION(BlueprintCallable, BlueprintPure = false, Category = "Stream Chat|Client")
    void FlagUser(const FUserRef& User) const;

    /**
     * @brief Mute the given user
     * @param Timeout Timeout of ban. User will be unbanned after this period of time (unlimited if zero)
     *
     * @param User A reference to a user
     */
    UFUNCTION(BlueprintCallable, BlueprintPure = false, Category = "Stream Chat|Client", DisplayName = "Mute User")
    void MuteUserBP(const FUserRef& User, FTimespan Timeout) const;
    void MuteUser(const FUserRef& User, const TOptional<FTimespan>& Timeout = {}) const;

    /**
     * @brief Unmute a previously muted user
     *
     * @param User A reference to a user
     */
    UFUNCTION(BlueprintCallable, Category = "Stream Chat|Client")
    void UnmuteUser(const FUserRef& User) const;

    ///@}
#pragma endregion Moderation
};

#pragma region Events

template <class TEvent>
FDelegateHandle UStreamChatClientComponent::On(TEventDelegate<TEvent> Callback)
{
    return Socket->Events().SubscribeLambda<TEvent>(
        [this, Callback](const TEvent& Event)
        {
            // TODO can we enforce client-only events?
            Callback.ExecuteIfBound(Event);
        });
}

template <class TEvent, class UserClass>
typename TEnableIf<TIsDerivedFrom<UserClass, UObject>::IsDerived, FDelegateHandle>::Type UStreamChatClientComponent::On(
    UserClass* Obj,
    TEventDelegateUObjectMethodPtr<TEvent, UserClass> Method)
{
    const TEventDelegate<TEvent> Delegate = TEventDelegate<TEvent>::CreateUObject(Obj, Method);
    return On<TEvent>(Delegate);
}

template <class TEvent, class UserClass>
typename TEnableIf<!TIsDerivedFrom<UserClass, UObject>::IsDerived, FDelegateHandle>::Type UStreamChatClientComponent::On(
    UserClass* Obj,
    TEventDelegateSpMethodPtr<TEvent, UserClass> Method)
{
    const TEventDelegate<TEvent> Delegate = TEventDelegate<TEvent>::CreateSP(Obj, Method);
    return On<TEvent>(Delegate);
}

template <class TEvent, typename FunctorType, typename... VarTypes>
typename TEnableIf<TIsInvocable<FunctorType, const TEvent&, VarTypes...>::Value, FDelegateHandle>::Type UStreamChatClientComponent::On(
    FunctorType&& Functor,
    VarTypes... Vars)
{
    const TEventDelegate<TEvent> Delegate = TEventDelegate<TEvent>::CreateLambda(Forward<FunctorType>(Functor), Vars...);
    return On<TEvent>(Delegate);
}

template <class TEvent>
bool UStreamChatClientComponent::Unsubscribe(const FDelegateHandle Handle) const
{
    return Socket->Events().Unsubscribe<TEvent>(Handle);
}
#pragma endregion Events
