// Copyright 2026 Stream.IO, Inc. All Rights Reserved.

#include "Message/MessageWidget.h"

#include "Blueprint/WidgetTree.h"
#include "Components/GridSlot.h"
#include "Components/HorizontalBoxSlot.h"
#include "Components/Spacer.h"
#include "Components/VerticalBoxSlot.h"
#include "Framework/Application/SlateApplication.h"

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
