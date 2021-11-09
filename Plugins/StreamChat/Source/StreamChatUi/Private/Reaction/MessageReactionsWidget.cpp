// Copyright Stream.IO, Inc. All Rights Reserved.

#include "Reaction/MessageReactionsWidget.h"

void UMessageReactionsWidget::Setup(const FMessage& InMessage, const EMessageSide InSide)
{
    ensureMsgf(InMessage.Reactions.IsEmpty(), TEXT("Creating ReactionsWidget from message with no reactions"));

    Message = InMessage;
    Side = InSide;

    Super::Setup();
}

void UMessageReactionsWidget::OnSetup()
{
}
