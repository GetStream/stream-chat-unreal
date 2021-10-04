#pragma once

#include "CoreMinimal.h"
#include "Detail/JsonDetail.h"

namespace Json
{
template <class T>
FString Serialize(const T& Struct)
{
	return Detail::Serialize(T::StaticStruct(), &Struct);
}

template <class T>
T Deserialize(const FString& Json)
{
	return Detail::Deserialize<T>(Json);
}
}	 // namespace Json
