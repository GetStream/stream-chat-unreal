// Copyright Stream.IO, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Detail/ChatSocketDetail.h"
#include "Detail/JsonEventSubscription.h"

/**
 * @brief Encapsulates subscribing and unsubscribing to and from Stream Chat WebSocket events
 * @ingroup StreamChatWebSocket
 */
class FChatSocketEvents final
{
public:
    /// Subscribe to a WebSocket event using your own delegate object
    template <class TEvent>
    FDelegateHandle Subscribe(TEventDelegate<TEvent> Callback);

    /// Subscribe to a WebSocket event using a UObject-based member function delegate.
    template <class TEvent, class UserClass>
    FDelegateHandle SubscribeUObject(UserClass* Obj, TEventDelegateUObjectMethodPtr<TEvent, UserClass> Method);

    /// Subscribe to a WebSocket event using a shared pointer-based (fast, not thread-safe) member function delegate.
    template <class TEvent, class UserClass>
    FDelegateHandle SubscribeSp(UserClass* Obj, TEventDelegateSpMethodPtr<TEvent, UserClass> Method);

    /// Subscribe to a WebSocket event using a UObject-based member function delegate.
    template <class TEvent, typename FunctorType, typename... VarTypes>
    FDelegateHandle SubscribeLambda(FunctorType&& InFunctor, VarTypes... Vars);

    template <class TEvent>
    bool Unsubscribe(FDelegateHandle);
    STREAMCHATWEBSOCKET_API int32 UnsubscribeAll(UObject* Object);

private:
    friend class FChatSocket;

    template <class TEvent>
    void Broadcast(const TEvent&);

    void ProcessEvent(const FName& Type, const TSharedRef<FJsonObject>);

    TMap<FName, FEventSubscriptionPtr> Subscriptions;
};

template <class TEvent>
FDelegateHandle FChatSocketEvents::Subscribe(TEventDelegate<TEvent> Callback)
{
    return Detail::SubscribeToEvent<TEvent>(Subscriptions, Callback);
}

template <class TEvent, class UserClass>
FDelegateHandle FChatSocketEvents::SubscribeUObject(UserClass* Obj, TEventDelegateUObjectMethodPtr<TEvent, UserClass> Method)
{
    return Detail::SubscribeToUObjectEvent<TEvent, UserClass>(Subscriptions, Obj, Method);
}

template <class TEvent, class UserClass>
FDelegateHandle FChatSocketEvents::SubscribeSp(UserClass* Obj, TEventDelegateSpMethodPtr<TEvent, UserClass> Method)
{
    return Detail::SubscribeToSpEvent<TEvent, UserClass>(Subscriptions, Obj, Method);
}

template <class TEvent, typename FunctorType, typename... VarTypes>
FDelegateHandle FChatSocketEvents::SubscribeLambda(FunctorType&& InFunctor, VarTypes... Vars)
{
    return Detail::SubscribeToLambdaEvent<TEvent, FunctorType, VarTypes...>(Subscriptions, Forward<FunctorType>(InFunctor), Vars...);
}

template <class TEvent>
bool FChatSocketEvents::Unsubscribe(const FDelegateHandle DelegateHandle)
{
    return Detail::UnsubscribeFromEvent<TEvent>(Subscriptions, DelegateHandle);
}

template <class TEvent>
void FChatSocketEvents::Broadcast(const TEvent& Event)
{
    const FName EventType{TEvent::StaticType};
    const FEventSubscriptionPtr* SubPtr = Subscriptions.Find(EventType);
    if (!SubPtr)
    {
        return;
    }

    TJsonEventSubscription<TEvent>& Subscription = StaticCast<TJsonEventSubscription<TEvent>&>(**SubPtr);
    Subscription.Delegate.Broadcast(Event);
}
