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
    if (PaginationRequestState == EHttpRequestState::Started)
    {
        return;
    }

    const EPaginationDirection Direction = GetDirection();
    if (Direction == EPaginationDirection::None)
    {
        return;
    }

    const bool bPaginationEnded = EnumHasAnyFlags(EndedPaginationDirections, Direction);
    if (bPaginationEnded)
    {
        return;
    }

    SetPaginationRequestState(EHttpRequestState::Started, Direction);

    const int32 OrigWidgetCount = ScrollBox->GetChildrenCount();
    Paginate(
        Direction,
        [WeakThis = TWeakObjectPtr<UPaginateScrollWidget>(this), Direction, OrigWidgetCount]
        {
            if (!WeakThis.IsValid())
            {
                return;
            }
            const int32 DeltaMessageCount = WeakThis->ScrollBox->GetChildrenCount() - OrigWidgetCount;
            if (DeltaMessageCount == 0 || DeltaMessageCount < WeakThis->Limit)
            {
                // Don't need to paginate again in this direction in the future
                WeakThis->EndedPaginationDirections |= Direction;
            }

            WeakThis->SetPaginationRequestState(EHttpRequestState::Ended, Direction);
        });
}

void UPaginateScrollWidget::SetPaginationRequestState(const EHttpRequestState RequestState, const EPaginationDirection Direction)
{
    PaginationRequestState = RequestState;
    OnPaginatingMessages.Broadcast(Direction, RequestState);
}

EPaginationDirection UPaginateScrollWidget::GetDirection() const
{
    if (ScrollBox)
    {
        if (ScrollBox->GetScrollOffset() < PaginateScrollThreshold)
        {
            return EPaginationDirection::Top;
        }
        if (ScrollBox->GetScrollOffsetOfEnd() < PaginateScrollThreshold)
        {
            return EPaginationDirection::Bottom;
        }
    }
    return EPaginationDirection::None;
}
