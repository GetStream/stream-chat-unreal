// Copyright 2021 Stream.IO, Inc. All Rights Reserved.

#pragma once

#include "Channel/Message.h"
#include "Components/ScrollBox.h"
#include "CoreMinimal.h"
#include "MessageStackWidget.h"
#include "StreamWidget.h"

#include "MessageListWidget.generated.h"

class UChannelContextWidget;

/**
 *
 */
UCLASS()
class STREAMCHATUI_API UMessageListWidget final : public UStreamWidget
{
    GENERATED_BODY()

public:
    UMessageListWidget();

protected:
    virtual void NativeOnInitialized() override;
    virtual void OnChannel() override;
    virtual void NativeDestruct() override;

    UFUNCTION(BlueprintCallable, Category = "Stream Chat")
    void CreateMessageStackWidgets(const TArray<FMessage>& Messages);

    UPROPERTY(meta = (BindWidget))
    UScrollBox* ScrollBox;

    // If the scroll offset is below this value, then new messages will be fetched
    UPROPERTY(EditDefaultsOnly, Category = Defaults)
    float PaginateScrollThreshold = 100.f;

    UPROPERTY(EditDefaultsOnly, NoClear, Category = Defaults) TSubclassOf<UMessageStackWidget> MessageStackWidgetClass = UMessageStackWidget::StaticClass();

private:
    UFUNCTION()
    void SetMessages(const TArray<FMessage>& Messages);
    UFUNCTION()
    void ScrollToBottom(const FMessage& Message);
    UFUNCTION()
    void OnUserScroll(float CurrentOffset);
};
