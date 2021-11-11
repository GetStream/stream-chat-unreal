// Copyright Stream.IO, Inc. All Rights Reserved.

#pragma once

#include "Channel/Message.h"
#include "CoreMinimal.h"
#include "ReactionPickerButtonWidget.h"
#include "StreamUserWidget.h"

#include "ReactionPickerWidget.generated.h"

/**
 *
 */
UCLASS()
class STREAMCHATUI_API UReactionPickerWidget final : public UStreamUserWidget
{
    GENERATED_BODY()

public:
    UFUNCTION(BlueprintCallable, Category = "Stream Chat")
    void Setup(const FMessage& InMessage);

protected:
    UPROPERTY(meta = (BindWidget))
    UPanelWidget* ButtonsPanel;

    UPROPERTY(EditAnywhere, NoClear, Category = Defaults)
    TSubclassOf<UReactionPickerButtonWidget> ReactionPickerButtonWidgetClass =
        UReactionPickerButtonWidget::StaticClass();

private:
    virtual void OnSetup() override;

    void OnReactionButtonClicked(const FName& ReactionType);

    UPROPERTY(EditAnywhere, Category = Setup)
    FMessage Message;
};
