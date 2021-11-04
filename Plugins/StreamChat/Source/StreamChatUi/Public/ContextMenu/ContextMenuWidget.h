﻿// Copyright Stream.IO, Inc. All Rights Reserved.

#pragma once

#include "Channel/Message.h"
#include "Components/VerticalBox.h"
#include "ContextMenuAction.h"
#include "ContextMenuButtonWidget.h"
#include "CoreMinimal.h"
#include "Message/MessageSide.h"
#include "StreamUserWidget.h"

#include "ContextMenuWidget.generated.h"

/**
 *
 */
UCLASS()
class STREAMCHATUI_API UContextMenuWidget final : public UStreamUserWidget
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

private:
    virtual void OnSetup() override;
    void AddButton(UContextMenuAction* Action, EContextMenuButtonPosition Position);

    UPROPERTY(EditAnywhere, Category = Defaults)
    FMessage Message;

    UPROPERTY(EditAnywhere, Category = Setup)
    EMessageSide Side;
};