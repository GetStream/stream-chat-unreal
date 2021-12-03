// Copyright 2021 Stream.IO, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Kismet/BlueprintFunctionLibrary.h"

#include "JwtBlueprintFunctionLibrary.generated.h"

/**
 *
 */
UCLASS()
class JWT_API UJwtBlueprintFunctionLibrary final : public UBlueprintFunctionLibrary
{
    GENERATED_BODY()

    /**
     * @brief Generate a JSON Web Token
     * @param ApiSecret Your API secret as listed on the Dashboard
     * @param UserId The user ID of the user you wish to generate a token for
     * @return A newly generated JWT
     */
    UFUNCTION(BlueprintPure, Category = "JWT")
    static FString GenerateJwt(const FString& ApiSecret, const FString& UserId);
};
