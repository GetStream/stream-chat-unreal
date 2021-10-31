// Copyright Stream.IO, Inc. All Rights Reserved.

#pragma once

#include "Blueprint/UserWidget.h"
#include "Channel/Message.h"
#include "Components/Image.h"
#include "Components/Overlay.h"
#include "CoreMinimal.h"
#include "TextBubbleWidget.h"

#include "TimestampWidget.generated.h"

/**
 *
 */
UCLASS()
class STREAMCHATUI_API UTimestampWidget final : public UUserWidget
{
    GENERATED_BODY()

public:
    UFUNCTION(BlueprintCallable, Category = "Stream Chat")
    void Setup(const FMessage& InMessage, EBubbleStackSide InSide);
    virtual void NativePreConstruct() override;

protected:
    UPROPERTY(meta = (BindWidget))
    UOverlay* OuterOverlay;

    UPROPERTY(meta = (BindWidget))
    UTextBlock* UserTextBlock;

    UPROPERTY(meta = (BindWidget))
    UImage* MessageStateIconImage;

    UPROPERTY(meta = (BindWidget))
    UTextBlock* DateTimeTextBlock;

    UPROPERTY(EditAnywhere, Category = "Icon")
    UTexture2D* IconClock;

    UPROPERTY(EditAnywhere, Category = "Icon")
    UTexture2D* IconCheck;

    UPROPERTY(EditAnywhere, Category = "Icon")
    UTexture2D* IconCheckAll;

private:
    UTexture2D* GetStatusIcon() const;
    FText GetTimestampText() const;
    static FText GetDayOfWeek(const FDateTime&);

    UPROPERTY(EditAnywhere, Category = Defaults)
    FMessage Message;

    UPROPERTY(EditAnywhere, Category = Defaults)
    EBubbleStackSide Side;
};
