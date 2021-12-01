// Copyright 2021 Stream.IO, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Subsystems/GameInstanceSubsystem.h"

#include "ImageDownloadSubsystem.generated.h"

class UTexture2DDynamic;
using FHttpResponsePtr = TSharedPtr<class IHttpResponse, ESPMode::ThreadSafe>;
enum class EImageFormat : int8;

/**
 *
 */
UCLASS()
class STREAMCHATUI_API UImageDownloadSubsystem final : public UGameInstanceSubsystem
{
    GENERATED_BODY()

public:
    virtual void Initialize(FSubsystemCollectionBase& Collection) override;
    virtual bool ShouldCreateSubsystem(UObject* Outer) const override;

    void DownloadImage(const FString& Url, TFunction<void(UTexture2DDynamic*)>);

private:
    /// Add image with given url to in-memory cache
    void CacheToMemory(const FString& Url, UTexture2DDynamic* Texture);
    UTexture2DDynamic* QueryMemoryCache(const FString& Url) const;

    /// Add image with given url to on-disk cache
    void CacheToDisk(const FString& Url, const FHttpResponsePtr) const;
    UTexture2DDynamic* QueryDiskCache(const FString& Url);

    FString GetDiskPathForUrl(const FString& Url) const;

    UPROPERTY(Transient)
    TMap<FString, UTexture2DDynamic*> MemoryCache;

    // folders to save image files into on disk
    FString CacheFolder;
};
