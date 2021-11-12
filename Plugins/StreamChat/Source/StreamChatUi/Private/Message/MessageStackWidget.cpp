#include "Message/MessageStackWidget.h"

#include "Components/PanelWidget.h"
#include "Components/VerticalBoxSlot.h"
#include "Message/MessageWidget.h"

void UMessageStackWidget::Setup(const TArray<FMessage>& InMessages, const EMessageSide InSide)
{
    Messages = InMessages;
    Side = InSide;

    Super::Setup();
}

FString UMessageStackWidget::Describe() const
{
    return FString::Printf(
        TEXT("MessageStack [Side=%s, MessageCount=%d, Message[0]=%s]"),
        *StaticEnum<EMessageSide>()->GetNameStringByValue(static_cast<int64>(Side)),
        Messages.Num(),
        *(Messages.Num() > 0 ? Messages[0].Text : TEXT("")));
}

void UMessageStackWidget::OnSetup()
{
    // Init timestamp widget
    if (Timestamp)
    {
        if (Messages.Num() > 0)
        {
            const bool bShowUserName = Side == EMessageSide::You;
            const bool bShowMessageState = Side == EMessageSide::Me;
            Timestamp->Setup(Messages.Last(), bShowUserName, bShowMessageState);
            if (UVerticalBoxSlot* VerticalBoxSlot = Cast<UVerticalBoxSlot>(Timestamp->Slot))
            {
                if (Side == EMessageSide::Me)
                {
                    VerticalBoxSlot->SetHorizontalAlignment(HAlign_Right);
                }
                else if (Side == EMessageSide::You)
                {
                    VerticalBoxSlot->SetHorizontalAlignment(HAlign_Left);
                }
            }
        }
    }

    if (AvatarSlot)
    {
        if (Messages.Num() > 0 && Side == EMessageSide::You)
        {
            UAvatarWidget* Widget = CreateWidget<UAvatarWidget>(this, AvatarWidgetClass);
            Widget->Setup({Messages[0].User}, AvatarSize);
            AvatarSlot->AddChild(Widget);
        }
    }

    // Spawn bubbles
    if (MessagesPanel)
    {
        MessagesPanel->ClearChildren();

        const int32 LastIndex = Messages.Num() - 1;
        for (int32 Index = 0; Index < Messages.Num(); ++Index)
        {
            const FMessage& Message = Messages[Index];
            UMessageWidget* Widget = CreateWidget<UMessageWidget>(this, MessageWidgetClass);
            Widget->Setup(
                Message, Side, Index == LastIndex ? EBubbleStackPosition::End : EBubbleStackPosition::Opening);
            MessagesPanel->AddChildToVerticalBox(Widget);
        }
    }
}
