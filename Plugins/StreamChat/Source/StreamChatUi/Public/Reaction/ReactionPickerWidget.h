// Copyright 2021 Stream.IO, Inc. All Rights Reserved.

#pragma once

#include "Channel/Message.h"
#include "Components/Border.h"
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

    UPROPERTY(meta = (BindWidget))
    UBorder* Border;

    UPROPERTY(EditAnywhere, NoClear, Category = Defaults)
    TSubclassOf<UReactionPickerButtonWidget> ReactionPickerButtonWidgetClass = UReactionPickerButtonWidget::StaticClass();

    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Setup)
    FMessage Message;

private:
    virtual void OnSetup() override;
    virtual bool WantsTheme() override
    {
        return true;
    }
    virtual void OnTheme(const UThemeDataAsset*) override;

    void OnReactionButtonClicked(const FName& ReactionType);
};
