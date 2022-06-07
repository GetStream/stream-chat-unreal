// Copyright 2022 Stream.IO, Inc. All Rights Reserved.

#include "Reaction/ReactionsTooltipWidget.h"

#include "ThemeDataAsset.h"

UReactionsTooltipWidget::UReactionsTooltipWidget()
{
}

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

        if (ReactionGroup.Count > ReactionGroup.LatestReactions.Num())
        {
            // TODO fetch more reactions if needed
            UE_LOG(
                LogTemp,
                Warning,
                TEXT("TODO: Implement fetching more reactions when many [Remote=%d, Local=%d]"),
                ReactionGroup.Count,
                ReactionGroup.LatestReactions.Num());
        }
        for (const FReaction& Reaction : ReactionGroup.LatestReactions)
        {
            UNamedAvatarWidget* Widget = CreateWidget<UNamedAvatarWidget>(this, NamedAvatarWidgetClass);
            Widget->Setup(Reaction.User);
            AvatarPanel->AddChild(Widget);
        }
    }
}

void UReactionsTooltipWidget::NativePreConstruct()
{
    Super::NativePreConstruct();

    if (BackgroundBorder)
    {
        BackgroundBorder->SetBrushColor(GetTheme()->GetPaletteColor(GetTheme()->ReactionsTooltipBackgroundColor));
    }
    if (IconBorder)
    {
        IconBorder->SetBrushColor(GetTheme()->GetPaletteColor(GetTheme()->ReactionsTooltipIconHighlightColor));
    }
}
