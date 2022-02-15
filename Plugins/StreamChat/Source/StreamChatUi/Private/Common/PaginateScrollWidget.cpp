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

    // Check if we need to already paginate content
    OnUserScroll(ScrollBox->GetScrollOffset());
}

void UPaginateScrollWidget::OnUserScroll(const float CurrentOffset)
{
    if (PaginationRequestState == EHttpRequestState::Started)
    {
        return;
    }

    const EPaginationDirection Directions = GetDirections();
    if (Directions == EPaginationDirection::None)
    {
        return;
    }

    const bool bPaginationEnded = EnumHasAllFlags(EndedPaginationDirections, Directions);
    if (bPaginationEnded)
    {
        return;
    }

    SetPaginationRequestState(EHttpRequestState::Started, Directions);

    const int32 OrigWidgetCount = ScrollBox->GetChildrenCount();
    Paginate(
        Directions,
        [WeakThis = TWeakObjectPtr<UPaginateScrollWidget>(this), Directions, OrigWidgetCount]
        {
            if (!WeakThis.IsValid())
            {
                return;
            }
            const int32 DeltaChildrenCount = WeakThis->ScrollBox->GetChildrenCount() - OrigWidgetCount;
            if (DeltaChildrenCount == 0 || DeltaChildrenCount < WeakThis->Limit)
            {
                // Don't need to paginate again in this direction in the future
                WeakThis->EndedPaginationDirections |= Directions;
            }

            WeakThis->SetPaginationRequestState(EHttpRequestState::Ended, Directions);
        });
}

void UPaginateScrollWidget::SetPaginationRequestState(const EHttpRequestState RequestState, const EPaginationDirection Direction)
{
    PaginationRequestState = RequestState;
    OnPaginatingMessages.Broadcast(Direction, RequestState);
}

EPaginationDirection UPaginateScrollWidget::GetDirections() const
{
    EPaginationDirection Directions = EPaginationDirection::None;
    if (ScrollBox)
    {
        if (ScrollBox->GetScrollOffset() < PaginateScrollThreshold)
        {
            Directions |= EPaginationDirection::Top;
        }
        if (ScrollBox->GetScrollOffsetOfEnd() < PaginateScrollThreshold)
        {
            Directions |= EPaginationDirection::Bottom;
        }
    }
    return Directions;
}
