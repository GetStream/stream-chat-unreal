// Copyright 2021 Stream.IO, Inc. All Rights Reserved.

#pragma once

#include "Channel/Message.h"
#include "Components/Overlay.h"
#include "CoreMinimal.h"
#include "MessageHoverMenuWidget.h"
#include "Reaction/MessageReactionsWidget.h"
#include "StreamWidget.h"
#include "TextBubbleWidget.h"
#include "TimestampWidget.h"

#include "MessageWidget.generated.h"

/**
 * @brief Encapsulates a text bubble, reactions and the mouse hover menu for a message
 */
UCLASS()
class STREAMCHATUI_API UMessageWidget final : public UStreamWidget
{
    GENERATED_BODY()

public:
    explicit UMessageWidget(const FObjectInitializer& ObjectInitializer);
    UFUNCTION(BlueprintCallable, Category = "Stream Chat")
    void Setup(const FMessage& InMessage, EMessageSide InSide, EMessagePosition InPosition);

protected:
    UPROPERTY(meta = (BindWidgetOptional))
    UTextBubbleWidget* TextBubble;

    // Should contain whatever needs to be horizontally aligned
    UPROPERTY(meta = (BindWidgetOptional))
    UVerticalBox* AlignPanel;

    // Where the mouse hover menu will be spawned
    UPROPERTY(meta = (BindWidgetOptional))
    UContentWidget* HoverMenuTargetPanel;

    // Where the reactions will be spawned
    UPROPERTY(meta = (BindWidgetOptional))
    UContentWidget* ReactionsTargetPanel;

    // Where the timestamp will (maybe) be spawned
    UPROPERTY(meta = (BindWidgetOptional))
    UContentWidget* TimestampTargetPanel;

    // Where the avatar will (maybe) be spawned
    UPROPERTY(meta = (BindWidgetOptional))
    UContentWidget* AvatarTargetPanel;

    UPROPERTY(EditDefaultsOnly, NoClear, Category = Defaults)
    TSubclassOf<UMessageHoverMenuWidget> MouseHoverMenuWidgetClass = UMessageHoverMenuWidget::StaticClass();

    UPROPERTY(EditDefaultsOnly, NoClear, Category = Defaults)
    TSubclassOf<UMessageReactionsWidget> ReactionsWidgetClass = UMessageReactionsWidget::StaticClass();

    UPROPERTY(EditDefaultsOnly, NoClear, Category = Defaults)
    TSubclassOf<UAvatarWidget> AvatarWidgetClass = UAvatarWidget::StaticClass();

    UPROPERTY(EditDefaultsOnly, NoClear, Category = Defaults)
    TSubclassOf<UTimestampWidget> TimestampWidgetClass = UTimestampWidget::StaticClass();

    UPROPERTY(EditDefaultsOnly, Category = Defaults)
    int32 AvatarSize = 72;

    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Setup)
    FMessage Message;
    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Setup)
    EMessagePosition Position;
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
