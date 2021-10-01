#pragma once

#include "CoreMinimal.h"
#include "Interfaces/IHttpRequest.h"
#include "JsonObjectConverter.h"

#include "HttpResponse.generated.h"

USTRUCT(BlueprintType)
struct FHttpResponse
{
	GENERATED_BODY()

	FHttpResponse() = default;

	explicit FHttpResponse(const FHttpResponsePtr Response);

	/**
	 * Deserializes the JSON content of this response to a UStruct
	 * @tparam T The type of UStruct to attempt to deserialize to
	 * @return A new UStruct filled from the JSON content
	 */
	template <class T>
	T Json() const;

	UPROPERTY(BlueprintReadOnly)
	int32 StatusCode;

	UPROPERTY(BlueprintReadOnly)
	FString Text;
};

template <class T>
T FHttpResponse::Json() const
{
	T OutData;
	FJsonObjectConverter::JsonObjectStringToUStruct<T>(Text, &OutData);
	return OutData;
}
