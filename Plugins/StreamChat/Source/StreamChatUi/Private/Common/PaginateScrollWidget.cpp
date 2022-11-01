// Copyright 2022 Stream.IO, Inc. All Rights Reserved.

#include "Common/PaginateScrollWidget.h"

#include "TimerManager.h"
#include "UiBlueprintLibrary.h"

void UPaginateScrollWidget::NativeConstruct()
{
    Super::NativeConstruct();

    if (ScrollBox)
    {
        ScrollBox->OnUserScrolled.AddDynamic(this, &UPaginateScrollWidget::OnUserScroll);
    }
}

void UPaginateScrollWidget::NativeDestruct()
{
    if (ScrollBox)
    {
        ScrollBox->OnUserScrolled.RemoveDynamic(this, &UPaginateScrollWidget::OnUserScroll);
    }
    Super::NativeDestruct();
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
        if (PaginationDirection == EPaginationDirection::Top)
        {
            ScrollBox->ScrollToEnd();
        }
        else if (PaginationDirection == EPaginationDirection::Bottom)
        {
            ScrollBox->ScrollToStart();
        }
    }
    else
    {
        const int32 Index = PaginationDirection == EPaginationDirection::Top ? ScrollBox->GetChildrenCount() - StartListLength + FirstIndex : FirstIndex;
        UWidget* Widget = ScrollBox->GetChildAt(Index);
        ScrollBox->ScrollWidgetIntoView(Widget, false, EDescendantScrollDestination::TopOrLeft, FirstLeadingEdge);
    }

    // Check if we need to already paginate content in 2 ticks' time... TODO Why?
    GetWorld()->GetTimerManager().SetTimerForNextTick(
        [WeakThis = TWeakObjectPtr<UPaginateScrollWidget>(this)]
        {
            if (WeakThis.IsValid())
            {
                WeakThis->GetWorld()->GetTimerManager().SetTimerForNextTick(
                    [WeakThis]
                    {
                        if (WeakThis.IsValid())
                        {
                            WeakThis->OnUserScroll(WeakThis->ScrollBox->GetScrollOffset());
                        }
                    });
            }
        });
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

    if (!EnumHasAnyFlags(Directions, PaginationDirection))
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
        const float TopOffset = ScrollBox->GetScrollOffset();
        const float BottomOffset = ScrollBox->GetScrollOffsetOfEnd() - TopOffset;
        if (TopOffset < PaginateScrollThreshold)
        {
            Directions |= EPaginationDirection::Top;
        }
        if (BottomOffset < PaginateScrollThreshold)
        {
            Directions |= EPaginationDirection::Bottom;
        }
    }
    return Directions;
}
