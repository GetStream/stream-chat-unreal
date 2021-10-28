// Copyright Stream.IO, Inc. All Rights Reserved.

#pragma once

#include "Blueprint/UserWidget.h"
#include "Channel/Message.h"
#include "CoreMinimal.h"
#include "TextBubbleWidget.h"

#include "MessageStackWidget.generated.h"

/**
 *
 */
UCLASS()
class STREAMCHATUI_API UMessageStackWidget final : public UUserWidget
{
    GENERATED_BODY()

public:
    virtual void NativePreConstruct() override;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (ExposeOnSpawn = true))
    TArray<FMessage> Messages;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (ExposeOnSpawn = true))
    FUser CurrentUser;

protected:
    UPROPERTY(meta = (BindWidget))
    UPanelWidget* TextBubblePanel;

    UPROPERTY(EditDefaultsOnly, NoClear)
    TSubclassOf<UTextBubbleWidget> TextBubbleWidgetClass = UTextBubbleWidget::StaticClass();
};
