// Copyright Stream.IO, Inc. All Rights Reserved.

#include "Reaction/ReactionsTooltipWidget.h"

void UReactionsTooltipWidget::Setup(const FReactionGroup& InReactionGroup)
{
    ReactionGroup = InReactionGroup;

    Super::Setup();
}

void UReactionsTooltipWidget::OnSetup()
{
    if (ReactionIcon)
    {
        ReactionIcon->Setup(ReactionGroup.Type);
    }

    if (AvatarPanel)
    {
        AvatarPanel->ClearChildren();

        for (const FReaction& Reaction : ReactionGroup.LatestReactions)
        {
            UNamedAvatarWidget* Widget = CreateWidget<UNamedAvatarWidget>(this, NamedAvatarWidgetClass);
            Widget->Setup(Reaction.User);
            AvatarPanel->AddChild(Widget);
        }
    }
}
