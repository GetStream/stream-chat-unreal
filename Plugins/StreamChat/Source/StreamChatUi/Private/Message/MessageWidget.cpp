// Copyright 2026 Stream.IO, Inc. All Rights Reserved.

#include "Message/MessageWidget.h"

#include "Blueprint/WidgetTree.h"
#include "Components/GridSlot.h"
#include "Components/HorizontalBoxSlot.h"
#include "Components/Spacer.h"
#include "Components/VerticalBoxSlot.h"
#include "Context/ChannelContextWidget.h"
#include "Framework/Application/SlateApplication.h"
#include "ThemeDataAsset.h"
#include "TimerManager.h"
#include "WidgetUtil.h"

namespace
{
/**
 * Move a child to the end of its parent, preserving the slot settings that AddChild would otherwise
 * reset. Used because UPanelWidget::ReplaceChildAt is only compiled into editor builds.
 */
void MoveChildToEnd(UPanelWidget& Parent, UWidget& Child)
{
    FMargin Padding{};
    FSlateChildSize Size{};
    EHorizontalAlignment HorizontalAlignment = HAlign_Fill;
    EVerticalAlignment VerticalAlignment = VAlign_Fill;
    const bool bHadHorizontalSlot = Child.Slot && Child.Slot->IsA<UHorizontalBoxSlot>();
    if (bHadHorizontalSlot)
    {
        const UHorizontalBoxSlot* Previous = CastChecked<UHorizontalBoxSlot>(Child.Slot);
        Padding = Previous->GetPadding();
        Size = Previous->GetSize();
        HorizontalAlignment = Previous->GetHorizontalAlignment();
        VerticalAlignment = Previous->GetVerticalAlignment();
    }

    Parent.RemoveChild(&Child);

    if (UHorizontalBoxSlot* Moved = Cast<UHorizontalBoxSlot>(Parent.AddChild(&Child)); Moved && bHadHorizontalSlot)
    {
        Moved->SetPadding(Padding);
        Moved->SetSize(Size);
        Moved->SetHorizontalAlignment(HorizontalAlignment);
        Moved->SetVerticalAlignment(VerticalAlignment);
    }
}
}    // namespace

UMessageWidget::UMessageWidget()
{
    // Ensure hovering events are fired
    UUserWidget::SetVisibility(ESlateVisibility::Visible);
}

void UMessageWidget::Setup(const FMessage& InMessage, const EMessageSide InSide, const EMessagePosition InPosition)
{
    Message = InMessage;
    Side = InSide;
    Position = InPosition;

    Super::Setup();
}

void UMessageWidget::OnSetup()
{
    if (HoverMenuTargetPanel)
    {
        if (UPanelWidget* HoverMenuParent = Cast<UPanelWidget>(HoverMenuTargetPanel->GetParent()))
        {
            // The hover menu sits before the bubble for our own messages and after it for everyone
            // else. UPanelWidget::ReplaceChildAt would express that directly but is editor only, so
            // the widget that belongs last is moved there instead.
            UWidget* Last = Side == EMessageSide::Me ? static_cast<UWidget*>(TextBubble) : HoverMenuTargetPanel;
            UWidget* First = Side == EMessageSide::Me ? HoverMenuTargetPanel : static_cast<UWidget*>(TextBubble);
            if (First && Last && HoverMenuParent->GetChildIndex(First) > HoverMenuParent->GetChildIndex(Last))
            {
                MoveChildToEnd(*HoverMenuParent, *Last);
            }
        }
    }

    if (ReactionsTargetPanel)
    {
        if (Message.Reactions.IsEmpty())
        {
            // In case the reactions target panel has some padding that needs to be removed
            ReactionsTargetPanel->SetVisibility(ESlateVisibility::Collapsed);
        }
        else
        {
            ReactionsTargetPanel->SetVisibility(ESlateVisibility::SelfHitTestInvisible);
            Reactions = CreateWidget<UMessageReactionsWidget>(this, ReactionsWidgetClass);
            Reactions->Setup(Message, Side);
            ReactionsTargetPanel->SetContent(Reactions);
        }
    }

    // Create timestamp widget
    if (TimestampTargetPanel)
    {
        if (Position == EMessagePosition::End)
        {
            const bool bShowUserName = Side == EMessageSide::You;
            const bool bShowMessageState = Side == EMessageSide::Me;
            UTimestampWidget* Widget = CreateWidget<UTimestampWidget>(this, TimestampWidgetClass);
            Widget->Setup(Message, bShowUserName, bShowMessageState);
            TimestampTargetPanel->SetContent(Widget);
            TimestampTargetPanel->SetVisibility(ESlateVisibility::SelfHitTestInvisible);
        }
        else
        {
            TimestampTargetPanel->SetVisibility(ESlateVisibility::Collapsed);
        }
    }

    // Create avatar widget
    if (AvatarTargetPanel)
    {
        if (Side == EMessageSide::You)
        {
            if (Position == EMessagePosition::End)
            {
                UAvatarWidget* Widget = CreateWidget<UAvatarWidget>(this, AvatarWidgetClass);
                Widget->Setup({Message.User}, AvatarSize);
                AvatarTargetPanel->SetContent(Widget);
                AvatarTargetPanel->SetVisibility(ESlateVisibility::SelfHitTestInvisible);
            }
            else
            {
                USpacer* Widget = WidgetTree->ConstructWidget<USpacer>();
                const float Size = static_cast<float>(AvatarSize);
                Widget->SetSize({Size, 0.f});
                AvatarTargetPanel->SetContent(Widget);
            }
        }
        else
        {
            AvatarTargetPanel->SetVisibility(ESlateVisibility::Collapsed);
        }
    }

    CreateAttachmentWidgets();

    // Align everything in the outer panel to the left or right
    if (AlignPanel)
    {
        for (UPanelSlot* PanelSlot : AlignPanel->GetSlots())
        {
            if (UVerticalBoxSlot* BoxSlot = Cast<UVerticalBoxSlot>(PanelSlot))
            {
                if (Side == EMessageSide::Me)
                {
                    BoxSlot->SetHorizontalAlignment(HAlign_Right);
                }
                else if (Side == EMessageSide::You)
                {
                    BoxSlot->SetHorizontalAlignment(HAlign_Left);
                }
            }
        }
    }

    if (TextBubble)
    {
        TextBubble->Setup(Message, Side, Position);
    }
}

