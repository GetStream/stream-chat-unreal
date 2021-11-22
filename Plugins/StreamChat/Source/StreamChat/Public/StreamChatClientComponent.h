// Copyright Stream.IO, Inc. All Rights Reserved.

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

UCLASS(ClassGroup = (Custom), meta = (BlueprintSpawnableComponent))
class STREAMCHAT_API UStreamChatClientComponent final : public UActorComponent
{
    GENERATED_BODY()

public:
    UStreamChatClientComponent();

    void ConnectUser(
        const FUser& User,
        TUniquePtr<ITokenProvider> TokenProvider,
        TFunction<void(const FUserRef&)> Callback = {});
    void ConnectUser(const FUser& User, const FString& Token, TFunction<void(const FUserRef&)> Callback = {});

    void DisconnectUser();

    void QueryChannels(
        TFunction<void(const TArray<UChatChannel*>&)> Callback,
        TOptional<FFilter> Filter = {},
        const TArray<FSortOption>& SortOptions = {});

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

    UFUNCTION(BlueprintCallable, Category = "Stream Chat|Client", meta = (Latent, WorldContext = WorldContextObject, LatentInfo = LatentInfo))
    void ConnectUser(
        const FUser& User,
        const FString& Token,
        const UObject* WorldContextObject,
        FLatentActionInfo LatentInfo,
        FUserRef& OutUser);

    UFUNCTION(BlueprintCallable, Category = "Stream Chat|Client", meta = (Latent, WorldContext = WorldContextObject, LatentInfo = LatentInfo))
    void QueryChannels(
        FFilter Filter,
        const TArray<FSortOption>& SortOptions,
        const UObject* WorldContextObject,
        FLatentActionInfo LatentInfo,
        TArray<UChatChannel*>& OutChannels);

    UFUNCTION(BlueprintCallable, Category = "Stream Chat|Client", meta = (Latent, WorldContext = WorldContextObject, LatentInfo = LatentInfo))
    void WatchChannel(
        const FString& Type,
        const FString& Id,
        const TArray<FString>& Members,
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
    using TEventDelegateUObjectMethodPtr =
        typename TEventDelegate<TEvent>::template TUObjectMethodDelegate<UserClass>::FMethodPtr;
    template <class TEvent, class UserClass>
    using TEventDelegateSpMethodPtr =
        typename TEventDelegate<TEvent>::template TSPMethodDelegate<UserClass>::FMethodPtr;

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
    typename TEnableIf<TIsInvocable<FunctorType, const TEvent&, VarTypes...>::Value, FDelegateHandle>::Type On(
        FunctorType&& Functor,
        VarTypes... Vars);

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
typename TEnableIf<!TIsDerivedFrom<UserClass, UObject>::IsDerived, FDelegateHandle>::Type
UStreamChatClientComponent::On(UserClass* Obj, TEventDelegateSpMethodPtr<TEvent, UserClass> Method)
{
    const TEventDelegate<TEvent> Delegate = TEventDelegate<TEvent>::CreateSP(Obj, Method);
    return On<TEvent>(Delegate);
}

template <class TEvent, typename FunctorType, typename... VarTypes>
typename TEnableIf<TIsInvocable<FunctorType, const TEvent&, VarTypes...>::Value, FDelegateHandle>::Type
UStreamChatClientComponent::On(FunctorType&& Functor, VarTypes... Vars)
{
    const TEventDelegate<TEvent> Delegate =
        TEventDelegate<TEvent>::CreateLambda(Forward<FunctorType>(Functor), Vars...);
    return On<TEvent>(Delegate);
}

template <class TEvent>
bool UStreamChatClientComponent::Unsubscribe(const FDelegateHandle Handle) const
{
    return Socket->Events().Unsubscribe<TEvent>(Handle);
}
#pragma endregion Events
