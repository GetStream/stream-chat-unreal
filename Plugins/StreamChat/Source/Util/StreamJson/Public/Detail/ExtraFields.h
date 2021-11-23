// Copyright Stream.IO, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"

class FJsonObject;

namespace ExtraFields
{
template <typename T>
auto InvokeDeserializeExtraImpl(const FJsonObject& JsonObject, T& Self, T*)
    -> decltype(T::DeserializeExtra(JsonObject, Self), void())
{
    return T::DeserializeExtra(JsonObject, Self);
}

template <typename T>
void InvokeDeserializeExtraImpl(const FJsonObject&, T&, ...)
{
}

template <typename T>
void InvokeDeserializeExtra(const FJsonObject& JsonObject, T& Self)
{
    InvokeDeserializeExtraImpl(JsonObject, Self, static_cast<T*>(nullptr));
}

template <typename T>
auto InvokeSerializeExtraImpl(const T& Self, FJsonObject& JsonObject, T*)
    -> decltype(T::SerializeExtra(Self, JsonObject), void())
{
    return T::SerializeExtra(Self, JsonObject);
}

template <typename T>
void InvokeSerializeExtraImpl(const T&, FJsonObject&, ...)
{
}

template <typename T>
void InvokeSerializeExtra(const T& Self, FJsonObject& JsonObject)
{
    InvokeSerializeExtraImpl(Self, JsonObject, static_cast<T*>(nullptr));
}
}    // namespace ExtraFields
