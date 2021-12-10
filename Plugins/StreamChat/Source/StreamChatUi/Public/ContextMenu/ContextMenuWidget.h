// Copyright 2021 Stream.IO, Inc. All Rights Reserved.

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
    virtual void OnSetup() override;
    void AddButton(UContextMenuAction* Action, EContextMenuButtonPosition Position);
};
