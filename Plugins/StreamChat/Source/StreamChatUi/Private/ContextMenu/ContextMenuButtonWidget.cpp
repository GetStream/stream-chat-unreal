#include "ContextMenu/ContextMenuButtonWidget.h"

#include "Brushes/SlateImageBrush.h"

void UContextMenuButtonWidget::NativeOnInitialized()
{
    Button->OnClicked.AddDynamic(this, &UContextMenuButtonWidget::OnButtonClicked);

    Super::NativeOnInitialized();
}

void UContextMenuButtonWidget::Setup(EContextMenuButtonPosition InPosition)
{
    Position = InPosition;

    UTexture2D* Texture = GetButtonTexture();
    const FSlateImageBrush Brush = FSlateImageBrush(Texture, Texture->GetImportedSize());
    Button->WidgetStyle.SetNormal(Brush);
    Button->WidgetStyle.SetHovered(Brush);
    Button->WidgetStyle.SetPressed(Brush);

    Button->WidgetStyle.Normal.TintColor = DefaultButtonColor;
    Button->WidgetStyle.Hovered.TintColor = DefaultButtonColor;
    Button->WidgetStyle.Pressed.TintColor = SelectedButtonColor;
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
