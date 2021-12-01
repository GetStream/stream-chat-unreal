// Copyright 2021 Stream.IO, Inc. All Rights Reserved.

#pragma once

#include "BottomReactionWidget.h"
#include "Channel/Message.h"
#include "Components/HorizontalBox.h"
#include "Components/PanelWidget.h"
#include "CoreMinimal.h"
#include "Message/MessageSide.h"
#include "StreamUserWidget.h"

#include "MessageReactionsWidget.generated.h"

/**
 *
 */
UCLASS()
class STREAMCHATUI_API UMessageReactionsWidget final : public UStreamUserWidget
{
    GENERATED_BODY()

public:
    UFUNCTION(BlueprintCallable, Category = "Stream Chat")
    void Setup(const FMessage& InMessage, EMessageSide InSide);

protected:
    UPROPERTY(meta = (BindWidget))
    UHorizontalBox* ReactionsPanel;

    UPROPERTY(EditAnywhere, NoClear, Category = Defaults)
    TSubclassOf<UBottomReactionWidget> BottomReactionWidgetClass = UBottomReactionWidget::StaticClass();

    UPROPERTY(EditAnywhere, Category = Defaults)
    float Spacing = 8.f;

private:
    virtual void OnSetup() override;

    FMargin GetPadding() const;

    UPROPERTY(EditAnywhere, Category = Setup)
    FMessage Message;

    UPROPERTY(EditAnywhere, Category = Setup)
    EMessageSide Side;
};
