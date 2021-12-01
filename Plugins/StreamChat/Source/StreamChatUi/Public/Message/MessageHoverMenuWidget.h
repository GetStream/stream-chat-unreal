// Copyright 2021 Stream.IO, Inc. All Rights Reserved.

#pragma once

#include "Channel/Message.h"
#include "Components/Button.h"
#include "Components/GridPanel.h"
#include "Components/MenuAnchor.h"
#include "ContextMenu/ContextMenuWidget.h"
#include "CoreMinimal.h"
#include "Reaction/ReactionPickerWidget.h"
#include "StreamUserWidget.h"

#include "MessageHoverMenuWidget.generated.h"

/**
 * @brief Shown when mouse hovers over message.
 *
 * Includes Options and Reactions buttons.
 */
UCLASS()
class STREAMCHATUI_API UMessageHoverMenuWidget final : public UStreamUserWidget
{
    GENERATED_BODY()

public:
    UFUNCTION(BlueprintCallable, Category = "Stream Chat")
    void Setup(const FMessage& InMessage, EMessageSide InSide);

protected:
    UPROPERTY(meta = (BindWidget))
    UButton* ReactionButton;

    UPROPERTY(meta = (BindWidget))
    UButton* OptionsButton;

    UPROPERTY(meta = (BindWidget))
    UMenuAnchor* ReactionMenuAnchor;

    UPROPERTY(meta = (BindWidget))
    UMenuAnchor* OptionsMenuAnchor;

    UPROPERTY(EditDefaultsOnly, NoClear, Category = Defaults)
    TSubclassOf<UReactionPickerWidget> ReactionPickerWidgetClass = UReactionPickerWidget::StaticClass();

    UPROPERTY(EditDefaultsOnly, NoClear, Category = Defaults)
    TSubclassOf<UContextMenuWidget> ContextMenuWidgetClass = UContextMenuWidget::StaticClass();

private:
    virtual void OnSetup() override;

    UFUNCTION()
    void OnOptionsButtonClicked();
    UFUNCTION()
    void OnReactionButtonClicked();
    UFUNCTION()
    UUserWidget* CreateOptionsMenu();
    UFUNCTION()
    UUserWidget* CreateReactionsMenu();

    // TODO Just ID?
    UPROPERTY(EditAnywhere, Category = Setup)
    FMessage Message;

    UPROPERTY(EditAnywhere, Category = Setup)
    EMessageSide Side;

#if WITH_EDITOR
    static void ValidateChild(const FName& Parent, const FName& Child, const UWidgetTree& BlueprintWidgetTree, IWidgetCompilerLog& CompileLog);
    virtual void ValidateCompiledWidgetTree(const UWidgetTree& BlueprintWidgetTree, IWidgetCompilerLog& CompileLog) const override;
#endif
};
