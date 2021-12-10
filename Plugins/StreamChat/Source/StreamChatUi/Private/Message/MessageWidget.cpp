// Copyright 2021 Stream.IO, Inc. All Rights Reserved.

#include "Message/MessageWidget.h"

#include "Blueprint/WidgetTree.h"
#include "Components/GridSlot.h"
#include "Components/HorizontalBoxSlot.h"
#include "Components/OverlaySlot.h"
#include "Components/Spacer.h"
#include "Components/VerticalBoxSlot.h"

UMessageWidget::UMessageWidget(const FObjectInitializer& ObjectInitializer) : Super(ObjectInitializer)
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
        // TODO replacing children can be buggy. Should use Grid.
        if (UPanelWidget* HoverMenuParent = Cast<UPanelWidget>(HoverMenuTargetPanel->GetParent()))
        {
            if (Side == EMessageSide::Me)
            {
                HoverMenuParent->ReplaceChildAt(0, HoverMenuTargetPanel);
                HoverMenuParent->ReplaceChildAt(1, TextBubble);
            }
            else if (Side == EMessageSide::You)
            {
                HoverMenuParent->ReplaceChildAt(0, TextBubble);
                HoverMenuParent->ReplaceChildAt(1, HoverMenuTargetPanel);
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
    return Message.Type != EMessageType::Deleted;
}
