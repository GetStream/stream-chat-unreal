// Copyright 2021 Stream.IO, Inc. All Rights Reserved.

#include "Reaction/BottomReactionWidget.h"

#include "Reaction/ReactionsTooltipWidget.h"
#include "TimerManager.h"

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

    if (Anchor)
    {
        Anchor->Placement = MenuPlacement_CenteredBelowAnchor;
        Anchor->OnGetUserMenuContentEvent.BindDynamic(this, &UBottomReactionWidget::CreateReactionsMenu);
    }
}

void UBottomReactionWidget::NativeOnMouseEnter(const FGeometry& InGeometry, const FPointerEvent& InMouseEvent)
{
    if (HoverTimerHandle.IsValid())
    {
        GetWorld()->GetTimerManager().ClearTimer(HoverTimerHandle);
    }

    GetWorld()->GetTimerManager().SetTimer(
        HoverTimerHandle,
        [WeakThis = TWeakObjectPtr<UBottomReactionWidget>(this)]
        {
            if (WeakThis.IsValid())
                WeakThis->Anchor->Open(true);
        },
        1.f,
        false);

    Super::NativeOnMouseEnter(InGeometry, InMouseEvent);
}

void UBottomReactionWidget::NativeOnMouseLeave(const FPointerEvent& InMouseEvent)
{
    if (HoverTimerHandle.IsValid())
    {
        GetWorld()->GetTimerManager().ClearTimer(HoverTimerHandle);
    }

    if (Anchor && Anchor->IsOpen())
    {
        Anchor->Close();
    }

    Super::NativeOnMouseLeave(InMouseEvent);
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

UUserWidget* UBottomReactionWidget::CreateReactionsMenu()
{
    UReactionsTooltipWidget* Widget = CreateWidget<UReactionsTooltipWidget>(this, ReactionsTooltipWidgetClass);
    Widget->Setup(ReactionGroup);
    return Widget;
}
