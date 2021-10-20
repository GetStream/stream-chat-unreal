#pragma once

#include "CoreMinimal.h"
#include "EventSubscription.h"

namespace Detail
{
template <class TEvent>
TEventSubscription<TEvent>& GetSubscription(TMap<FName, FEventSubscriptionPtr>& Subscriptions)
{
    const FName& EventType = TEvent::StaticType;
    const FEventSubscriptionPtr* Existing = Subscriptions.Find(EventType);
    const FEventSubscriptionPtr& Subscription =
        Existing ? *Existing : Subscriptions.Emplace(EventType, MakeShared<TEventSubscription<TEvent>>());
    return StaticCast<TEventSubscription<TEvent>&>(*Subscription);
}

template <class TEvent>
FDelegateHandle SubscribeToEvent(
    TMap<FName, FEventSubscriptionPtr>& Subscriptions,
    TEventReceivedDelegate<TEvent> Callback)
{
    return Detail::GetSubscription<TEvent>(Subscriptions).Delegate.Add(MoveTemp(Callback));
}

template <class TEvent, class UserClass>
FDelegateHandle SubscribeToUObjectEvent(
    TMap<FName, FEventSubscriptionPtr>& Subscriptions,
    UserClass* InObj,
    TEventReceivedDelegateUObjectMethodPtr<TEvent, UserClass> InMethod)
{
    const TEventReceivedDelegate<TEvent> Delegate = TEventReceivedDelegate<TEvent>::CreateUObject(InObj, InMethod);
    return Detail::SubscribeToEvent<TEvent>(Subscriptions, Delegate);
}

template <class TEvent, class UserClass>
FDelegateHandle SubscribeToSpEvent(
    TMap<FName, FEventSubscriptionPtr>& Subscriptions,
    UserClass* InObj,
    TEventReceivedDelegateSpMethodPtr<TEvent, UserClass> InMethod)
{
    const TEventReceivedDelegate<TEvent> Delegate = TEventReceivedDelegate<TEvent>::CreateSP(InObj, InMethod);
    return Detail::SubscribeToEvent<TEvent>(Subscriptions, Delegate);
}

template <class TEvent, typename FunctorType, typename... VarTypes>
FDelegateHandle
SubscribeToLambdaEvent(TMap<FName, FEventSubscriptionPtr>& Subscriptions, FunctorType&& InFunctor, VarTypes... Vars)
{
    const TEventReceivedDelegate<TEvent> Delegate =
        TEventReceivedDelegate<TEvent>::CreateLambda(Forward<FunctorType>(InFunctor), Vars...);
    return Detail::SubscribeToEvent<TEvent>(Subscriptions, Delegate);
}

template <class TEvent>
bool UnsubscribeFromEvent(TMap<FName, FEventSubscriptionPtr>& Subscriptions, FDelegateHandle DelegateHandle)
{
    return Detail::GetSubscription<TEvent>(Subscriptions).Delegate.Remove(DelegateHandle);
}

}    // namespace Detail
