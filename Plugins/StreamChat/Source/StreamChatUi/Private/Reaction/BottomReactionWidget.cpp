// Copyright 2022 Stream.IO, Inc. All Rights Reserved.

#include "Reaction/BottomReactionWidget.h"

#include "Reaction/ReactionsTooltipWidget.h"
#include "ThemeDataAsset.h"
#include "TimerManager.h"

UBottomReactionWidget::UBottomReactionWidget()
{
    bWantsTheme = true;
}

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
        Button->OnClicked.AddUniqueDynamic(this, &UBottomReactionWidget::OnButtonClicked);
    }

    if (Anchor)
    {
        Anchor->Placement = MenuPlacement_CenteredBelowAnchor;
        Anchor->OnGetUserMenuContentEvent.BindDynamic(this, &UBottomReactionWidget::CreateReactionsMenu);
    }
}

void UBottomReactionWidget::OnTheme()
{
    if (Border)
    {
        Border->SetBrushColor(Theme->GetPaletteColor(Theme->BottomReactionBorderColor));
    }
    if (Button)
    {
        const EMessageSide Side = GetSide();
        if (Side == EMessageSide::Me)
        {
            Button->WidgetStyle.Normal.TintColor = FSlateColor{Theme->GetPaletteColor(Theme->MeBottomReactionColor)};
            Button->WidgetStyle.Pressed.TintColor = FSlateColor{Theme->GetPaletteColor(Theme->MeBottomReactionColor)};
            Button->WidgetStyle.Hovered.TintColor = FSlateColor{Theme->GetPaletteColor(Theme->MeBottomReactionColor)};
        }
        else if (Side == EMessageSide::You)
        {
            Button->WidgetStyle.Normal.TintColor = FSlateColor{Theme->GetPaletteColor(Theme->YouBottomReactionColor)};
            Button->WidgetStyle.Pressed.TintColor = FSlateColor{Theme->GetPaletteColor(Theme->YouBottomReactionColor)};
            Button->WidgetStyle.Hovered.TintColor = FSlateColor{Theme->GetPaletteColor(Theme->YouBottomReactionColor)};
        }
    }
    if (ReactionCountTextBlock)
    {
        ReactionCountTextBlock->SetColorAndOpacity(Theme->GetPaletteColor(Theme->BottomReactionTextColor));
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
