#include "Reaction/BottomReactionWidget.h"

void UBottomReactionWidget::Setup(const FReactionGroup& InReactionGroup)
{
    ReactionGroup = InReactionGroup;

    Super::Setup();
}

void UBottomReactionWidget::OnSetup()
{
    if (ReactionIcon)
    {
        ReactionIcon->Setup(ReactionGroup.Type, GetSide());
    }

    if (ReactionCountTextBlock)
    {
        ReactionCountTextBlock->SetText(FText::AsNumber(ReactionGroup.Count));
    }
}

EMessageSide UBottomReactionWidget::GetSide() const
{
    if (ReactionGroup.HasOwnReaction())
    {
        return EMessageSide::Me;
    }
    return EMessageSide::You;
}
