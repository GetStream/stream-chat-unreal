// Copyright 2021 Stream.IO, Inc. All Rights Reserved.

#pragma once

#include "Channel/Filter.h"
#include "Channel/SortOption.h"
#include "ChatSocketEvents.h"
#include "Components/ActorComponent.h"
#include "CoreMinimal.h"
#include "Engine/LatentActionManager.h"
#include "IChatSocket.h"
#include "User/User.h"
#include "User/UserRef.h"

#include "StreamChatClientComponent.generated.h"

struct FConnectionRecoveredEvent;
struct FChannelStateResponseFieldsDto;
struct FUserPresenceChangedEvent;
class FChatApi;
class FTokenManager;
class FUserManager;
class ITokenProvider;
class UChatChannel;

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
     * Create a connection to the API for the given user and using a custom token provider.

     * @param User Generally only the user id is required
     * @param TokenProvider Can be used to asynchronously generate tokens from your own backend
     * @param Callback Called when a response is received from the API
     * @return Latest info if the logged in user
     */
    void ConnectUser(const FUser& User, TUniquePtr<ITokenProvider> TokenProvider, TFunction<void(const FUserRef&)> Callback = {});

    /**
     * Create a connection to the API for the given user and credentials.

     * @param User Generally only the user id is required
     * @param Token A JWT token for the given user
     * @param Callback Called when a response is received from the API
     * @return Latest info if the logged in user
     */
    void ConnectUser(const FUser& User, const FString& Token, TFunction<void(const FUserRef&)> Callback = {});

    /**
     * Close the connection to the API and resets any state
     */
    void DisconnectUser();

    /**
     * Query the API for all channels which match the given filter. Will also automatically watch all channels.
     *
     * @param Callback Called when a response is received from the API
     * @param Filter The query filters to use. You can query on any of the custom fields you've defined on the Channel.
     * As a minimum, the filter should be something like: { members: { $in: [userID] } }
     * @param SortOptions The sorting used for the channels matching the filters. Sorting is based on field and
     * direction, and multiple sorting options can be provided.
     * @return An array of channel objects which can be used to interact with the channels
     */
    void QueryChannels(TFunction<void(const TArray<UChatChannel*>&)> Callback, TOptional<FFilter> Filter = {}, const TArray<FSortOption>& SortOptions = {});

    /**
    * Create a channel if it doesn't exist yet (if this user has the right permissions), get data about the channel
    (including members, watchers and messages) and subscribe to future updates

     * @param Callback Called when a response is received from the API
     * @param Type The channel type. Default types are livestream, messaging, team, gaming and commerce. You can also
    create your own types.
     * @param Members The members participating in this Channel (optional)
     * @param Id A unique name for the channel (optional)
     * @return A channel object which can be used to interact with the channel
     */
    void WatchChannel(
        TFunction<void(UChatChannel*)> Callback,
        const FString& Type,
        const TOptional<FString>& Id = {},
        const TOptional<TArray<FString>>& Members = {});

    // TODO does this need to be exposed?
    void UpdateMessage(const FString& Id, const FString& Text) const;
    void DeleteMessage(const FString& Id) const;

    UFUNCTION(BlueprintPure, Category = "Stream Chat|Client")
    const TArray<UChatChannel*>& GetChannels() const;

    UPROPERTY(EditAnywhere, Config, Category = "Stream Chat", meta = (DisplayName = "API Key"))
    FString ApiKey;

private:
    // Called when the game starts
    virtual void BeginPlay() override;

    void ConnectUserInternal(const FUser& User, TFunction<void(const FUserRef&)> Callback);
    UChatChannel* CreateChannelObject(const FChannelStateResponseFieldsDto&);

    void OnConnectionRecovered(const FConnectionRecoveredEvent&);
    void OnUserPresenceChanged(const FUserPresenceChangedEvent&);

    TSharedPtr<FTokenManager> TokenManager;
    TSharedPtr<FChatApi> Api;
    TSharedPtr<FUserManager> UserManager;

    UPROPERTY(Transient)
    TArray<UChatChannel*> Channels;

#pragma region Blueprint

public:
    /**
     * Create a connection to the API for the given user and credentials.

     * @param User Generally only the user id is required
     * @param Token A JWT token for the given user
     * @param OutUser Latest info if the logged in user
     */
    UFUNCTION(BlueprintCallable, Category = "Stream Chat|Client", meta = (Latent, WorldContext = WorldContextObject, LatentInfo = LatentInfo))
    void ConnectUser(const FUser& User, const FString& Token, const UObject* WorldContextObject, FLatentActionInfo LatentInfo, FUserRef& OutUser);

    /**
     * Query the API for all channels which match the given filter. Will also automatically watch all channels.
     *
     * @param Filter The query filters to use. You can query on any of the custom fields you've defined on the Channel.
     * As a minimum, the filter should be something like: { members: { $in: [userID] } }
     * @param SortOptions The sorting used for the channels matching the filters. Sorting is based on field and
     * direction, and multiple sorting options can be provided.
     * @param OutChannels An array of channel objects which can be used to interact with the channels
     */
    UFUNCTION(BlueprintCallable, Category = "Stream Chat|Client", meta = (Latent, WorldContext = WorldContextObject, LatentInfo = LatentInfo))
    void QueryChannels(
        FFilter Filter,
        const TArray<FSortOption>& SortOptions,
        const UObject* WorldContextObject,
        FLatentActionInfo LatentInfo,
        TArray<UChatChannel*>& OutChannels);

    /**
    * Create a channel if it doesn't exist yet (if this user has the right permissions), get data about the channel
    (including members, watchers and messages) and subscribe to future updates

     * @param Type The channel type. Default types are livestream, messaging, team, gaming and commerce. You can also
    create your own types.
     * @param Members The members participating in this Channel (optional)
     * @param Id A unique name for the channel (optional)
     * @param OutChannel Object which can be used to interact with the channel
     */
    UFUNCTION(BlueprintCallable, Category = "Stream Chat|Client", meta = (Latent, WorldContext = WorldContextObject, LatentInfo = LatentInfo, AdvancedDisplay = Id))
    void WatchChannel(
        const FString& Type,
        const TArray<FString>& Members,
        const FString& Id,
        const UObject* WorldContextObject,
        FLatentActionInfo LatentInfo,
        UChatChannel*& OutChannel);

#pragma endregion Blueprint

#pragma region Events

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

#pragma endregion Events
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
