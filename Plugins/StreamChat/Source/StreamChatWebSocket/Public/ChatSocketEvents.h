#pragma once

#include "CoreMinimal.h"
#include "Detail/ChatSocketDetail.h"
#include "Detail/EventSubscription.h"

class FChatSocketEvents final
{
public:
    template <class TEvent>
    FDelegateHandle Subscribe(TEventReceivedDelegate<TEvent> Callback);
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

template <class TEvent>
bool FChatSocketEvents::Unsubscribe(const FDelegateHandle DelegateHandle)
{
    return Detail::UnsubscribeFromEvent<TEvent>(Subscriptions, DelegateHandle);
}
