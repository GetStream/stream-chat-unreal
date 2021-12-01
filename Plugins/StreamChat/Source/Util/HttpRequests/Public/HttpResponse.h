// Copyright 2021 Stream.IO, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Interfaces/IHttpRequest.h"
#include "StreamJson.h"

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

    UPROPERTY(BlueprintReadOnly, Category = "HTTP Response")
    int32 StatusCode;

    UPROPERTY(BlueprintReadOnly, Category = "HTTP Response")
    FString Text;
};

template <class T>
T FHttpResponse::Json() const
{
    return Json::Deserialize<T>(Text);
}
