// Copyright 2026 Stream.IO, Inc. All Rights Reserved.

#pragma once

#include "Channel/Attachment.h"
#include "Components/Border.h"
#include "Components/Image.h"
#include "Components/PanelWidget.h"
#include "Components/SizeBox.h"
#include "Components/TextBlock.h"
#include "CoreMinimal.h"
#include "MessageSide.h"
#include "StreamWidget.h"

#include "AttachmentWidget.generated.h"

class UTexture2DDynamic;

/**
 * @brief Displays a single attachment of a message.
 *
 * Images are downloaded and shown inline. Everything else gets a row naming the file, because the
 * SDK has no way of opening arbitrary content that would work on every platform it supports.
 *
 * Unlike the other widgets here this one builds its own UMG tree in C++, so it needs no widget
 * blueprint. Point UMessageWidget::AttachmentWidgetClass at your own class to replace it, or give a
 * subclass a widget blueprint with a panel named ContentPanel and the content will be built into
 * that instead of into a generated root.
 * @ingroup StreamChatUi
 */
UCLASS()
class STREAMCHATUI_API UAttachmentWidget : public UStreamWidget
{
    GENERATED_BODY()

public:
    UFUNCTION(BlueprintCallable, Category = "Stream Chat")
    void Setup(const FAttachment& InAttachment, EMessageSide InSide);

protected:
    /// Holds the attachment content. Generated in C++ when no widget blueprint provides one.
    UPROPERTY(meta = (BindWidgetOptional))
    UPanelWidget* ContentPanel;

    /// How much of the width an inline image may take. Its height then follows from its aspect ratio.
    UPROPERTY(EditDefaultsOnly, Category = Defaults, meta = (ClampMin = "0.1", ClampMax = "1.0"))
    float ImageWidthFraction = 0.66f;

    /// Ceiling on the above, so an image does not run away with a wide desktop window
    UPROPERTY(EditDefaultsOnly, Category = Defaults)
    float MaxImageWidth = 320.f;

    /// How many times its own width an image may be tall before it is fitted by height instead
    UPROPERTY(EditDefaultsOnly, Category = Defaults, meta = (ClampMin = "1.0"))
    float MaxImageAspect = 2.f;

    /**
     * @brief Ask Stream's CDN for an image scaled to roughly the size it will be drawn at.
     *
     * A photo off a phone camera is around 25 megapixels, which is 93 MB once decoded, for something
     * that ends up a couple of hundred points wide. Turn this off to always fetch the original.
     */
    UPROPERTY(EditDefaultsOnly, Category = Defaults)
    bool bRequestResizedImages = true;

    UPROPERTY(EditDefaultsOnly, Category = Defaults)
    FMargin FileRowPadding = FMargin{12.f, 8.f};

    // Only the size is configurable: the typeface is taken from whatever font the text block already
    // carries, which keeps this working without a font asset of our own.
    UPROPERTY(EditDefaultsOnly, Category = Defaults)
    int32 TitleFontSize = 14;

    UPROPERTY(EditDefaultsOnly, Category = Defaults)
    int32 SubtitleFontSize = 12;

    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Setup)
    FAttachment Attachment;

    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Setup)
    EMessageSide Side = EMessageSide::You;

private:
    virtual void OnSetup() override;
    virtual void NativePreConstruct() override;

    /// True when the attachment is something we can fetch and draw ourselves
    bool ShouldRenderInline() const;
    void FetchRemoteImage();
    void OnImageDownloaded(UTexture2DDynamic* Texture);

    /// Width available to lay out in, in slate units. Falls back to the ceiling with no viewport.
    float GetAvailableWidth() const;
    /// Widest an image is allowed to be drawn
    float GetImageBudgetWidth() const;
    /**
     * The size to draw a source of the given pixel dimensions at.
     *
     * Fitted to the budget width with the height following the aspect ratio, and never enlarged past
     * the source's own size, so a small image is not blown up into a blurry one.
     */
    FVector2D GetImageDisplaySize(int32 SourceWidth, int32 SourceHeight) const;
    /// Slate units per pixel. 1 with no viewport to ask.
    float GetDpiScale() const;
    void ApplyImageSize(const FVector2D& Size);

    /// Filename or, failing that, something better than an empty row
    FString GetTitle() const;
    /// Type and size of the file, when the backend told us either
    FString GetSubtitle() const;

    UPanelWidget* GetOrCreateContentPanel();

    // Both the image and the file row are built for every attachment: an image that fails to
    // download falls back to the row, and there is no theme to style a late-built one with.
    UPROPERTY(Transient)
    UImage* Image;
    // The image is sized by this rather than by UImage::SetDesiredSizeOverride, which does nothing
    // unless the slate widget already exists and is not reapplied when it is rebuilt
    UPROPERTY(Transient)
    USizeBox* ImageSizer;
    UPROPERTY(Transient)
    UBorder* FileRow;
    UPROPERTY(Transient)
    UTextBlock* TitleTextBlock;
    UPROPERTY(Transient)
    UTextBlock* SubtitleTextBlock;

    bool bImageLoaded = false;
};
