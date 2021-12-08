// Copyright 2021 Stream.IO, Inc. All Rights Reserved.

#pragma once

#include "Channel/Message.h"
#include "Components/Overlay.h"
#include "CoreMinimal.h"
#include "MessageHoverMenuWidget.h"
#include "Reaction/MessageReactionsWidget.h"
#include "StreamUserWidget.h"
#include "TextBubbleWidget.h"

#include "MessageWidget.generated.h"

/**
 * @brief Encapsulates a text bubble, reactions and the mouse hover menu for a message
 */
UCLASS()
class STREAMCHATUI_API UMessageWidget final : public UStreamUserWidget
{
    GENERATED_BODY()

public:
    explicit UMessageWidget(const FObjectInitializer& ObjectInitializer);
    UFUNCTION(BlueprintCallable, Category = "Stream Chat")
    void Setup(const FMessage& InMessage, EMessageSide InSide, EBubbleStackPosition InPosition);

protected:
    // Should contain whatever needs to be horizontally aligned
    UPROPERTY(meta = (BindWidget))
    UVerticalBox* OuterPanel;

    UPROPERTY(meta = (BindWidget))
    UTextBubbleWidget* TextBubble;

    // Where the mouse hover menu will be spawned
    UPROPERTY(meta = (BindWidget))
    UContentWidget* HoverMenuTargetPanel;

    // Where the reactions will be spawned
    UPROPERTY(meta = (BindWidget))
    UContentWidget* ReactionsTargetPanel;

    // TODO Reactions

    UPROPERTY(EditDefaultsOnly, NoClear, Category = Defaults)
    TSubclassOf<UMessageHoverMenuWidget> MouseHoverMenuWidgetClass = UMessageHoverMenuWidget::StaticClass();

    UPROPERTY(EditDefaultsOnly, NoClear, Category = Defaults)
    TSubclassOf<UMessageReactionsWidget> ReactionsWidgetClass = UMessageReactionsWidget::StaticClass();

    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Setup)
    FMessage Message;
    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Setup)
    EBubbleStackPosition Position;
    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Setup)
    EMessageSide Side;

private:
    virtual void OnSetup() override;

    virtual void NativeOnMouseEnter(const FGeometry& InGeometry, const FPointerEvent& InMouseEvent) override;
    virtual void NativeOnMouseLeave(const FPointerEvent& InMouseEvent) override;

    bool ShouldDisplayHoverMenu() const;

    // Only valid while hovered
    UPROPERTY(Transient)
    UMessageHoverMenuWidget* MouseHoverMenu;

    // Only valid if message has reactions
    UPROPERTY(Transient)
    UMessageReactionsWidget* Reactions;
};
