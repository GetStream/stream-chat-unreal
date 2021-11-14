// Copyright Stream.IO, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Subsystems/GameInstanceSubsystem.h"

#include "ImageDownloadSubsystem.generated.h"

class UTexture2DDynamic;
enum class EImageFormat : int8;

/**
 *
 */
UCLASS()
class STREAMCHAT_API UImageDownloadSubsystem final : public UGameInstanceSubsystem
{
    GENERATED_BODY()

public:
    virtual bool ShouldCreateSubsystem(UObject* Outer) const override;

    void DownloadImage(const FString& Url, TFunction<void(UTexture2DDynamic*)>);

private:
    /// Add image with given url to in-memory cache
    void Cache(const FString& Url, UTexture2DDynamic* Texture);

    UPROPERTY(Transient)
    TMap<FString, UTexture2DDynamic*> MemoryCache;
};
