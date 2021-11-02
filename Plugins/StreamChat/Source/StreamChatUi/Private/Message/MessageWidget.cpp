#include "Message/MessageWidget.h"

#include "Components/HorizontalBoxSlot.h"
#include "Components/OverlaySlot.h"

UMessageWidget::UMessageWidget(const FObjectInitializer& ObjectInitializer) : Super(ObjectInitializer)
{
    // Ensure hovering events are fired
    UUserWidget::SetVisibility(ESlateVisibility::Visible);
}

void UMessageWidget::Setup(const FMessage& InMessage, const EMessageSide InSide, const EBubbleStackPosition InPosition)
{
    Message = InMessage;
    Side = InSide;
    Position = InPosition;

    Super::Setup();
}

void UMessageWidget::OnSetup()
{
    // Changing the order of elements in a panel doesn't seem to work in PreConstruct.
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

    for (UPanelSlot* PanelSlot : OuterOverlay->GetSlots())
    {
        UOverlaySlot* OverlaySlot = CastChecked<UOverlaySlot>(PanelSlot);
        if (Side == EMessageSide::Me)
        {
            OverlaySlot->SetHorizontalAlignment(HAlign_Right);
        }
        else if (Side == EMessageSide::You)
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
