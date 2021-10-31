// Copyright Stream.IO, Inc. All Rights Reserved.

#pragma once

#include "Blueprint/UserWidget.h"
#include "Channel/Message.h"
#include "Components/Button.h"
#include "ContextMenuWidget.h"
#include "CoreMinimal.h"
#include "ReactionPickerWidget.h"

#include "MessageHoverMenuWidget.generated.h"

/**
 * Shown when mouse hovers over message. Includes Options and Reactions buttons.
 */
UCLASS()
class STREAMCHATUI_API UMessageHoverMenuWidget final : public UUserWidget
{
    GENERATED_BODY()

public:
    UFUNCTION(BlueprintCallable, Category = "Stream Chat")
    void Setup(const FMessage& InMessage, EBubbleStackSide InSide);
    virtual bool Initialize() override;
    virtual void NativeOnInitialized() override;

protected:
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

    void SetupChildren() const;

    // TODO Just ID?
    UPROPERTY(EditAnywhere, Category = Setup)
    FMessage Message;

    UPROPERTY(EditAnywhere, Category = Setup)
    EBubbleStackSide Side;
};
