// Copyright 2021 Stream.IO, Inc. All Rights Reserved.

#pragma once

#include "Channel/Message.h"
#include "Common/Html/HtmlTextBlock.h"
#include "Components/Border.h"
#include "CoreMinimal.h"
#include "MessagePosition.h"
#include "MessageSide.h"
#include "StreamWidget.h"

#include "TextBubbleWidget.generated.h"

/**
 *
 */
UCLASS()
class STREAMCHATUI_API UTextBubbleWidget final : public UStreamWidget
{
    GENERATED_BODY()

public:
    UTextBubbleWidget();
    UFUNCTION(BlueprintCallable, Category = "Stream Chat")
    void Setup(const FMessage& InMessage, EMessageSide InSide, EMessagePosition InPosition);

protected:
    UPROPERTY(meta = (BindWidget))
    UHtmlTextBlock* TextBlock;
    UPROPERTY(meta = (BindWidget))
    UBorder* Border;

    UPROPERTY(EditAnywhere, Category = Bubble)
    UTexture2D* MaskOpeningTexture;
    UPROPERTY(EditAnywhere, Category = Bubble)
    UTexture2D* MaskEndTexture;
    UPROPERTY(EditAnywhere, Category = Bubble)
    UTexture2D* YouOpeningTexture;
    UPROPERTY(EditAnywhere, Category = Bubble)
    UTexture2D* YouEndTexture;
    UPROPERTY(EditAnywhere, Category = Emoji)
    FSlateFontInfo SingleEmojiFont;
    UPROPERTY(EditAnywhere, Category = Emoji)
    float SingleEmojiBorderStyle = 32.f;

    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Setup)
    FMessage Message;
    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Setup)
    EMessagePosition Position;
    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Setup)
    EMessageSide Side;

private:
    virtual void OnSetup() override;
    virtual void OnTheme() override;

    UTexture2D* GetBubbleTexture() const;
    const FLinearColor& GetBubbleColor() const;
    FText GetText() const;
    bool IsSingleEmoji() const;
};
