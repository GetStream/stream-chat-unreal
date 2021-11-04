// Copyright Stream.IO, Inc. All Rights Reserved.


#include "Header/HeaderWidget.h"

#include "Components/NamedSlot.h"

void UHeaderWidget::NativeOnInitialized()
{
    if (!SubtitleSlot->HasAnyChildren())
    {
        SubtitleSlot->SetVisibility(ESlateVisibility::Collapsed);
    }

    HeaderTitleTextBlock->SetText(GetTitleText());

    Super::NativeOnInitialized();
}

FText UHeaderWidget::GetTitleText() const
{
    if (OnNativeGetTitle.IsBound())
    {
        return OnNativeGetTitle.Execute();
    }
    if (OnGetTitle.IsBound())
    {
        FText TitleText;
        OnGetTitle.Broadcast(TitleText);
        return TitleText;
    }

    return DefaultTitle;
}
