// Copyright Stream.IO, Inc. All Rights Reserved.

#include "ImageDownloadSubsystem.h"

#include "Engine/Texture2DDynamic.h"
#include "HttpModule.h"
#include "IImageWrapper.h"
#include "IImageWrapperModule.h"
#include "Interfaces/IHttpRequest.h"
#include "Interfaces/IHttpResponse.h"
#include "Modules/ModuleManager.h"
#include "RHIResources.h"

namespace
{
void RenderRawData(FTexture2DDynamicResource* TextureResource, const TSharedRef<TArray64<uint8>>& RawData)
{
    ENQUEUE_RENDER_COMMAND(FWriteRawDataToTexture)
    (
        [TextureResource, RawData](FRHICommandListImmediate&)
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
            uint8* DestData =
                static_cast<uint8*>(RHILockTexture2D(TextureRHI, 0, RLM_WriteOnly, DestStride, false, false));

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
        });
}

UTexture2DDynamic* TryCreateTexture(const FHttpResponsePtr HttpResponse, const EImageFormat Format)
{
    IImageWrapperModule& ImageWrapperModule =
        FModuleManager::LoadModuleChecked<IImageWrapperModule>(FName("ImageWrapper"));
    const TSharedPtr<IImageWrapper> ImageWrapper = ImageWrapperModule.CreateImageWrapper(Format);

    if (!ImageWrapper.IsValid())
    {
        return nullptr;
    }

    if (!ImageWrapper->SetCompressed(HttpResponse->GetContent().GetData(), HttpResponse->GetContentLength()))
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

    RenderRawData(TextureResource, RawData);
    return Texture;
}
}    // namespace

bool UImageDownloadSubsystem::ShouldCreateSubsystem(UObject* Outer) const
{
    return !CastChecked<UGameInstance>(Outer)->IsDedicatedServerInstance();
}

void UImageDownloadSubsystem::DownloadImage(const FString& Url, TFunction<void(UTexture2DDynamic*)> Callback)
{
    if (UTexture2DDynamic** Cached = MemoryCache.Find(Url))
    {
        Callback(*Cached);
        return;
    }

    // Create the Http request and add to pending request list
    const TSharedRef<IHttpRequest, ESPMode::ThreadSafe> HttpRequest = FHttpModule::Get().CreateRequest();

    const auto OnRequest = [WeakThis = TWeakObjectPtr<UImageDownloadSubsystem>(this), Url, Callback](
                               FHttpRequestPtr, const FHttpResponsePtr HttpResponse, const bool bSucceeded)
    {
        if (!bSucceeded || !HttpResponse.IsValid() || HttpResponse->GetContentLength() <= 0)
        {
            Callback(nullptr);
            return;
        }

        for (const EImageFormat Format : {EImageFormat::PNG, EImageFormat::JPEG, EImageFormat::BMP})
        {
            UTexture2DDynamic* Texture = TryCreateTexture(HttpResponse, Format);
            if (!Texture)
            {
                continue;
            }

            if (WeakThis.IsValid())
            {
                WeakThis->Cache(Url, Texture);
            }

            Callback(Texture);
            return;
        }

        Callback(nullptr);
    };

    HttpRequest->OnProcessRequestComplete().BindLambda(OnRequest);
    HttpRequest->SetURL(Url);
    HttpRequest->SetVerb(TEXT("GET"));
    HttpRequest->ProcessRequest();
}

void UImageDownloadSubsystem::Cache(const FString& Url, UTexture2DDynamic* Texture)
{
    MemoryCache.Add(Url, Texture);
}
