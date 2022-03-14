// Copyright 2022 Stream.IO, Inc. All Rights Reserved.

#include "Context/ContextWidget.h"

#include "WidgetUtil.h"
#include "Widgets/DeclarativeSyntaxSupport.h"
#include "Widgets/Layout/SBox.h"
#include "Widgets/SNullWidget.h"
#include "Widgets/Text/STextBlock.h"

UContextWidget::UContextWidget()
{
    bIsVariable = true;
    Visibility = ESlateVisibility::SelfHitTestInvisible;
}

void UContextWidget::ReleaseSlateResources(bool bReleaseChildren)
{
    Super::ReleaseSlateResources(bReleaseChildren);

    MyBox.Reset();
}

TSharedRef<SWidget> UContextWidget::RebuildWidget()
{
    MyBox = SNew(SBox);

    if (IsDesignTime())
    {
        MyBox->SetContent(SNew(SBox).HAlign(HAlign_Center).VAlign(VAlign_Center)[SNew(STextBlock).Text(FText::FromName(GetFName()))]);
    }

    // Add any existing content to the new slate box
    if (GetChildrenCount() > 0)
    {
        const UPanelSlot* ContentSlot = GetContentSlot();
        if (ContentSlot->Content)
        {
            MyBox->SetContent(ContentSlot->Content->TakeWidget());
        }
    }

    return MyBox.ToSharedRef();
}

void UContextWidget::OnSlotAdded(UPanelSlot* InSlot)
{
    // Add the child to the live slot if it already exists
    if (MyBox.IsValid() && InSlot->Content)
    {
        MyBox->SetContent(InSlot->Content->TakeWidget());
    }
}

void UContextWidget::OnSlotRemoved(UPanelSlot* InSlot)
{
    // Remove the widget from the live slot if it exists.
    if (MyBox.IsValid())
    {
        MyBox->SetContent(SNullWidget::NullWidget);

        if (IsDesignTime())
        {
            MyBox->SetContent(
                // clang-format off
				SNew(SBox)
				.HAlign(HAlign_Center)
				.VAlign(VAlign_Center)
				[
					SNew(STextBlock)
					.Text(FText::FromName(GetFName()))
				]
                // clang-format on
            );
        }
    }
}

#if WITH_EDITOR

const FText UContextWidget::GetPaletteCategory()
{
    return FText::FromString(TEXT("Common"));
}

#endif
