#include "User/SectionHeadingWidget.h"

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
