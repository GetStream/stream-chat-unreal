// Copyright Stream.IO, Inc. All Rights Reserved.

#include "Reaction/MessageReactionsWidget.h"

void UMessageReactionsWidget::Setup(const FMessage& InMessage, EMessageSide InSide)
{
    ensureMsgf(InMessage.Reactions.IsEmpty(), TEXT("Creating ReactionsWidget from message with no reactions"));

    Message = InMessage;
    Side = InSide;

    Super::Setup();
}

void UMessageReactionsWidget::OnSetup(){CreateWidget<UReactionsBubbleWidget>(this, )}

TSubclassOf<UReactionsBubbleWidget> UMessageReactionsWidget::GetReactionsBubbleWidgetClass() const
{
    if (Side == EMessageSide::Me)
    {
        if (Message.Reactions.ReactionGroups.Num())
        {
        }
    }
    else if (Side == EMessageSide::You)
    {
    }
}
