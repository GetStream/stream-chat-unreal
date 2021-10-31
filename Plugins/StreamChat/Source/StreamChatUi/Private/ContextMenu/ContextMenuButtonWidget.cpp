#include "ContextMenu/ContextMenuButtonWidget.h"

#include "Brushes/SlateBoxBrush.h"
#include "Engine/Texture2D.h"

void UContextMenuButtonWidget::Setup(const EContextMenuButtonPosition InPosition)
{
    Position = InPosition;

    OnSetup();
}

void UContextMenuButtonWidget::OnSetup()
{
    if (Position == EContextMenuButtonPosition::Top)
    {
        TopBorderImage->SetVisibility(ESlateVisibility::Collapsed);
    }
    else
    {
        TopBorderImage->SetVisibility(ESlateVisibility::Visible);
    }

    UTexture2D* Texture = GetButtonTexture();
    const FMargin Margin = GetButtonMargin();
    const FSlateBoxBrush NormalBrush = FSlateBoxBrush(Texture, Margin, DefaultButtonColor);
    const FSlateBoxBrush SelectedBrush = FSlateBoxBrush(Texture, Margin, SelectedButtonColor);
    Button->WidgetStyle.SetNormal(NormalBrush);
    Button->WidgetStyle.SetHovered(NormalBrush);
    Button->WidgetStyle.SetPressed(SelectedBrush);
    Button->WidgetStyle.NormalPadding = {};
    Button->WidgetStyle.PressedPadding = {};

    Button->OnClicked.AddDynamic(this, &UContextMenuButtonWidget::OnButtonClicked);
}

void UContextMenuButtonWidget::OnButtonClicked()
{
    OnContextMenuButtonClicked.Broadcast();
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
