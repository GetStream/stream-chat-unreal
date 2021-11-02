// Copyright Stream.IO, Inc. All Rights Reserved.

#pragma once

#include "Channel/Message.h"
#include "Components/VerticalBox.h"
#include "ContextMenuAction.h"
#include "ContextMenuButtonWidget.h"
#include "CoreMinimal.h"
#include "Message/BubbleStackSide.h"
#include "StreamUserWidget.h"

#include "ContextMenuWidget.generated.h"

USTRUCT(BlueprintType)
struct FContextMenuActions
{
    GENERATED_BODY()

    UPROPERTY(EditAnywhere, Instanced, Category = Defaults, meta = (TitleProperty = Label))
    TArray<UContextMenuAction*> Actions;
};

/**
 *
 */
UCLASS()
class STREAMCHATUI_API UContextMenuWidget final : public UStreamUserWidget
{
    GENERATED_BODY()

public:
    UFUNCTION(BlueprintCallable, Category = "Stream Chat")
    void Setup(const FMessage& InMessage, EBubbleStackSide InSide);

protected:
    UPROPERTY(meta = (BindWidget))
    UVerticalBox* ButtonsPanel;

    UPROPERTY(EditAnywhere, Category = Defaults, meta = (ShowOnlyInnerProperties))
    TMap<EBubbleStackSide, FContextMenuActions> MenuActions;

    UPROPERTY(EditDefaultsOnly, NoClear, Category = Defaults)
    TSubclassOf<UContextMenuButtonWidget> ContextMenuButtonWidgetClass = UContextMenuButtonWidget::StaticClass();

private:
    virtual void OnSetup() override;
    void AddButton(UContextMenuAction* Action, EContextMenuButtonPosition Position);

    UPROPERTY(EditAnywhere, Category = Defaults)
    FMessage Message;

    UPROPERTY(EditAnywhere, Category = Setup)
    EBubbleStackSide Side;
};
