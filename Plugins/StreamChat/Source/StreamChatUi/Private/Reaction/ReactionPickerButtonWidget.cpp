// Copyright 2021 Stream.IO, Inc. All Rights Reserved.

#include "Reaction/ReactionPickerButtonWidget.h"

#include "ThemeDataAsset.h"

UReactionPickerButtonWidget::UReactionPickerButtonWidget()
{
    bWantsTheme = true;
}

void UReactionPickerButtonWidget::Setup(const FName& InReactionType, const EMessageSide InSide)
{
    ReactionType = InReactionType;
    Side = InSide;

    Super::Setup();
}

void UReactionPickerButtonWidget::OnSetup()
{
    if (Button)
    {
        Button->OnClicked.AddUniqueDynamic(this, &UReactionPickerButtonWidget::OnButtonClicked);
    }
    if (Icon)
    {
        Icon->Setup(ReactionType, Side);
    }
}

void UReactionPickerButtonWidget::OnTheme(const UThemeDataAsset* Theme)
{
    if (Button)
    {
        Button->WidgetStyle.Normal.TintColor = FSlateColor{Theme->GetPaletteColor(Theme->ReactionPickerBackgroundColor)};
        Button->WidgetStyle.Pressed.TintColor = FSlateColor{Theme->GetPaletteColor(Theme->ReactionPickerSelectedColor)};
        Button->WidgetStyle.Hovered.TintColor = FSlateColor{Theme->GetPaletteColor(Theme->ReactionPickerSelectedColor)};
    }
}

void UReactionPickerButtonWidget::OnButtonClicked()
{
    OnReactionPickerButtonClickedNative.Broadcast(ReactionType);
    OnReactionPickerButtonClicked.Broadcast(ReactionType);
}
