#pragma once

#include "CoreMinimal.h"
#include "StreamJson.h"

template <class TEvent>
using TEventReceivedMulticastDelegate = TMulticastDelegate<void(const TEvent& Event)>;
template <class TEvent>
using TEventReceivedDelegate = typename TEventReceivedMulticastDelegate<TEvent>::FDelegate;

class IEventSubscription
{
public:
    virtual ~IEventSubscription() = default;
    virtual void OnMessage(const FString& Message) = 0;
};

template <class T>
class TEventSubscription final : public IEventSubscription
{
public:
    virtual void OnMessage(const FString& Message) override;

    TEventReceivedMulticastDelegate<T> Delegate;
};

template <class T>
void TEventSubscription<T>::OnMessage(const FString& Message)
{
    Delegate.Broadcast(Json::Deserialize<T>(Message));
}
