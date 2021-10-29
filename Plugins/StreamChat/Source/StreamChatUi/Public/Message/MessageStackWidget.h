// Copyright Stream.IO, Inc. All Rights Reserved.

#pragma once

#include "Blueprint/UserWidget.h"
#include "Channel/Message.h"
#include "Components/SizeBox.h"
#include "CoreMinimal.h"
#include "TextBubbleWidget.h"
#include "TimestampWidget.h"

#include "MessageStackWidget.generated.h"

/**
 *
 */
UCLASS()
class STREAMCHATUI_API UMessageStackWidget final : public UUserWidget
{
    GENERATED_BODY()

public:
    UFUNCTION(BlueprintCallable, Category = "Stream Chat")
    void Setup(const TArray<FMessage> InMessages, EBubbleStackSide InSide);
    virtual bool Initialize() override;
    virtual void NativePreConstruct() override;

protected:
    UPROPERTY(meta = (BindWidget))
    UPanelWidget* TextBubblePanel;

    UPROPERTY(meta = (BindWidget))
    UTimestampWidget* Timestamp;

    UPROPERTY(meta = (BindWidget))
    USizeBox* SizeBox;

    UPROPERTY(EditDefaultsOnly, NoClear, Category = "Text Bubble")
    TSubclassOf<UTextBubbleWidget> TextBubbleWidgetClass = UTextBubbleWidget::StaticClass();

private:
    void SetupChildren();

    UPROPERTY(EditAnywhere, Category = Defaults)
    TArray<FMessage> Messages;

    UPROPERTY(EditAnywhere, Category = Defaults)
    EBubbleStackSide Side;
};
