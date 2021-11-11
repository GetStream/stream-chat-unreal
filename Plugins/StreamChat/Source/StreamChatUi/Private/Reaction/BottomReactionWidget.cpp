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

    if (Button)
    {
        if (const FButtonStyle* Style = ButtonStyles.Find(Side))
        {
            Button->SetStyle(*Style);
        }

        Button->OnClicked.AddUniqueDynamic(this, &UBottomReactionWidget::OnButtonClicked);
    }
}

EMessageSide UBottomReactionWidget::GetSide() const
{
    if (ReactionGroup.OwnReaction.IsSet())
    {
        return EMessageSide::Me;
    }
    return EMessageSide::You;
}

void UBottomReactionWidget::OnButtonClicked()
{
    OnBottomReactionClickedNative.Broadcast(ReactionGroup);
    OnBottomReactionClicked.Broadcast(ReactionGroup);
}
