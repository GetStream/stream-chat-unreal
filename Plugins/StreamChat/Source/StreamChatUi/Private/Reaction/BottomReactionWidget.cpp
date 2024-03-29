// Copyright 2022 Stream.IO, Inc. All Rights Reserved.

#include "Reaction/BottomReactionWidget.h"

#include "Reaction/ReactionsTooltipWidget.h"
#include "ThemeDataAsset.h"
#include "TimerManager.h"
#include "User/UserManager.h"
#include "WidgetUtil.h"

UBottomReactionWidget::UBottomReactionWidget()
{
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

    if (Anchor)
    {
        Anchor->Placement = MenuPlacement_CenteredBelowAnchor;
    }
}

void UBottomReactionWidget::NativePreConstruct()
{
    Super::NativePreConstruct();
    if (Border)
    {
        Border->SetBrushColor(GetTheme()->GetPaletteColor(GetTheme()->BottomReactionBorderColor));
    }
    if (Button)
    {
        Button->WidgetStyle.Normal.DrawAs = ESlateBrushDrawType::Box;
        Button->WidgetStyle.Pressed.DrawAs = ESlateBrushDrawType::Box;
        Button->WidgetStyle.Hovered.DrawAs = ESlateBrushDrawType::Box;

        const EMessageSide Side = GetSide();
        if (Side == EMessageSide::Me)
        {
            Button->WidgetStyle.Normal.TintColor = FSlateColor{GetTheme()->GetPaletteColor(GetTheme()->MeBottomReactionColor)};
            Button->WidgetStyle.Pressed.TintColor = FSlateColor{GetTheme()->GetPaletteColor(GetTheme()->MeBottomReactionColor)};
            Button->WidgetStyle.Hovered.TintColor = FSlateColor{GetTheme()->GetPaletteColor(GetTheme()->MeBottomReactionColor)};
        }
        else if (Side == EMessageSide::You)
        {
            Button->WidgetStyle.Normal.TintColor = FSlateColor{GetTheme()->GetPaletteColor(GetTheme()->YouBottomReactionColor)};
            Button->WidgetStyle.Pressed.TintColor = FSlateColor{GetTheme()->GetPaletteColor(GetTheme()->YouBottomReactionColor)};
            Button->WidgetStyle.Hovered.TintColor = FSlateColor{GetTheme()->GetPaletteColor(GetTheme()->YouBottomReactionColor)};
        }
    }
    if (ReactionCountTextBlock)
    {
        ReactionCountTextBlock->SetColorAndOpacity(GetTheme()->GetPaletteColor(GetTheme()->BottomReactionTextColor));
    }
}

void UBottomReactionWidget::NativeConstruct()
{
    Super::NativeConstruct();
    if (Anchor)
    {
        Anchor->OnGetUserMenuContentEvent.BindDynamic(this, &UBottomReactionWidget::CreateTooltip);
    }
    if (Button)
    {
        Button->OnClicked.AddDynamic(this, &UBottomReactionWidget::OnButtonClicked);
    }
}

void UBottomReactionWidget::NativeDestruct()
{
    CancelTooltip();

    if (Button)
    {
        Button->OnClicked.RemoveDynamic(this, &UBottomReactionWidget::OnButtonClicked);
    }

    Super::NativeDestruct();
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
            {
                WeakThis->Anchor->Open(true);
                WidgetUtil::HideDefaultMenuBackground(WeakThis->Anchor);
            }
        },
        0.5f,
        false);

    Super::NativeOnMouseEnter(InGeometry, InMouseEvent);
}

void UBottomReactionWidget::NativeOnMouseLeave(const FPointerEvent& InMouseEvent)
{
    CancelTooltip();

    Super::NativeOnMouseLeave(InMouseEvent);
}

EMessageSide UBottomReactionWidget::GetSide() const
{
    if (ReactionGroup.GetOwnReaction(UUserManager::Get()).IsSet())
    {
        return EMessageSide::Me;
    }
    return EMessageSide::You;
}

void UBottomReactionWidget::CancelTooltip()
{
    if (HoverTimerHandle.IsValid())
    {
        GetWorld()->GetTimerManager().ClearTimer(HoverTimerHandle);
    }

    if (Anchor && Anchor->IsOpen())
    {
        Anchor->Close();
    }
}

void UBottomReactionWidget::OnButtonClicked()
{
    OnBottomReactionClickedNative.Broadcast(ReactionGroup);
    OnBottomReactionClicked.Broadcast(ReactionGroup);
}

UUserWidget* UBottomReactionWidget::CreateTooltip()
{
    UReactionsTooltipWidget* Widget = CreateWidget<UReactionsTooltipWidget>(this, ReactionsTooltipWidgetClass);
    Widget->Setup(ReactionGroup);
    return Widget;
}
