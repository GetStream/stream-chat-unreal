﻿#include "Message/MessageWidget.h"

#include "Components/HorizontalBoxSlot.h"
#include "Components/OverlaySlot.h"

UMessageWidget::UMessageWidget(const FObjectInitializer& ObjectInitializer) : UUserWidget(ObjectInitializer)
{
    // Ensure hovering events are fired
    UUserWidget::SetVisibility(ESlateVisibility::Visible);
}

bool UMessageWidget::Initialize()
{
    if (Super::Initialize())
    {
        if (IsDesignTime())
        {
            SetupChildren();
        }
        return true;
    }

    return false;
}

void UMessageWidget::Setup(
    const FMessage& InMessage,
    const EBubbleStackSide InSide,
    const EBubbleStackPosition InPosition)
{
    Message = InMessage;
    Side = InSide;
    Position = InPosition;

    SetupChildren();
}

void UMessageWidget::SetupChildren() const
{
    // Changing the order of elements in a panel doesn't seem to work in PreConstruct.
    if (UPanelWidget* HoverMenuParent = Cast<UPanelWidget>(HoverMenuTargetPanel->GetParent()))
    {
        if (Side == EBubbleStackSide::Me)
        {
            HoverMenuParent->ReplaceChildAt(0, HoverMenuTargetPanel);
            HoverMenuParent->ReplaceChildAt(1, TextBubble);
        }
        else if (Side == EBubbleStackSide::You)
        {
            HoverMenuParent->ReplaceChildAt(0, TextBubble);
            HoverMenuParent->ReplaceChildAt(1, HoverMenuTargetPanel);
        }
    }

    for (UPanelSlot* PanelSlot : OuterOverlay->GetSlots())
    {
        UOverlaySlot* OverlaySlot = CastChecked<UOverlaySlot>(PanelSlot);
        if (Side == EBubbleStackSide::Me)
        {
            OverlaySlot->SetHorizontalAlignment(HAlign_Right);
        }
        else if (Side == EBubbleStackSide::You)
        {
            OverlaySlot->SetHorizontalAlignment(HAlign_Left);
        }
    }

    if (TextBubble)
    {
        TextBubble->Setup(Message, Side, Position);
    }
}

void UMessageWidget::NativeOnMouseEnter(const FGeometry& InGeometry, const FPointerEvent& InMouseEvent)
{
    MouseHoverMenu = CreateWidget<UMessageHoverMenuWidget>(this, MouseHoverMenuWidgetClass);
    MouseHoverMenu->Setup(Message, Side);
    HoverMenuTargetPanel->SetContent(MouseHoverMenu);
    Super::NativeOnMouseEnter(InGeometry, InMouseEvent);
}

void UMessageWidget::NativeOnMouseLeave(const FPointerEvent& InMouseEvent)
{
    MouseHoverMenu->RemoveFromParent();
    MouseHoverMenu = nullptr;
    Super::NativeOnMouseLeave(InMouseEvent);
}