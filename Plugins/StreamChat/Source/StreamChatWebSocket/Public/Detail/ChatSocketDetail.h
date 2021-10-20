#pragma once

#include "CoreMinimal.h"
#include "JsonEventSubscription.h"

namespace Detail
{
template <class TEvent>
TJsonEventSubscription<TEvent>& GetSubscription(TMap<FName, FEventSubscriptionPtr>& Subscriptions)
{
    const FName& EventType = TEvent::StaticType;
    const FEventSubscriptionPtr* Existing = Subscriptions.Find(EventType);
    const FEventSubscriptionPtr& Subscription =
        Existing ? *Existing : Subscriptions.Emplace(EventType, MakeShared<TJsonEventSubscription<TEvent>>());
    return StaticCast<TJsonEventSubscription<TEvent>&>(*Subscription);
}

template <class TEvent>
FDelegateHandle SubscribeToEvent(
    TMap<FName, FEventSubscriptionPtr>& Subscriptions,
    TEventDelegate<TEvent> Callback)
{
    return Detail::GetSubscription<TEvent>(Subscriptions).Delegate.Add(MoveTemp(Callback));
}

template <class TEvent, class UserClass>
FDelegateHandle SubscribeToUObjectEvent(
    TMap<FName, FEventSubscriptionPtr>& Subscriptions,
    UserClass* InObj,
    TEventDelegateUObjectMethodPtr<TEvent, UserClass> InMethod)
{
    const TEventDelegate<TEvent> Delegate = TEventDelegate<TEvent>::CreateUObject(InObj, InMethod);
    return Detail::SubscribeToEvent<TEvent>(Subscriptions, Delegate);
}

template <class TEvent, class UserClass>
FDelegateHandle SubscribeToSpEvent(
    TMap<FName, FEventSubscriptionPtr>& Subscriptions,
    UserClass* InObj,
    TEventDelegateSpMethodPtr<TEvent, UserClass> InMethod)
{
    const TEventDelegate<TEvent> Delegate = TEventDelegate<TEvent>::CreateSP(InObj, InMethod);
    return Detail::SubscribeToEvent<TEvent>(Subscriptions, Delegate);
}

template <class TEvent, typename FunctorType, typename... VarTypes>
FDelegateHandle
SubscribeToLambdaEvent(TMap<FName, FEventSubscriptionPtr>& Subscriptions, FunctorType&& InFunctor, VarTypes... Vars)
{
    const TEventDelegate<TEvent> Delegate =
        TEventDelegate<TEvent>::CreateLambda(Forward<FunctorType>(InFunctor), Vars...);
    return Detail::SubscribeToEvent<TEvent>(Subscriptions, Delegate);
}

template <class TEvent>
bool UnsubscribeFromEvent(TMap<FName, FEventSubscriptionPtr>& Subscriptions, FDelegateHandle DelegateHandle)
{
    return Detail::GetSubscription<TEvent>(Subscriptions).Delegate.Remove(DelegateHandle);
}

}    // namespace Detail
