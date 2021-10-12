#pragma once

#include "CoreMinimal.h"
#include "Socket/EventSubscription.h"

namespace Detail
{
template <class TEvent>
TEventSubscription<TEvent>& GetSubscription(TMap<FName, TUniquePtr<IEventSubscription>>& Subscriptions)
{
    const FName& EventType = TEvent::StaticType;
    const TUniquePtr<IEventSubscription>* Existing = Subscriptions.Find(EventType);
    const TUniquePtr<IEventSubscription>& Subscription =
        Existing ? *Existing : Subscriptions.Emplace(EventType, MakeUnique<TEventSubscription<TEvent>>());
    return StaticCast<TEventSubscription<TEvent>&>(*Subscription);
}

template <class TEvent>
FDelegateHandle SubscribeToEvent(
    TMap<FName, TUniquePtr<IEventSubscription>>& Subscriptions,
    TEventReceivedDelegate<TEvent> Callback)
{
    return Detail::GetSubscription<TEvent>(Subscriptions).Delegate.Add(MoveTemp(Callback));
}

template <class TEvent>
bool UnsubscribeFromEvent(
    TMap<FName, TUniquePtr<IEventSubscription>>& Subscriptions,
    FDelegateHandle DelegateHandle)
{
    return Detail::GetSubscription<TEvent>(Subscriptions).Delegate.Remove(DelegateHandle);
}

}    // namespace Detail
