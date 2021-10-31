// Copyright Stream.IO, Inc. All Rights Reserved.

#pragma once

#include "Blueprint/UserWidget.h"
#include "Channel/Message.h"
#include "Components/VerticalBox.h"
#include "CoreMinimal.h"
#include "MessageWidget.h"
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
    virtual bool Initialize() override;
    UFUNCTION(BlueprintCallable, Category = "Stream Chat")
    void Setup(const TArray<FMessage>& InMessages, EBubbleStackSide InSide);
    virtual void NativePreConstruct() override;

protected:
    UPROPERTY(meta = (BindWidget))
    UVerticalBox* MessagesPanel;

    UPROPERTY(meta = (BindWidget))
    UTimestampWidget* Timestamp;

    UPROPERTY(EditDefaultsOnly, NoClear, Category = Defaults)
    TSubclassOf<UMessageWidget> MessageWidgetClass = UMessageWidget::StaticClass();

private:
    void SetupChildren();

    UPROPERTY(EditAnywhere, Category = Defaults)
    TArray<FMessage> Messages;

    UPROPERTY(EditAnywhere, Category = Defaults)
    EBubbleStackSide Side;
};
