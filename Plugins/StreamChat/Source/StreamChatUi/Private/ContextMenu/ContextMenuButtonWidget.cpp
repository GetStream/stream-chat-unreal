// Copyright 2022 Stream.IO, Inc. All Rights Reserved.

#include "ContextMenu/ContextMenuButtonWidget.h"

#include "Brushes/SlateBoxBrush.h"
#include "Engine/Texture2D.h"
#include "ThemeDataAsset.h"

UContextMenuButtonWidget::UContextMenuButtonWidget()
{
    bWantsTheme = true;
}

void UContextMenuButtonWidget::Setup(const FMessage& InMessage, const EContextMenuButtonPosition InPosition, UContextMenuAction* InAction)
{
    Message = InMessage;
    Position = InPosition;
    Action = InAction;

    Super::Setup();
}

void UContextMenuButtonWidget::OnSetup()
{
    if (TopBorderImage)
    {
        if (Position == EContextMenuButtonPosition::Top)
        {
            TopBorderImage->SetVisibility(ESlateVisibility::Collapsed);
        }
        else
        {
            TopBorderImage->SetVisibility(ESlateVisibility::Visible);
        }
    }

    if (Button)
    {
        Button->OnClicked.AddUniqueDynamic(this, &UContextMenuButtonWidget::OnButtonClicked);
    }

    if (IconImage)
    {
        if (Action)
        {
            IconImage->SetBrushFromTexture(Action->IconTexture, true);
        }
    }

    if (TextBlock)
    {
        if (Action)
        {
            TextBlock->SetText(Action->Label);
        }
    }
}

void UContextMenuButtonWidget::OnTheme()
{
    if (TopBorderImage)
    {
        TopBorderImage->SetColorAndOpacity(Theme->GetPaletteColor(Theme->ContextMenuBorderColor));
    }
    if (Button)
    {
        UTexture2D* Texture = GetButtonTexture();
        const FMargin Margin = GetButtonMargin();
        const FSlateBoxBrush NormalBrush = FSlateBoxBrush(Texture, Margin, Theme->GetPaletteColor(Theme->ContextMenuDefaultButtonColor));
        const FSlateBoxBrush SelectedBrush = FSlateBoxBrush(Texture, Margin, Theme->GetPaletteColor(Theme->ContextMenuPressedButtonColor));
        Button->WidgetStyle.SetNormal(NormalBrush);
        Button->WidgetStyle.SetHovered(NormalBrush);
        Button->WidgetStyle.SetPressed(SelectedBrush);
        Button->WidgetStyle.NormalPadding = {};
        Button->WidgetStyle.PressedPadding = {};
    }

    if (IconImage)
    {
        IconImage->SetColorAndOpacity(GetIconColor());
    }

    if (TextBlock)
    {
        TextBlock->SetColorAndOpacity(GetTextColor());
    }
}

void UContextMenuButtonWidget::OnButtonClicked()
{
    if (Action)
    {
        Action->Perform(Message, this);
    }
}

UTexture2D* UContextMenuButtonWidget::GetButtonTexture() const
{
    switch (Position)
    {
        case EContextMenuButtonPosition::Top:
            return TopButtonTexture;
        case EContextMenuButtonPosition::Mid:
            return MidButtonTexture;
        case EContextMenuButtonPosition::Bottom:
            return BottomButtonTexture;
    }
    return nullptr;
}

FMargin UContextMenuButtonWidget::GetButtonMargin() const
{
    switch (Position)
    {
        case EContextMenuButtonPosition::Top:
            return {0.5f, 1.f, 0.5, 0.f};
        case EContextMenuButtonPosition::Mid:
            return {0.5f};
        case EContextMenuButtonPosition::Bottom:
            return {0.5f, 0.f, 0.5, 1.f};
    }
    return {};
}

const FLinearColor& UContextMenuButtonWidget::GetIconColor() const
{
    if (Action)
    {
        switch (Action->Style)
        {
            case EContextMenuButtonStyle::Standard:
                return Theme->GetPaletteColor(Theme->ContextMenuDefaultIconColor);
            case EContextMenuButtonStyle::Negative:
                return Theme->GetPaletteColor(Theme->ContextMenuNegativeIconColor);
        }
    }
    return FLinearColor::Red;
}

const FLinearColor& UContextMenuButtonWidget::GetTextColor() const
{
    if (Action)
    {
        switch (Action->Style)
        {
            case EContextMenuButtonStyle::Standard:
                return Theme->GetPaletteColor(Theme->ContextMenuDefaultTextColor);
            case EContextMenuButtonStyle::Negative:
                return Theme->GetPaletteColor(Theme->ContextMenuNegativeTextColor);
        }
    }
    return FLinearColor::Red;
}
