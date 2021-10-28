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

    UPROPERTY(EditAnywhere, Category = Texture)
    UTexture2D* MeOpeningTexture;
    UPROPERTY(EditAnywhere, Category = Texture)
    UTexture2D* YouOpeningTexture;
    UPROPERTY(EditAnywhere, Category = Texture)
    UTexture2D* MeEndTexture;
    UPROPERTY(EditAnywhere, Category = Texture)
    UTexture2D* YouEndTexture;

private:
    UTexture2D* GetBorderTexture() const;
};
