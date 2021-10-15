#pragma once

#include "CoreMinimal.h"
#include "Detail/ChatSocketDetail.h"
#include "Detail/EventSubscription.h"

class FChatSocketEvents final
{
public:
    /// Subscribe to a WebSocket event using your own delegate object
    template <class TEvent>
    FDelegateHandle Subscribe(TEventReceivedDelegate<TEvent> Callback);

    /// Subscribe to a WebSocket event using a UObject-based member function delegate.
    template <class TEvent, class UserClass>
    FORCEINLINE FDelegateHandle
    SubscribeUObject(UserClass* Obj, TEventReceivedDelegateUObjectMethodPtr<TEvent, UserClass> Method);

    /// Subscribe to a WebSocket event using a shared pointer-based (fast, not thread-safe) member function delegate.
    template <class TEvent, class UserClass>
    FORCEINLINE FDelegateHandle
    SubscribeSp(UserClass* Obj, TEventReceivedDelegateSpMethodPtr<TEvent, UserClass> Method);

    template <class TEvent>
    bool Unsubscribe(FDelegateHandle);

private:
    friend class FChatSocket;
    void ProcessEvent(const FString& JsonString);

    TMap<FName, FEventSubscriptionPtr> Subscriptions;
};

template <class TEvent>
FDelegateHandle FChatSocketEvents::Subscribe(TEventReceivedDelegate<TEvent> Callback)
{
    return Detail::SubscribeToEvent<TEvent>(Subscriptions, Callback);
}

template <class TEvent, class UserClass>
FDelegateHandle FChatSocketEvents::SubscribeUObject(
    UserClass* Obj,
    TEventReceivedDelegateUObjectMethodPtr<TEvent, UserClass> Method)
{
    return Detail::SubscribeToUObjectEvent<TEvent, UserClass>(Subscriptions, Obj, Method);
}

template <class TEvent, class UserClass>
FDelegateHandle FChatSocketEvents::SubscribeSp(
    UserClass* Obj,
    TEventReceivedDelegateSpMethodPtr<TEvent, UserClass> Method)
{
    return Detail::SubscribeToSpEvent<TEvent, UserClass>(Subscriptions, Obj, Method);
}

template <class TEvent>
bool FChatSocketEvents::Unsubscribe(const FDelegateHandle DelegateHandle)
{
    return Detail::UnsubscribeFromEvent<TEvent>(Subscriptions, DelegateHandle);
}
