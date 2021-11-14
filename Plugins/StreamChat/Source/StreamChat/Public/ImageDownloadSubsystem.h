// Copyright Stream.IO, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Subsystems/GameInstanceSubsystem.h"

#include "ImageDownloadSubsystem.generated.h"

/**
 *
 */
UCLASS()
class STREAMCHAT_API UImageDownloadSubsystem final : public UGameInstanceSubsystem
{
    GENERATED_BODY()

public:
    virtual bool ShouldCreateSubsystem(UObject* Outer) const override;

    static void DownloadImage(const FString& Url, TFunction<void(UTexture2DDynamic*)>);
};
