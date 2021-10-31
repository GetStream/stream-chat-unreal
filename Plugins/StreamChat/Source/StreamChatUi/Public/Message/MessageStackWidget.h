// Copyright Stream.IO, Inc. All Rights Reserved.

#pragma once

#include "Channel/Message.h"
#include "Components/VerticalBox.h"
#include "CoreMinimal.h"
#include "MessageWidget.h"
#include "StreamUserWidget.h"
#include "TextBubbleWidget.h"
#include "TimestampWidget.h"

#include "MessageStackWidget.generated.h"

/**
 *
 */
UCLASS()
class STREAMCHATUI_API UMessageStackWidget final : public UStreamUserWidget
{
    GENERATED_BODY()

public:
    UFUNCTION(BlueprintCallable, Category = "Stream Chat")
    void Setup(const TArray<FMessage>& InMessages, EBubbleStackSide InSide);

protected:
    virtual void OnSetup() override;

    UPROPERTY(meta = (BindWidget))
    UVerticalBox* MessagesPanel;

    UPROPERTY(meta = (BindWidget))
    UTimestampWidget* Timestamp;

    UPROPERTY(EditDefaultsOnly, NoClear, Category = Defaults)
    TSubclassOf<UMessageWidget> MessageWidgetClass = UMessageWidget::StaticClass();

private:
    UPROPERTY(EditAnywhere, Category = Defaults)
    TArray<FMessage> Messages;

    UPROPERTY(EditAnywhere, Category = Defaults)
    EBubbleStackSide Side;
};
