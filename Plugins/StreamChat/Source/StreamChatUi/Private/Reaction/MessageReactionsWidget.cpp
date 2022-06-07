// Copyright 2022 Stream.IO, Inc. All Rights Reserved.

#include "Reaction/MessageReactionsWidget.h"

#include "Components/HorizontalBoxSlot.h"
#include "Reaction/BottomReactionWidget.h"
#include "User/UserManager.h"

UMessageReactionsWidget::UMessageReactionsWidget()
{
}

void UMessageReactionsWidget::Setup(const FMessage& InMessage, const EMessageSide InSide)
{
    ensureMsgf(!InMessage.Reactions.IsEmpty(), TEXT("Creating ReactionsWidget from message with no reactions"));

    Message = InMessage;
    Side = InSide;

    Super::Setup();
}

void UMessageReactionsWidget::NativeOnMouseEnter(const FGeometry& InGeometry, const FPointerEvent& InMouseEvent)
{
    if (!bFetchingReactions && !Message.Reactions.HasAllDataLocally() && GetChannel())
    {
        bFetchingReactions = true;
        GetChannel()->GetReactions(Message, {200, Message.Reactions.LocalCount() - 1}, [&](const TArray<FReaction>&) { bFetchingReactions = false; });
    }
    Super::NativeOnMouseEnter(InGeometry, InMouseEvent);
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
                    if (GetChannel())
                    {
                        const TOptional<FReaction> OwnReaction = Reaction.GetOwnReaction(UUserManager::Get());
                        if (OwnReaction.IsSet())
                        {
                            GetChannel()->DeleteReaction(Message, OwnReaction.GetValue());
                        }
                        else
                        {
                            GetChannel()->SendReaction(Message, Reaction.Type, false);
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
