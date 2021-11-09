#include "WidgetUtil.h"

#include "Engine/Texture2DDynamic.h"
#include "HttpModule.h"
#include "IImageWrapper.h"
#include "IImageWrapperModule.h"
#include "Interfaces/IHttpRequest.h"
#include "Interfaces/IHttpResponse.h"
#include "Modules/ModuleManager.h"

UWidget* WidgetUtil::GetTypedParentWidget(const UWidget* Widget, const TSubclassOf<UWidget> Type)

{
    UWidget* NextOuter = const_cast<UWidget*>(Widget);
    while (NextOuter)
    {
        if (NextOuter->IsA(Type))
        {
            return NextOuter;
        }

        if (NextOuter->Slot)
        {
            NextOuter = NextOuter->Slot->Parent;
            continue;
        }

        if (const UObject* WidgetTree = NextOuter->GetOuter())
        {
            if (UUserWidget* UserWidget = Cast<UUserWidget>(WidgetTree->GetOuter()))
            {
                NextOuter = UserWidget;
                continue;
            }
        }
        break;
    }
    return nullptr;
}

void WidgetUtil::DownloadImage(const FString& Url, TFunction<void(UTexture2DDynamic*)> Callback)
{
    // Create the Http request and add to pending request list
    const TSharedRef<IHttpRequest, ESPMode::ThreadSafe> HttpRequest = FHttpModule::Get().CreateRequest();

    const auto OnRequest = [Callback](FHttpRequestPtr, const FHttpResponsePtr HttpResponse, const bool bSucceeded)
    {
        if (!bSucceeded || !HttpResponse.IsValid() || HttpResponse->GetContentLength() <= 0)
        {
            Callback(nullptr);
            return;
        }

        IImageWrapperModule& ImageWrapperModule =
            FModuleManager::LoadModuleChecked<IImageWrapperModule>(FName("ImageWrapper"));
        TSharedPtr<IImageWrapper> ImageWrappers[3] = {
            ImageWrapperModule.CreateImageWrapper(EImageFormat::PNG),
            ImageWrapperModule.CreateImageWrapper(EImageFormat::JPEG),
            ImageWrapperModule.CreateImageWrapper(EImageFormat::BMP),
        };

        for (auto ImageWrapper : ImageWrappers)
        {
            if (!ImageWrapper.IsValid())
            {
                continue;
            }
            if (!ImageWrapper->SetCompressed(HttpResponse->GetContent().GetData(), HttpResponse->GetContentLength()))
            {
                continue;
            }

            TSharedRef<TArray64<uint8>> RawData = MakeShared<TArray64<uint8>>();
            constexpr ERGBFormat InFormat = ERGBFormat::BGRA;
            if (!ImageWrapper->GetRaw(InFormat, 8, *RawData))
            {
                continue;
            }
            UTexture2DDynamic* Texture = UTexture2DDynamic::Create(ImageWrapper->GetWidth(), ImageWrapper->GetHeight());
            if (!Texture)
            {
                continue;
            }
            Texture->SRGB = true;
            Texture->UpdateResource();

            FTexture2DDynamicResource* TextureResource = static_cast<FTexture2DDynamicResource*>(Texture->Resource);
            if (!TextureResource)
            {
                continue;
            }
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

                    for (int32 y = 0; y < Height; y++)
                    {
                        const int64 InvY = static_cast<int64>(Height) - 1 - y;
                        uint8* DestPtr = &DestData[InvY * DestStride];

                        const FColor* SrcPtr = &reinterpret_cast<FColor*>(RawData->GetData())[InvY * Width];
                        for (int32 x = 0; x < Width; x++)
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

uint32 WidgetUtil::HashStringWithMax(const FString& String, const uint32 Max)
{
    const uint32 Hash = GetTypeHash(String);
    const uint32 Reduced = Hash & (Max - 1);
    check(Reduced < Max);
    return Reduced;
}
