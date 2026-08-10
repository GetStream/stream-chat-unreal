// Copyright 2026 Stream.IO, Inc. All Rights Reserved.

#pragma once

#include "Channel/Message.h"
#include "Components/VerticalBox.h"
#include "ContextMenuAction.h"
#include "ContextMenuButtonWidget.h"
#include "CoreMinimal.h"
#include "Message/MessageSide.h"
#include "StreamWidget.h"

#include "ContextMenuWidget.generated.h"

/**
 *
 */
UCLASS()
class STREAMCHATUI_API UContextMenuWidget final : public UStreamWidget
{
    GENERATED_BODY()

public:
    UFUNCTION(BlueprintCallable, Category = "Stream Chat")
    void Setup(const FMessage& InMessage, EMessageSide InSide);

    /**
     * @brief Show a widget above the action buttons, inside this menu's panel
     *
     * Used to hang the reaction picker off the long press menu, so one gesture offers both reacting
     * and the actions. Call before the menu is added to the UI: the buttons are built on construct,
     * and the header goes in with them.
     */
    UFUNCTION(BlueprintCallable, Category = "Stream Chat")
    void SetHeaderContent(UWidget* Content);

protected:
    UPROPERTY(meta = (BindWidget))
    UVerticalBox* ButtonsPanel;

    UPROPERTY(EditAnywhere, Instanced, Category = Defaults, meta = (TitleProperty = Label))
    TArray<UContextMenuAction*> Actions;

    UPROPERTY(EditDefaultsOnly, NoClear, Category = Defaults)
    TSubclassOf<UContextMenuButtonWidget> ContextMenuButtonWidgetClass = UContextMenuButtonWidget::StaticClass();

    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Setup)
    FMessage Message;

    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Setup)
    EMessageSide Side;

private:
    virtual void NativePreConstruct() override;
    void AddButton(UContextMenuAction* Action, EContextMenuButtonPosition Position);

    /// Sits above the buttons once the menu is built. Null unless a caller supplied one.
    UPROPERTY(Transient)
    UWidget* HeaderContent;
};
