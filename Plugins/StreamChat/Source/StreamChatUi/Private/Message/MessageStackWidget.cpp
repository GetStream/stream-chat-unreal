#include "Message/MessageStackWidget.h"

#include "Components/PanelWidget.h"
#include "Components/SizeBoxSlot.h"

void UMessageStackWidget::Setup(const TArray<FMessage>& InMessages, EBubbleStackSide InSide)
{
    Messages = InMessages;
    Side = InSide;

    SetupChildren();
}

bool UMessageStackWidget::Initialize()
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

void UMessageStackWidget::SetupChildren()
{
    // Init timestamp widget
    if (Messages.Num() > 0)
    {
        Timestamp->Setup(Messages.Last(), Side);
    }
}

void UMessageStackWidget::NativePreConstruct()
{
    // Spawn bubbles
    TextBubblePanel->ClearChildren();

    const int32 LastIndex = Messages.Num() - 1;
    for (int32 Index = 0; Index < Messages.Num(); ++Index)
    {
        const FMessage& Message = Messages[Index];
        UTextBubbleWidget* Widget = CreateWidget<UTextBubbleWidget>(this, TextBubbleWidgetClass);
        Widget->Message = Message;
        Widget->Side = Side;
        Widget->Position = Index == LastIndex ? EBubbleStackPosition::End : EBubbleStackPosition::Opening;
        TextBubblePanel->AddChild(Widget);
    }

    USizeBoxSlot* SizeBoxSlot = Cast<USizeBoxSlot>(SizeBox->GetContentSlot());
    switch (Side)
    {
        case EBubbleStackSide::You:
            SizeBoxSlot->SetHorizontalAlignment(HAlign_Left);
            break;
        case EBubbleStackSide::Me:
            SizeBoxSlot->SetHorizontalAlignment(HAlign_Right);
            break;
    }

    Super::NativePreConstruct();
}
