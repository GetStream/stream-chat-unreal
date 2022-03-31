#include "User/SectionHeadingWidget.h"

#include "ThemeDataAsset.h"

USectionHeadingWidget::USectionHeadingWidget()
{
    bWantsTheme = true;
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

void USectionHeadingWidget::OnTheme()
{
    if (TextBlock)
    {
        TextBlock->SetColorAndOpacity(Theme->GetPaletteColor(Theme->SectionHeadingTextColor));
    }
    if (Border)
    {
        Border->SetBrushColor(Theme->GetPaletteColor(Theme->SectionHeadingBackgroundColor));
    }
}
