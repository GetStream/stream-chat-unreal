// Copyright 2026 Stream.IO, Inc. All Rights Reserved.

#pragma once

#include "AttachmentWidget.h"
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
    explicit UMessageWidget();
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

    UPROPERTY(EditDefaultsOnly, NoClear, Category = Defaults)
    TSubclassOf<UAttachmentWidget> AttachmentWidgetClass = UAttachmentWidget::StaticClass();

    UPROPERTY(EditDefaultsOnly, Category = Defaults)
    int32 AvatarSize = 36;

    /// Space around each attachment shown above the message
    UPROPERTY(EditDefaultsOnly, Category = Defaults)
    FMargin AttachmentPadding = FMargin{0.f, 0.f, 0.f, 2.f};

    /// Space around the reply count shown below a message that has a thread
    UPROPERTY(EditDefaultsOnly, Category = Thread)
    FMargin ThreadFooterPadding = FMargin{2.f, 2.f, 2.f, 0.f};

    UPROPERTY(EditDefaultsOnly, Category = Thread)
    int32 ThreadFooterFontSize = 12;

    /**
     * @brief How long a press has to be held to bring up the message actions.
     *
     * The hover menu is the only route to the actions and the reaction picker, and hover does not
     * exist on a touch screen, so a long press opens them directly.
     */
    UPROPERTY(EditDefaultsOnly, Category = "Message Actions")
    float LongPressSeconds = 0.4f;

    /**
     * @brief How far the message may shift during a press before it stops counting as a long press.
     *
     * A press that drags the list is a scroll, and the message moves with it. Comparing the widget's
     * own position catches that even when the list view has taken the pointer and no move events
     * reach this widget at all.
     */
    UPROPERTY(EditDefaultsOnly, Category = "Message Actions")
    float LongPressMoveTolerance = 4.f;

    /// Space around the warning shown below a message moderation bounced
    UPROPERTY(EditDefaultsOnly, Category = Moderation)
    FMargin ModerationWarningPadding = FMargin{2.f, 2.f, 2.f, 0.f};

    UPROPERTY(EditDefaultsOnly, Category = Moderation)
    int32 ModerationWarningFontSize = 12;

    /// Shown under the author's own message when moderation bounced it back to be rephrased
    UPROPERTY(EditDefaultsOnly, Category = Moderation)
    FText BouncedMessageText = NSLOCTEXT("StreamChat", "BouncedMessage", "Not sent. Hold to edit, send anyway or delete.");

    /// Shown under a message that has exactly one reply
    UPROPERTY(EditDefaultsOnly, Category = Thread)
    FText OneReplyText = NSLOCTEXT("StreamChat", "OneReply", "1 reply");

    /// Shown under a message with several replies. {0} is the count.
    UPROPERTY(EditDefaultsOnly, Category = Thread)
    FText ManyRepliesFormat = NSLOCTEXT("StreamChat", "ManyReplies", "{0} replies");

    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Setup)
    FMessage Message;
    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Setup)
    EMessagePosition Position;
    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Setup)
    EMessageSide Side;

private:
    virtual void OnSetup() override;
    virtual void NativeConstruct() override;
    virtual void NativeDestruct() override;

    virtual void NativeOnMouseEnter(const FGeometry& InGeometry, const FPointerEvent& InMouseEvent) override;
    virtual void NativeOnMouseLeave(const FPointerEvent& InMouseEvent) override;
    virtual FReply NativeOnMouseButtonDown(const FGeometry& InGeometry, const FPointerEvent& InMouseEvent) override;
    virtual FReply NativeOnMouseButtonUp(const FGeometry& InGeometry, const FPointerEvent& InMouseEvent) override;

    bool ShouldDisplayHoverMenu() const;

    /** @name Long press
     *  Press and hold to bring up the reaction picker and the message actions.
     *  @{
     */
    void BeginLongPress();
    void CancelLongPress();
    void OnLongPressElapsed();
    bool ShouldDisplayActionsMenu() const;
    void CreateActionsMenuAnchor();

    UFUNCTION()
    UUserWidget* CreateActionsMenu();
    /// @}

    void CreateAttachmentWidgets();

    /**
     * Build the "Reply in thread" / "N replies" line under the message.
     *
     * A tap target rather than something on the message hover menu, because the hover menu never
     * appears on a touch screen and this has to work on a phone.
     */
    void CreateThreadFooter();
    bool ShouldDisplayThreadFooter() const;
    FText GetThreadFooterText() const;

    /**
     * Build the "not sent" warning under a message moderation bounced.
     *
     * The author is the only one who ever sees a bounced message, and without this the message looks
     * sent. The actions to do something about it are on the long press menu, which the warning says
     * so, because a bounced message has no affordance of its own.
     */
    void CreateModerationWarning();
    bool ShouldDisplayModerationWarning() const;

    UFUNCTION()
    void OnThreadFooterClicked();

    // Only valid while hovered
    UPROPERTY(Transient)
    UMessageHoverMenuWidget* MouseHoverMenu;

    // Only valid if message has reactions
    UPROPERTY(Transient)
    UMessageReactionsWidget* Reactions;

    // Live in AlignPanel rather than a panel of their own, so tracked here to be replaced on re-setup
    UPROPERTY(Transient)
    TArray<UAttachmentWidget*> Attachments;

    // Also lives in AlignPanel, for the same reason: WBP_Message has no slot for it
    UPROPERTY(Transient)
    UButton* ThreadFooterButton;

    UPROPERTY(Transient)
    UTextBlock* ThreadFooterText;

    /// Also in AlignPanel, below the bubble. Null unless moderation bounced this message.
    UPROPERTY(Transient)
    UTextBlock* ModerationWarningText;

    /// Hosts the long press menu. Empty and invisible until opened, and spawned in C++ because
    /// WBP_Message has no anchor of its own.
    UPROPERTY(Transient)
    UMenuAnchor* ActionsMenuAnchor;

    FTimerHandle LongPressTimer;
    /// Where this widget sat when the press started, to tell a long press from a scroll
    FVector2D PressPosition = FVector2D::ZeroVector;
};
