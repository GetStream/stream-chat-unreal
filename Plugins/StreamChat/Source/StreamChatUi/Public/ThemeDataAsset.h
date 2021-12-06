// Copyright 2021 Stream.IO, Inc. All Rights Reserved.

#pragma once

#include "Components/Widget.h"
#include "CoreMinimal.h"
#include "Engine/DataAsset.h"

#include "ThemeDataAsset.generated.h"

/**
 *
 */
UCLASS()
class STREAMCHATUI_API UThemeDataAsset : public UDataAsset
{
    GENERATED_BODY()

public:
    static UThemeDataAsset* Get(const UWidget* Widget);
    /// The name of a user appears colored
    UPROPERTY(EditAnywhere, Category = Defaults)
    bool bColoredName;
};
