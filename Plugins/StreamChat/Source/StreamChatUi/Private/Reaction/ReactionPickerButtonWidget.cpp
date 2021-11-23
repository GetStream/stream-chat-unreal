// Copyright Stream.IO, Inc. All Rights Reserved.

#include "Reaction/ReactionPickerButtonWidget.h"

void UReactionPickerButtonWidget::Setup(const FName& InReactionType, const EMessageSide InSide)
{
    ReactionType = InReactionType;
    Side = InSide;

    Super::Setup();
}

void UReactionPickerButtonWidget::OnSetup()
{
    if (Button)
    {
        Button->OnClicked.AddUniqueDynamic(this, &UReactionPickerButtonWidget::OnButtonClicked);
    }
    if (Icon)
    {
        Icon->Setup(ReactionType, Side);
    }
}

void UReactionPickerButtonWidget::OnButtonClicked()
{
    OnReactionPickerButtonClickedNative.Broadcast(ReactionType);
    OnReactionPickerButtonClicked.Broadcast(ReactionType);
}
