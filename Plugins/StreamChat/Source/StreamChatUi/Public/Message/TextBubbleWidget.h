// Copyright Stream.IO, Inc. All Rights Reserved.

#pragma once

#include "Blueprint/UserWidget.h"
#include "Channel/Message.h"
#include "Components/Border.h"
#include "Components/TextBlock.h"
#include "CoreMinimal.h"

#include "TextBubbleWidget.generated.h"

UENUM(BlueprintType)
enum class EBubbleStackPosition : uint8
{
    Opening,
    End
};

UENUM(BlueprintType)
enum class EBubbleStackSide : uint8
{
    // Left
    You,
    // Right
    Me
};

/**
 *
 */
UCLASS()
class STREAMCHATUI_API UTextBubbleWidget final : public UUserWidget
{
    GENERATED_BODY()

public:
    UFUNCTION(BlueprintCallable, Category = "Stream Chat")
    void Setup(const FMessage& InMessage, EBubbleStackSide InSide, EBubbleStackPosition InPosition);
    virtual void NativePreConstruct() override;

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
    UPROPERTY(EditAnywhere, Category = Bubble)
    FLinearColor MeBubbleColor;
    UPROPERTY(EditAnywhere, Category = Bubble)
    FLinearColor DeletedBubbleColor;
    UPROPERTY(EditAnywhere, Category = Text)
    FLinearColor NormalTextColor;
    UPROPERTY(EditAnywhere, Category = Text)
    FLinearColor DeletedTextColor;

private:
    UTexture2D* GetBubbleTexture() const;
    const FLinearColor& GetBubbleColor() const;
    const FLinearColor& GetTextColor() const;
    FText GetText() const;

    UPROPERTY(EditAnywhere, Category = Defaults)
    FMessage Message;
    UPROPERTY(EditAnywhere, Category = Defaults)
    EBubbleStackPosition Position;
    UPROPERTY(EditAnywhere, Category = Defaults)
    EBubbleStackSide Side;
};