void UMessageWidget::NativeConstruct()
{
    Super::NativeConstruct();

    // Not in OnSetup: these need the channel context, and that means walking up to an ancestor.
    // Setup() runs before the list view parents this widget, so there is no ancestor to find yet.
    CreateThreadFooter();
    CreateModerationWarning();
    CreateActionsMenuAnchor();
}

void UMessageWidget::NativeDestruct()
{
    CancelLongPress();
    Super::NativeDestruct();
}

FReply UMessageWidget::NativeOnMouseButtonDown(const FGeometry& InGeometry, const FPointerEvent& InMouseEvent)
{
    if (InMouseEvent.IsTouchEvent() || InMouseEvent.GetEffectingButton() == EKeys::LeftMouseButton)
    {
        BeginLongPress();
    }

    // Deliberately left unhandled. The list view needs this same press to start a scroll drag, and
    // capturing it here would stop the conversation scrolling at all.
    return Super::NativeOnMouseButtonDown(InGeometry, InMouseEvent);
}

FReply UMessageWidget::NativeOnMouseButtonUp(const FGeometry& InGeometry, const FPointerEvent& InMouseEvent)
{
    CancelLongPress();
    return Super::NativeOnMouseButtonUp(InGeometry, InMouseEvent);
}

void UMessageWidget::BeginLongPress()
{
    if (!ShouldDisplayActionsMenu() || !ActionsMenuAnchor || !GetWorld())
    {
        return;
    }

    CancelLongPress();
    PressPosition = FVector2D{GetCachedGeometry().GetAbsolutePosition()};
    GetWorld()->GetTimerManager().SetTimer(LongPressTimer, this, &UMessageWidget::OnLongPressElapsed, LongPressSeconds, false);
}

void UMessageWidget::CancelLongPress()
{
    if (LongPressTimer.IsValid() && GetWorld())
    {
        GetWorld()->GetTimerManager().ClearTimer(LongPressTimer);
    }
    LongPressTimer.Invalidate();
}

void UMessageWidget::OnLongPressElapsed()
{
    LongPressTimer.Invalidate();

    // The message moved, so the press was dragging the list rather than resting on this message. This
    // is the only reliable signal: once the list view takes the pointer, no move or up event arrives.
    const FVector2D Now{GetCachedGeometry().GetAbsolutePosition()};
    if (!Now.Equals(PressPosition, LongPressMoveTolerance))
    {
        return;
    }

    // A quick tap whose release went to the list view instead of here: the finger is already gone
    if (!IsHovered())
    {
        return;
    }

    if (ActionsMenuAnchor && !ActionsMenuAnchor->IsOpen())
    {
        ActionsMenuAnchor->Open(true);
        WidgetUtil::HideDefaultMenuBackground(ActionsMenuAnchor);
    }
}

bool UMessageWidget::ShouldDisplayActionsMenu() const
{
    return Message.Type != EMessageType::Deleted && !Message.Id.IsEmpty();
}

