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
    virtual void NativePreConstruct() override;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Stream Chat", meta = (ExposeOnSpawn = true))
    FMessage Message;
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Stream Chat", meta = (ExposeOnSpawn = true))
    EBubbleStackPosition Position;
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Stream Chat", meta = (ExposeOnSpawn = true))
    EBubbleStackSide Side;

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
};
