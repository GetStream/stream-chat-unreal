// Copyright 2022 Stream.IO, Inc. All Rights Reserved.

#include "Reaction/MessageReactionsWidget.h"

#include "Components/HorizontalBoxSlot.h"
#include "Context/ChannelContextWidget.h"
#include "Reaction/BottomReactionWidget.h"

UMessageReactionsWidget::UMessageReactionsWidget()
{
    bWantsChannel = true;
}

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

        if (Message.Type == EMessageType::Deleted)
        {
            return;
        }

        for (const auto& Group : Message.Reactions.GetReactionGroups())
        {
            UBottomReactionWidget* Widget = CreateWidget<UBottomReactionWidget>(this, BottomReactionWidgetClass);
            Widget->Setup(Group.Value);
            Widget->OnBottomReactionClickedNative.AddLambda(
                [this](const FReactionGroup& Reaction)
                {
                    if (ensure(Channel))
                    {
                        if (Reaction.OwnReaction.IsSet())
                        {
                            Channel->DeleteReaction(Message, Reaction.OwnReaction.GetValue());
                        }
                        else
                        {
                            Channel->SendReaction(Message, Reaction.Type, false);
                        }
                    }
                });
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
