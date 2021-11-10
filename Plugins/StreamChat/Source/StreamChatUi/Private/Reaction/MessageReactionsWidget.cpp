// Copyright Stream.IO, Inc. All Rights Reserved.

#include "Reaction/MessageReactionsWidget.h"

#include "Components/HorizontalBoxSlot.h"
#include "Reaction/BottomReactionWidget.h"

void UMessageReactionsWidget::Setup(const FMessage& InMessage, const EMessageSide InSide)
{
    ensureMsgf(!InMessage.Reactions.IsEmpty(), TEXT("Creating ReactionsWidget from message with no reactions"));

    Message = InMessage;
    Side = InSide;

    Super::Setup();
}

void UMessageReactionsWidget::OnSetup()
{
    if (ReactionsPanel)
    {
        ReactionsPanel->ClearChildren();
        for (const auto& [Type, Group] : Message.Reactions.ReactionGroups)
        {
            UBottomReactionWidget* Widget = CreateWidget<UBottomReactionWidget>(this, BottomReactionWidgetClass);
            Widget->Setup(Group);
            ReactionsPanel->AddChildToHorizontalBox(Widget)->SetPadding(GetPadding());
        }
    }
}

FMargin UMessageReactionsWidget::GetPadding() const
{
    if (Side == EMessageSide::Me)
    {
        return {Spacing, 0.f, 0.f, 0.f};
    }
    return {0.f, 0.f, Spacing, 0.f};
}
