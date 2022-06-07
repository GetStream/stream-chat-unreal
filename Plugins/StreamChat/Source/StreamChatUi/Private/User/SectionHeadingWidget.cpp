#include "User/SectionHeadingWidget.h"

#include "ThemeDataAsset.h"

USectionHeadingWidget::USectionHeadingWidget()
{
}

void USectionHeadingWidget::Setup(const FText& InLabel)
{
    Label = InLabel;

    Super::Setup();
}

void USectionHeadingWidget::OnSetup()
{
    if (TextBlock)
    {
        TextBlock->SetText(Label);
    }
}

void USectionHeadingWidget::NativePreConstruct()
{
    Super::NativePreConstruct();
    if (TextBlock)
    {
        TextBlock->SetColorAndOpacity(GetTheme()->GetPaletteColor(GetTheme()->SectionHeadingTextColor));
    }
    if (Border)
    {
        Border->SetBrushColor(GetTheme()->GetPaletteColor(GetTheme()->SectionHeadingBackgroundColor));
    }
}
