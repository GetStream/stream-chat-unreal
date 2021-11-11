#include "Reaction/ReactionPickerWidget.h"

void UReactionPickerWidget::Setup(const FMessage& InMessage)
{
    Message = InMessage;

    Super::Setup();
}

void UReactionPickerWidget::OnSetup()
{
    if (ButtonsPanel)
    {
        ButtonsPanel->ClearChildren();
        for (const FName& ReactionType : {TEXT("love"), TEXT("like"), TEXT("sad"), TEXT("haha"), TEXT("wow")})
        {
            UReactionPickerButtonWidget* Widget =
                CreateWidget<UReactionPickerButtonWidget>(this, ReactionPickerButtonWidgetClass);
            Widget->Setup(ReactionType, Message.User.IsCurrent() ? EMessageSide::Me : EMessageSide::You);
            ButtonsPanel->AddChild(Widget);
        }
    }
}
