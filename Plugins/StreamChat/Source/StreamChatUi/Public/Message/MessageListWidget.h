// Copyright 2021 Stream.IO, Inc. All Rights Reserved.

#pragma once

#include "Channel/Message.h"
#include "Components/ScrollBox.h"
#include "CoreMinimal.h"
#include "MessageStackWidget.h"
#include "StreamWidget.h"

#include "MessageListWidget.generated.h"

/**
 *
 */
UCLASS()
class STREAMCHATUI_API UMessageListWidget final : public UStreamWidget
{
    GENERATED_BODY()

public:
    UFUNCTION(BlueprintCallable, Category = "Stream Chat")
    void CreateMessageStackWidgets(const TArray<FMessage> Messages);

protected:
    UPROPERTY(meta = (BindWidget))
    UScrollBox* ScrollBox;

    UPROPERTY(EditDefaultsOnly, NoClear, Category = Defaults)
    TSubclassOf<UMessageStackWidget> MessageStackWidgetClass = UMessageStackWidget::StaticClass();
};
