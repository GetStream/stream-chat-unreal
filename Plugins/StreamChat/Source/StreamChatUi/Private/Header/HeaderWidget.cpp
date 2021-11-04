// Copyright Stream.IO, Inc. All Rights Reserved.

#include "Header/HeaderWidget.h"

#include "Components/NamedSlot.h"

void UHeaderWidget::NativeOnInitialized()
{
    if (!SubtitleSlot->HasAnyChildren())
    {
        SubtitleSlot->SetVisibility(ESlateVisibility::Collapsed);
    }

    HeaderTitleTextBlock->SetText(Title);

    Super::NativeOnInitialized();
}

void UHeaderWidget::SetTitle(const FText& InTitle)
{
    Title = InTitle;
    HeaderTitleTextBlock->SetText(Title);
}
