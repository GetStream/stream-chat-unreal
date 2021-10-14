#pragma once

#include "CoreMinimal.h"
#include "Dom/JsonObject.h"
#include "StreamJson.h"

template <class TEvent>
using TEventReceivedMulticastDelegate = TMulticastDelegate<void(const TEvent& Event)>;
template <class TEvent>
using TEventReceivedDelegate = typename TEventReceivedMulticastDelegate<TEvent>::FDelegate;

class IEventSubscription
{
public:
    virtual ~IEventSubscription() = 0;
    virtual bool OnMessage(const TSharedRef<FJsonObject>&) = 0;
};

inline IEventSubscription::~IEventSubscription() = default;

template <class T>
class TEventSubscription final : public IEventSubscription
{
public:
    virtual bool OnMessage(const TSharedRef<FJsonObject>&) override;

    TEventReceivedMulticastDelegate<T> Delegate;
};

using FEventSubscriptionPtr = TSharedPtr<IEventSubscription>;

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
