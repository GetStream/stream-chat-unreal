// Copyright 2022 Stream.IO, Inc. All Rights Reserved.

#pragma once

#include "Channel/Message.h"
#include "Components/Button.h"
#include "Components/GridPanel.h"
#include "Components/MenuAnchor.h"
#include "ContextMenu/ContextMenuWidget.h"
#include "CoreMinimal.h"
#include "Reaction/ReactionPickerWidget.h"
#include "StreamWidget.h"

#include "MessageHoverMenuWidget.generated.h"

/**
 * @brief Shown when mouse hovers over message.
 *
 * Includes Options and Reactions buttons.
 */
UCLASS()
class STREAMCHATUI_API UMessageHoverMenuWidget final : public UStreamWidget
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

    // TODO Just ID?
    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Setup)
    FMessage Message;

    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Setup)
    EMessageSide Side;

private:
    virtual void OnSetup() override;
    virtual void NativeConstruct() override;
    virtual void NativeDestruct() override;

    UFUNCTION()
    void OnOptionsButtonClicked();
    UFUNCTION()
    void OnReactionButtonClicked();
    UFUNCTION()
    UUserWidget* CreateOptionsMenu();
    UFUNCTION()
    UUserWidget* CreateReactionsMenu();

#if WITH_EDITOR
    static void ValidateChild(const FName& Parent, const FName& Child, const UWidgetTree& BlueprintWidgetTree, IWidgetCompilerLog& CompileLog);
    virtual void ValidateCompiledWidgetTree(const UWidgetTree& BlueprintWidgetTree, IWidgetCompilerLog& CompileLog) const override;
#endif
};
