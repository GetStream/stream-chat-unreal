// Copyright 2021 Stream.IO, Inc. All Rights Reserved.

#include "Common/PaginateScrollWidget.h"

#include "UiBlueprintLibrary.h"

void UPaginateScrollWidget::NativeOnInitialized()
{
    Super::NativeOnInitialized();
    if (ScrollBox)
    {
        ScrollBox->OnUserScrolled.AddDynamic(this, &UPaginateScrollWidget::OnUserScroll);
    }
}

void UPaginateScrollWidget::OnPreConstruct()
{
    // Check if we need to already paginate content
    OnUserScroll(ScrollBox->GetScrollOffset());
}

void UPaginateScrollWidget::SetChildren(const TArray<UWidget*>& Children)
{
    if (!ScrollBox)
    {
        return;
    }

    int32 FirstIndex;
    float FirstLeadingEdge;
    UUiBlueprintLibrary::GetFirstVisibleChildOfScrollBox(ScrollBox, FirstIndex, FirstLeadingEdge);
    const int32 StartListLength = ScrollBox->GetChildrenCount();

    ScrollBox->ClearChildren();
    for (UWidget* Child : Children)
    {
        ScrollBox->AddChild(Child);
    }

    // Try to maintain the position of all children
    if (FirstIndex == INDEX_NONE)
    {
        ScrollBox->ScrollToEnd();
    }
    else
    {
        const int32 Index = ScrollBox->GetChildrenCount() - StartListLength + FirstIndex;
        UWidget* Widget = ScrollBox->GetChildAt(Index);
        ScrollBox->ScrollWidgetIntoView(Widget, false, EDescendantScrollDestination::TopOrLeft, FirstLeadingEdge);
    }
}

void UPaginateScrollWidget::OnUserScroll(const float CurrentOffset)
{
    if (CurrentOffset < PaginateScrollThreshold)
    {
        Paginate();
    }
}
