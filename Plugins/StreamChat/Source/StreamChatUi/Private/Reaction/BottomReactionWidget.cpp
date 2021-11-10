#include "Reaction/BottomReactionWidget.h"

void UBottomReactionWidget::Setup(const FReactionGroup& InReactionGroup)
{
    ReactionGroup = InReactionGroup;

    Super::Setup();
}

void UBottomReactionWidget::OnSetup()
{
    const EMessageSide Side = GetSide();

    if (ReactionIcon)
    {
        ReactionIcon->Setup(ReactionGroup.Type, Side);
    }

    if (ReactionCountTextBlock)
    {
        ReactionCountTextBlock->SetText(FText::AsNumber(ReactionGroup.Count));
    }

    if (Border)
    {
        if (const FSlateBrush* Brush = Brushes.Find(Side))
        {
            Border->SetBrush(*Brush);
        }
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
