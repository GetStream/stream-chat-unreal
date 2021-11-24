// Copyright Stream.IO, Inc. All Rights Reserved.

#include "ImageDownloadSubsystem.h"

#include "Engine/Texture2DDynamic.h"
#include "HAL/FileManager.h"
#include "HttpModule.h"
#include "IImageWrapper.h"
#include "IImageWrapperModule.h"
#include "Interfaces/IHttpRequest.h"
#include "Interfaces/IHttpResponse.h"
#include "Modules/ModuleManager.h"
#include "RHIResources.h"

namespace
{
void RenderRawData_RenderThread(FTexture2DDynamicResource* TextureResource, const TSharedRef<TArray64<uint8>>& RawData)
{
    check(IsInRenderingThread());

    if (!TextureResource)
    {
        return;
    }

    FRHITexture2D* TextureRHI = TextureResource->GetTexture2DRHI();

    const int32 Width = TextureRHI->GetSizeX();
    const int32 Height = TextureRHI->GetSizeY();

    uint32 DestStride = 0;
    uint8* DestData = static_cast<uint8*>(RHILockTexture2D(TextureRHI, 0, RLM_WriteOnly, DestStride, false, false));

    for (int32 Y = 0; Y < Height; Y++)
    {
        const int64 InvY = static_cast<int64>(Height) - 1 - Y;
        uint8* DestPtr = &DestData[InvY * DestStride];

        const FColor* SrcPtr = &reinterpret_cast<FColor*>(RawData->GetData())[InvY * Width];
        for (int32 X = 0; X < Width; X++)
        {
            *DestPtr++ = SrcPtr->B;
            *DestPtr++ = SrcPtr->G;
            *DestPtr++ = SrcPtr->R;
            *DestPtr++ = SrcPtr->A;
            SrcPtr++;
        }
    }

    RHIUnlockTexture2D(TextureRHI, 0, false, false);
}

UTexture2DDynamic* TryCreateTexture(const void* Data, const int64 Size, const EImageFormat Format)
{
    IImageWrapperModule& ImageWrapperModule = FModuleManager::LoadModuleChecked<IImageWrapperModule>(FName("ImageWrapper"));
    const TSharedPtr<IImageWrapper> ImageWrapper = ImageWrapperModule.CreateImageWrapper(Format);

    if (!ImageWrapper.IsValid())
    {
        return nullptr;
    }

    if (!ImageWrapper->SetCompressed(Data, Size))
    {
        return nullptr;
    }

    const TSharedRef<TArray64<uint8>> RawData = MakeShared<TArray64<uint8>>();
    if (!ImageWrapper->GetRaw(ERGBFormat::BGRA, 8, *RawData))
    {
        return nullptr;
    }

    UTexture2DDynamic* Texture = UTexture2DDynamic::Create(ImageWrapper->GetWidth(), ImageWrapper->GetHeight());
    if (!Texture)
    {
        return nullptr;
    }

    Texture->SRGB = true;
    Texture->UpdateResource();

    FTexture2DDynamicResource* TextureResource = static_cast<FTexture2DDynamicResource*>(Texture->Resource);
    if (!TextureResource)
    {
        return nullptr;
    }

    ENQUEUE_RENDER_COMMAND(FWriteRawDataToTexture)
    ([TextureResource, RawData](FRHICommandListImmediate&) { RenderRawData_RenderThread(TextureResource, RawData); });

    return Texture;
}

UTexture2DDynamic* TryCreateTexture(const void* Data, const int64 Size)
{
    for (const EImageFormat Format : {EImageFormat::PNG, EImageFormat::JPEG, EImageFormat::BMP})
    {
        if (UTexture2DDynamic* Texture = TryCreateTexture(Data, Size, Format))
        {
            return Texture;
        }
    }
    return nullptr;
}

}    // namespace

void UImageDownloadSubsystem::Initialize(FSubsystemCollectionBase& Collection)
{
    if (FPaths::HasProjectPersistentDownloadDir())
    {
        CacheFolder = FPaths::ProjectPersistentDownloadDir() / TEXT("ImgCache/");

        // make sure the cache folder exists
        if (!IFileManager::Get().MakeDirectory(*CacheFolder, true))
        {
            UE_LOG(LogTemp, Error, TEXT("Unable to create cache folder at '%s'"), *CacheFolder);
        }
    }

    Super::Initialize(Collection);
}

bool UImageDownloadSubsystem::ShouldCreateSubsystem(UObject* Outer) const
{
    return !CastChecked<UGameInstance>(Outer)->IsDedicatedServerInstance();
}

