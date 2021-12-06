// Copyright 2021 Stream.IO, Inc. All Rights Reserved.

#pragma once

#include "BubbleStackPosition.h"
#include "Channel/Message.h"
#include "Components/Border.h"
#include "Components/TextBlock.h"
#include "CoreMinimal.h"
#include "MessageSide.h"
#include "StreamUserWidget.h"

#include "TextBubbleWidget.generated.h"

/**
 *
 */
UCLASS()
class STREAMCHATUI_API UTextBubbleWidget final : public UStreamUserWidget
{
    GENERATED_BODY()

public:
    UFUNCTION(BlueprintCallable, Category = "Stream Chat")
    void Setup(const FMessage& InMessage, EMessageSide InSide, EBubbleStackPosition InPosition);

protected:
    UPROPERTY(meta = (BindWidget))
    UTextBlock* TextBlock;
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

private:
    virtual void OnSetup() override;
    virtual bool WantsTheme() override
    {
        return true;
    }
    virtual void OnTheme(UThemeDataAsset*) override;

    UTexture2D* GetBubbleTexture() const;
    const FLinearColor& GetBubbleColor(const UThemeDataAsset*) const;
    const FLinearColor& GetTextColor(const UThemeDataAsset*) const;
    FText GetText() const;

    UPROPERTY(EditAnywhere, Category = Setup)
    FMessage Message;
    UPROPERTY(EditAnywhere, Category = Setup)
    EBubbleStackPosition Position;
    UPROPERTY(EditAnywhere, Category = Setup)
    EMessageSide Side;
};
