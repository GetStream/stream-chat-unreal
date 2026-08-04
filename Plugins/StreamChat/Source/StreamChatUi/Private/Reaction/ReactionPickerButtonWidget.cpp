// Copyright 2026 Stream.IO, Inc. All Rights Reserved.

#include "Reaction/ReactionPickerButtonWidget.h"

#include "ThemeDataAsset.h"

UReactionPickerButtonWidget::UReactionPickerButtonWidget()
{
}

void UReactionPickerButtonWidget::Setup(const FName& InReactionType, const EMessageSide InSide)
{
    ReactionType = InReactionType;
    Side = InSide;

    Super::Setup();
}

void UReactionPickerButtonWidget::OnSetup()
{
    if (Icon)
    {
        Icon->Setup(ReactionType, Side);
    }
}

void UReactionPickerButtonWidget::NativePreConstruct()
{
    Super::NativePreConstruct();
    if (Button)
    {
        FButtonStyle Style = Button->GetStyle();
        Style.Normal.DrawAs = ESlateBrushDrawType::NoDrawType;
        Style.Pressed.DrawAs = ESlateBrushDrawType::Box;
        Style.Hovered.DrawAs = ESlateBrushDrawType::Box;
        Style.Normal.TintColor = FSlateColor{GetTheme()->GetPaletteColor(GetTheme()->ReactionPickerBackgroundColor)};
        Style.Pressed.TintColor = FSlateColor{GetTheme()->GetPaletteColor(GetTheme()->ReactionPickerSelectedColor)};
        Style.Hovered.TintColor = FSlateColor{GetTheme()->GetPaletteColor(GetTheme()->ReactionPickerSelectedColor)};
        Button->SetStyle(Style);
    }
}

void UReactionPickerButtonWidget::NativeConstruct()
{
    Super::NativeConstruct();
    if (Button)
    {
        Button->OnClicked.AddDynamic(this, &UReactionPickerButtonWidget::OnButtonClicked);
    }
}

void UReactionPickerButtonWidget::NativeDestruct()
{
    if (Button)
    {
        Button->OnClicked.RemoveDynamic(this, &UReactionPickerButtonWidget::OnButtonClicked);
    }

    Super::NativeDestruct();
}

void UReactionPickerButtonWidget::OnButtonClicked()
{
    OnReactionPickerButtonClickedNative.Broadcast(ReactionType);
    OnReactionPickerButtonClicked.Broadcast(ReactionType);
}