void UMessageWidget::CreateActionsMenuAnchor()
{
    if (!AlignPanel || !WidgetTree || ActionsMenuAnchor)
    {
        return;
    }

    ActionsMenuAnchor = WidgetTree->ConstructWidget<UMenuAnchor>();
    ActionsMenuAnchor->OnGetUserMenuContentEvent.BindDynamic(this, &UMessageWidget::CreateActionsMenu);
    ActionsMenuAnchor->SetPlacement(MenuPlacement_CenteredBelowAnchor);
    // Nothing to see or hit until it opens, and it must never swallow a tap meant for the message
    ActionsMenuAnchor->SetVisibility(ESlateVisibility::SelfHitTestInvisible);

    if (UVerticalBoxSlot* BoxSlot = Cast<UVerticalBoxSlot>(AlignPanel->AddChild(ActionsMenuAnchor)))
    {
        BoxSlot->SetHorizontalAlignment(Side == EMessageSide::Me ? HAlign_Right : HAlign_Left);
    }
}

UUserWidget* UMessageWidget::CreateActionsMenu()
{
    // The picker and the action list are configured on the hover menu's Blueprint, not on this widget.
    // Taking them from its defaults reuses that wiring; the native classes have no widget tree behind
    // them and would come up blank.
    const UMessageHoverMenuWidget* MenuDefaults = MouseHoverMenuWidgetClass.GetDefaultObject();
    const TSubclassOf<UContextMenuWidget> ActionsClass = MenuDefaults ? MenuDefaults->GetContextMenuWidgetClass() : nullptr;
    if (!ActionsClass)
    {
        return nullptr;
    }

    UContextMenuWidget* Actions = CreateWidget<UContextMenuWidget>(this, ActionsClass);

    // A menu anchor shows exactly one widget, so the picker rides along at the top of the action
    // sheet rather than as a second menu. That is also where a phone user expects it. Left out for a
    // bounced message: it was never stored, so there is nothing for a reaction to attach to.
    if (!Message.IsBounced())
    {
        if (const TSubclassOf<UReactionPickerWidget> PickerClass = MenuDefaults->GetReactionPickerWidgetClass())
        {
            UReactionPickerWidget* Picker = CreateWidget<UReactionPickerWidget>(this, PickerClass);
            Picker->Setup(Message);
            Actions->SetHeaderContent(Picker);
        }
    }

    Actions->Setup(Message, Side);
    return Actions;
}

void UMessageWidget::NativeOnMouseEnter(const FGeometry& InGeometry, const FPointerEvent& InMouseEvent)
{
    if (FSlateApplication::Get().AnyMenusVisible())
    {
        return;
    }

    if (ShouldDisplayHoverMenu())
    {
        MouseHoverMenu = CreateWidget<UMessageHoverMenuWidget>(this, MouseHoverMenuWidgetClass);
        MouseHoverMenu->Setup(Message, Side);
        HoverMenuTargetPanel->SetContent(MouseHoverMenu);
    }
    Super::NativeOnMouseEnter(InGeometry, InMouseEvent);
}

void UMessageWidget::NativeOnMouseLeave(const FPointerEvent& InMouseEvent)
{
    if (FSlateApplication::Get().AnyMenusVisible())
    {
        return;
    }

    if (MouseHoverMenu)
    {
        MouseHoverMenu->RemoveFromParent();
        MouseHoverMenu = nullptr;
    }
    Super::NativeOnMouseLeave(InMouseEvent);
}

bool UMessageWidget::ShouldDisplayHoverMenu() const
{
    return HoverMenuTargetPanel && Message.Type != EMessageType::Deleted;
}

void UMessageWidget::CreateAttachmentWidgets()
{
    if (!AlignPanel)
    {
        return;
    }

    for (UAttachmentWidget* Previous : Attachments)
    {
        AlignPanel->RemoveChild(Previous);
    }
    Attachments.Reset();

    if (Message.Type == EMessageType::Deleted)
    {
        return;
    }

    // WBP_Message has no slot for these, so they go straight into the panel that handles the
    // left/right alignment, ahead of the bubble. The loop below then aligns them like everything
    // else in there.
    int32 Index = 0;
    for (const FAttachment& Attachment : Message.Attachments)
    {
        UAttachmentWidget* Widget = CreateWidget<UAttachmentWidget>(this, AttachmentWidgetClass);
        Widget->Setup(Attachment, Side);

        if (UVerticalBoxSlot* BoxSlot = Cast<UVerticalBoxSlot>(AlignPanel->InsertChildAt(Index++, Widget)))
        {
            BoxSlot->SetPadding(AttachmentPadding);
        }
        Attachments.Add(Widget);
    }
}