void UImageDownloadSubsystem::DownloadImage(const FString& Url, TFunction<void(UTexture2DDynamic*)> Callback)
{
    if (UTexture2DDynamic* Cached = QueryMemoryCache(Url))
    {
        UE_LOG(LogTemp, Verbose, TEXT("Serving image from in-memory cache [url=%s]"), *Url);
        Callback(Cached);
        return;
    }

    if (UTexture2DDynamic* Cached = QueryDiskCache(Url))
    {
        UE_LOG(LogTemp, Verbose, TEXT("Serving image from on-disk cache [url=%s]"), *Url);
        Callback(Cached);
        return;
    }

    // Create the Http request and add to pending request list
    const TSharedRef<IHttpRequest, ESPMode::ThreadSafe> HttpRequest = FHttpModule::Get().CreateRequest();

    const auto OnRequest =
        [WeakThis = TWeakObjectPtr<UImageDownloadSubsystem>(this), Url, Callback](FHttpRequestPtr, const FHttpResponsePtr HttpResponse, const bool bSucceeded)
    {
        if (!bSucceeded || !HttpResponse.IsValid() || HttpResponse->GetContentLength() <= 0)
        {
            Callback(nullptr);
            return;
        }

        UTexture2DDynamic* Texture = TryCreateTexture(HttpResponse->GetContent().GetData(), HttpResponse->GetContentLength());
        if (!Texture)
        {
            Callback(nullptr);
            return;
        }

        if (WeakThis.IsValid())
        {
            WeakThis->CacheToMemory(Url, Texture);
            WeakThis->CacheToDisk(Url, HttpResponse);
        }

        Callback(Texture);
    };

    HttpRequest->OnProcessRequestComplete().BindLambda(OnRequest);
    HttpRequest->SetURL(Url);
    HttpRequest->SetVerb(TEXT("GET"));
    HttpRequest->ProcessRequest();
}

void UImageDownloadSubsystem::CacheToMemory(const FString& Url, UTexture2DDynamic* Texture)
{
    MemoryCache.Add(Url, Texture);
}

UTexture2DDynamic* UImageDownloadSubsystem::QueryMemoryCache(const FString& Url) const
{
    if (UTexture2DDynamic* const* Cached = MemoryCache.Find(Url))
    {
        return *Cached;
    }
    return nullptr;
}

void UImageDownloadSubsystem::CacheToDisk(const FString& Url, const FHttpResponsePtr Response) const
{
    if (!FPaths::HasProjectPersistentDownloadDir())
    {
        return;
    }

    const FString AbsolutePath = GetDiskPathForUrl(Url);

    const TUniquePtr<FArchive> FileWriter(IFileManager::Get().CreateFileWriter(*AbsolutePath));
    if (!FileWriter)
    {
        return;
    }

    void* Data = const_cast<void*>(static_cast<const void*>(Response->GetContent().GetData()));
    FileWriter->Serialize(Data, Response->GetContentLength());
    FileWriter->Close();
}

UTexture2DDynamic* UImageDownloadSubsystem::QueryDiskCache(const FString& Url)
{
    if (!FPaths::HasProjectPersistentDownloadDir())
    {
        return nullptr;
    }

    const FString AbsolutePath = GetDiskPathForUrl(Url);
    IFileManager& FileManager = IFileManager::Get();
    if (!FileManager.FileExists(*AbsolutePath))
    {
        return nullptr;
    }

    const TUniquePtr<FArchive> FileReader(FileManager.CreateFileReader(*AbsolutePath));
    if (!FileReader)
    {
        return nullptr;
    }

    const int64 FileSize = FileReader->TotalSize();
    uint8* NewBuffer = static_cast<uint8*>(FMemory::Malloc(FileSize));
    FileReader->Serialize(NewBuffer, FileSize);
    FileReader->Close();

    UTexture2DDynamic* Texture = TryCreateTexture(NewBuffer, FileSize);
    if (Texture)
    {
        CacheToMemory(Url, Texture);
    }

    FMemory::Free(NewBuffer);
    return Texture;
}

FString UImageDownloadSubsystem::GetDiskPathForUrl(const FString& Url) const
{
    const uint32 Hash = GetTypeHash(Url);
    const FString HashAsString = FString::Printf(TEXT("%08x"), Hash);

    const FString AbsolutePath = FPaths::ConvertRelativePathToFull(CacheFolder / HashAsString);
    return AbsolutePath;
}
