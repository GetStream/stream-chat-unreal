// Copyright 2021 Stream.IO, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Dom/JsonObject.h"
#include "StreamJson.h"

template <class TEvent>
using TEventMulticastDelegate = TMulticastDelegate<void(const TEvent& Event)>;
template <class TEvent>
using TEventDelegate = typename TEventMulticastDelegate<TEvent>::FDelegate;
template <class TEvent, class UserClass>
using TEventDelegateUObjectMethodPtr = typename TEventDelegate<TEvent>::template TUObjectMethodDelegate<UserClass>::FMethodPtr;
template <class TEvent, class UserClass>
using TEventDelegateSpMethodPtr = typename TEventDelegate<TEvent>::template TSPMethodDelegate<UserClass>::FMethodPtr;

class IJsonEventSubscription
{
public:
    virtual ~IJsonEventSubscription() = 0;
    virtual bool OnMessage(const TSharedRef<FJsonObject>&) = 0;
    virtual int32 Unsubscribe(UObject* Object) = 0;
};

inline IJsonEventSubscription::~IJsonEventSubscription() = default;

template <class T>
class TJsonEventSubscription final : public IJsonEventSubscription
{
public:
    virtual bool OnMessage(const TSharedRef<FJsonObject>&) override;
    virtual int32 Unsubscribe(UObject* Object) override;

    TEventMulticastDelegate<T> Delegate;
};

using FEventSubscriptionPtr = TSharedPtr<IJsonEventSubscription>;

template <class T>
bool TJsonEventSubscription<T>::OnMessage(const TSharedRef<FJsonObject>& JsonObject)
{
    T OutStruct;
    if (!JsonObjectDeserialization::JsonObjectToUStruct<T>(JsonObject, &OutStruct))
    {
        return false;
    }
    check(OutStruct.Type == T::StaticType);

    Delegate.Broadcast(OutStruct);
    return true;
}

template <class T>
int32 TJsonEventSubscription<T>::Unsubscribe(UObject* Object)
{
    return Delegate.RemoveAll(Object);
}
