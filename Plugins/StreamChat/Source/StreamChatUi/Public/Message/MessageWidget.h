// Copyright Stream.IO, Inc. All Rights Reserved.

#pragma once

#include "Channel/Message.h"
#include "Components/Overlay.h"
#include "CoreMinimal.h"
#include "MessageHoverMenuWidget.h"
#include "StreamUserWidget.h"
#include "TextBubbleWidget.h"

#include "MessageWidget.generated.h"

/**
 * Encapsulates a text bubble, reactions and the mouse hover menu for a message
 */
UCLASS()
class STREAMCHATUI_API UMessageWidget final : public UStreamUserWidget
{
    GENERATED_BODY()

public:
    explicit UMessageWidget(const FObjectInitializer& ObjectInitializer);
    UFUNCTION(BlueprintCallable, Category = "Stream Chat")
    void Setup(const FMessage& InMessage, EBubbleStackSide InSide, EBubbleStackPosition InPosition);

protected:
    virtual void OnSetup() override;

    // Should contain whatever needs to be horizontally aligned
    UPROPERTY(meta = (BindWidget))
    UOverlay* OuterOverlay;

    UPROPERTY(meta = (BindWidget))
    UTextBubbleWidget* TextBubble;

    // Where the mouse hover menu will be spawned
    UPROPERTY(meta = (BindWidget))
    UContentWidget* HoverMenuTargetPanel;

    // TODO Reactions

    UPROPERTY(EditDefaultsOnly, NoClear, Category = Defaults)
    TSubclassOf<UMessageHoverMenuWidget> MouseHoverMenuWidgetClass = UMessageHoverMenuWidget::StaticClass();

private:
    virtual void NativeOnMouseEnter(const FGeometry& InGeometry, const FPointerEvent& InMouseEvent) override;
    virtual void NativeOnMouseLeave(const FPointerEvent& InMouseEvent) override;

    UPROPERTY(EditAnywhere, Category = Setup)
    FMessage Message;
    UPROPERTY(EditAnywhere, Category = Setup)
    EBubbleStackPosition Position;
    UPROPERTY(EditAnywhere, Category = Setup)
    EBubbleStackSide Side;

    // Only valid while hovered
    UPROPERTY(Transient)
    UMessageHoverMenuWidget* MouseHoverMenu;
};