bool UMessageWidget::ShouldDisplayThreadFooter() const
{
    // Only a count, and only when there is something to count. Starting a thread is a message action,
    // reached by long pressing the message, so no affordance is needed under messages without one.
    if (Message.ReplyCount <= 0 || Message.Type == EMessageType::Deleted || Message.Id.IsEmpty())
    {
        return false;
    }

    // Threads do not nest, so a reply gets no footer of its own. Neither does anything shown while a
    // thread is already open, which includes the parent at the top of that thread.
    if (Message.IsThreadReply())
    {
        return false;
    }

    const UChannelContextWidget* Context = UChannelContextWidget::TryGet(this);
    return Context && !Context->IsThreadOpen();
}

FText UMessageWidget::GetThreadFooterText() const
{
    if (Message.ReplyCount == 1)
    {
        return OneReplyText;
    }
    return FText::Format(ManyRepliesFormat, FText::AsNumber(Message.ReplyCount));
}

void UMessageWidget::CreateThreadFooter()
{
    if (!AlignPanel || !WidgetTree)
    {
        return;
    }

    if (ThreadFooterButton)
    {
        AlignPanel->RemoveChild(ThreadFooterButton);
        ThreadFooterButton = nullptr;
        ThreadFooterText = nullptr;
    }

    if (!ShouldDisplayThreadFooter())
    {
        return;
    }

    ThreadFooterText = WidgetTree->ConstructWidget<UTextBlock>();
    ThreadFooterText->SetText(GetThreadFooterText());
    FSlateFontInfo Font = ThreadFooterText->GetFont();
    Font.Size = ThreadFooterFontSize;
    ThreadFooterText->SetFont(Font);
    if (const UThemeDataAsset* ThemeAsset = GetTheme())
    {
        ThreadFooterText->SetColorAndOpacity(ThemeAsset->GetPaletteColor(ThemeAsset->ThreadFooterTextColor));
    }

    ThreadFooterButton = WidgetTree->ConstructWidget<UButton>();
    ThreadFooterButton->OnClicked.AddDynamic(this, &UMessageWidget::OnThreadFooterClicked);

    // Flat, like the composer's attach button: the engine's default button background looks nothing
    // like the rest of a message, and its padding would push the label away from the bubble
    FButtonStyle Style = ThreadFooterButton->GetStyle();
    for (FSlateBrush* Brush : {&Style.Normal, &Style.Hovered, &Style.Pressed, &Style.Disabled})
    {
        Brush->DrawAs = ESlateBrushDrawType::NoDrawType;
    }
    Style.NormalPadding = FMargin{0.f};
    Style.PressedPadding = FMargin{0.f};
    ThreadFooterButton->SetStyle(Style);
    ThreadFooterButton->SetContent(ThreadFooterText);

    // Last in the panel, so it sits below the bubble. OnSetup's alignment pass has already been and
    // gone by now, so this slot is aligned to the message's side here instead.
    if (UVerticalBoxSlot* BoxSlot = Cast<UVerticalBoxSlot>(AlignPanel->AddChild(ThreadFooterButton)))
    {
        BoxSlot->SetPadding(ThreadFooterPadding);
        BoxSlot->SetHorizontalAlignment(Side == EMessageSide::Me ? HAlign_Right : HAlign_Left);
    }
}

void UMessageWidget::OnThreadFooterClicked()
{
    if (UChannelContextWidget* Context = UChannelContextWidget::TryGet(this))
    {
        Context->OpenThread(Message);
    }
}

bool UMessageWidget::ShouldDisplayModerationWarning() const
{
    // IsModerationError already restricts this to the current user's own messages, which is the only
    // place a bounce can show up: nobody else is ever sent one.
    return Message.IsModerationError();
}

void UMessageWidget::CreateModerationWarning()
{
    if (!AlignPanel || !WidgetTree)
    {
        return;
    }

    if (ModerationWarningText)
    {
        AlignPanel->RemoveChild(ModerationWarningText);
        ModerationWarningText = nullptr;
    }

    if (!ShouldDisplayModerationWarning())
    {
        return;
    }

    ModerationWarningText = WidgetTree->ConstructWidget<UTextBlock>();
    ModerationWarningText->SetText(BouncedMessageText);
    FSlateFontInfo Font = ModerationWarningText->GetFont();
    Font.Size = ModerationWarningFontSize;
    ModerationWarningText->SetFont(Font);
    if (const UThemeDataAsset* ThemeAsset = GetTheme())
    {
        ModerationWarningText->SetColorAndOpacity(ThemeAsset->GetPaletteColor(ThemeAsset->ModerationWarningTextColor));
    }

    // Last in the panel, so it sits below the bubble. OnSetup's alignment pass has already been and
    // gone by now, so this slot is aligned to the message's side here instead.
    if (UVerticalBoxSlot* BoxSlot = Cast<UVerticalBoxSlot>(AlignPanel->AddChild(ModerationWarningText)))
    {
        BoxSlot->SetPadding(ModerationWarningPadding);
        BoxSlot->SetHorizontalAlignment(Side == EMessageSide::Me ? HAlign_Right : HAlign_Left);
    }
}
