// Copyright Stream.IO, Inc. All Rights Reserved.

#pragma once

#include "Blueprint/UserWidget.h"
#include "Channel/Message.h"
#include "Components/ScrollBox.h"
#include "CoreMinimal.h"
#include "MessageStackWidget.h"

#include "MessageListWidget.generated.h"

/**
 *
 */
UCLASS()
class STREAMCHATUI_API UMessageListWidget final : public UUserWidget
{
    GENERATED_BODY()

public:
    UFUNCTION(BlueprintCallable, Category = "Stream Chat")
    void CreateMessageStackWidgets(const TArray<FMessage> Messages);

protected:
    UPROPERTY(meta = (BindWidget))
    UScrollBox* ScrollBox;

    UPROPERTY(EditDefaultsOnly, NoClear, Category = "Timestamp")
    TSubclassOf<UMessageStackWidget> MessageStackWidgetClass = UMessageStackWidget::StaticClass();
};
