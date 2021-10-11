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
    virtual bool OnMessage(const TSharedRef<FJsonObject>&) = 0;
};

template <class T>
class TEventSubscription final : public IEventSubscription
{
public:
    virtual bool OnMessage(const TSharedRef<FJsonObject>&) override;

    TEventReceivedMulticastDelegate<T> Delegate;
};

template <class T>
bool TEventSubscription<T>::OnMessage(const TSharedRef<FJsonObject>& JsonObject)
{
    T OutStruct;
    if (!JsonObjectDeserialization::JsonObjectToUStruct<T>(JsonObject, &OutStruct))
    {
        return false;
    }

    Delegate.Broadcast(OutStruct);
    return true;
}
