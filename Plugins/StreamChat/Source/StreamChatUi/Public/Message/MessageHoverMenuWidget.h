// Copyright Stream.IO, Inc. All Rights Reserved.

#pragma once

#include "Channel/Message.h"
#include "Components/Button.h"
#include "ContextMenu/ContextMenuWidget.h"
#include "CoreMinimal.h"
#include "ReactionPickerWidget.h"
#include "StreamUserWidget.h"

#include "MessageHoverMenuWidget.generated.h"

/**
 * Shown when mouse hovers over message. Includes Options and Reactions buttons.
 */
UCLASS()
class STREAMCHATUI_API UMessageHoverMenuWidget final : public UStreamUserWidget
{
    GENERATED_BODY()

public:
    UFUNCTION(BlueprintCallable, Category = "Stream Chat")
    void Setup(const FMessage& InMessage, EBubbleStackSide InSide);

protected:
    virtual void OnSetup() override;

    UPROPERTY(meta = (BindWidget))
    UPanelWidget* ButtonGroup;

    UPROPERTY(meta = (BindWidget))
    UButton* ReactionButton;

    UPROPERTY(meta = (BindWidget))
    UButton* OptionsButton;

    UPROPERTY(EditDefaultsOnly, NoClear, Category = Defaults)
    TSubclassOf<UReactionPickerWidget> ReactionPickerWidgetClass = UReactionPickerWidget::StaticClass();

    UPROPERTY(EditDefaultsOnly, NoClear, Category = Defaults)
    TSubclassOf<UContextMenuWidget> ContextMenuWidgetClass = UContextMenuWidget::StaticClass();

private:
    UFUNCTION()
    void OnOptionsButtonClicked();
    UFUNCTION()
    void OnReactionButtonClicked();

    // TODO Just ID?
    UPROPERTY(EditAnywhere, Category = Setup)
    FMessage Message;

    UPROPERTY(EditAnywhere, Category = Setup)
    EBubbleStackSide Side;
};
