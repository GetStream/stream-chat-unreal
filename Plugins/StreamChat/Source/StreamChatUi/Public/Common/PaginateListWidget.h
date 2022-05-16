// Copyright 2022 Stream.IO, Inc. All Rights Reserved.

#pragma once

#include "Channel/ChatChannel.h"
#include "CoreMinimal.h"
#include "StreamListView.h"

template <class ItemType>
class SPaginateListWidget : public SStreamListView<ItemType>
{
    DECLARE_DELEGATE_TwoParams(FOnPaginatingDelegate, EPaginationDirection, EHttpRequestState);
    DECLARE_DELEGATE_TwoParams(FDoPaginateDelegate, EPaginationDirection, TFunction<void()>);

public:
    SLATE_BEGIN_ARGS(SPaginateListWidget<ItemType>)
        : _Limit{20}
        , _PaginateScrollThreshold{100.f}
        , _PaginationDirection{EPaginationDirection::Bottom}
        , _DoPaginate{}
        , _OnPaginating{}
        , _ListItemsSource{nullptr}
        , _CreateListViewWidget{}
    {
    }

    SLATE_ARGUMENT(uint32, Limit)
    SLATE_ARGUMENT(float, PaginateScrollThreshold)
    SLATE_ARGUMENT(EPaginationDirection, PaginationDirection)
    SLATE_EVENT(FDoPaginateDelegate, DoPaginate);
    SLATE_EVENT(FOnPaginatingDelegate, OnPaginating);
    SLATE_ARGUMENT(const TArray<ItemType>*, ListItemsSource)
    SLATE_EVENT(SStreamListView<ItemType>::FCreateListViewWidgetDelegate, CreateListViewWidget);

    SLATE_END_ARGS()

    void Construct(const FArguments& InArgs);

protected:
    void OnScroll(double CurrentOffset)
    {
        if (!DoPaginate.IsBound() || PaginationRequestState == EHttpRequestState::Started)
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

        // TODO if the user scrolls a lot while awaiting the response, these values will be out of date
        const int32 FirstItemIndex = FMath::TruncToInt32(CurrentOffset);
        const float FirstOffset = FMath::Fractional(CurrentOffset);
        const ItemType FirstItem = (*this->ItemsSource)[FirstItemIndex];
        const int32 OrigWidgetCount = GetItemCount();

        DoPaginate.Execute(
            Directions,
            [=, WeakThis = TWeakPtr<SPaginateListWidget>{SharedThis(this)}]
            {
                if (!WeakThis.IsValid())
                {
                    return;
                }
                TSharedPtr<SPaginateListWidget> This = WeakThis.Pin();

                const uint32 DeltaChildrenCount = This->GetItemCount() - OrigWidgetCount;
                if (DeltaChildrenCount == 0 || DeltaChildrenCount < This->Limit)
                {
                    // Don't need to paginate again in this direction in the future
                    This->EndedPaginationDirections |= Directions;
                }

                This->SetPaginationRequestState(EHttpRequestState::Ended, Directions);

                const int32 NewIndex = This->ItemsSource->Find(FirstItem);
                This->ScrollTo(static_cast<float>(NewIndex) + FirstOffset);
            });
    }

    /// Number of additional objects to be queried when paginating
    uint32 Limit = 20;
    // If the scroll offset is below this value, then new messages will be fetched
    float PaginateScrollThreshold = 100.f;
    // The end of the scroll box which causes pagination to be triggered
    EPaginationDirection PaginationDirection = EPaginationDirection::Bottom;
    /// The function that performs the pagination API call
    FDoPaginateDelegate DoPaginate;
    /// Fired during various stages of pagination
    FOnPaginatingDelegate OnPaginating;

private:
    void SetPaginationRequestState(const EHttpRequestState RequestState, const EPaginationDirection Direction)
    {
        PaginationRequestState = RequestState;
        OnPaginating.ExecuteIfBound(Direction, RequestState);
    }

    EPaginationDirection GetDirections() const
    {
        EPaginationDirection Directions = EPaginationDirection::None;
        const float Height = this->GetCachedGeometry().GetLocalSize().Y;
        const float TopOffset = this->ScrollBar->DistanceFromTop() * Height;
        const float BottomOffset = this->ScrollBar->DistanceFromBottom() * Height;
        if (TopOffset < PaginateScrollThreshold)
        {
            Directions |= EPaginationDirection::Top;
        }
        if (BottomOffset < PaginateScrollThreshold)
        {
            Directions |= EPaginationDirection::Bottom;
        }
        return Directions;
    }

    uint32 GetItemCount() const
    {
        return this->ItemsSource->Num();
    }

    EPaginationDirection EndedPaginationDirections = EPaginationDirection::None;
    EHttpRequestState PaginationRequestState = EHttpRequestState::Ended;
};

template <class ItemType>
void SPaginateListWidget<ItemType>::Construct(const FArguments& InArgs)
{
    SStreamListView<ItemType>::Construct(SStreamListView<ItemType>::FArguments()
                                             .ListItemsSource(InArgs._ListItemsSource)
                                             .CreateListViewWidget(InArgs._CreateListViewWidget)
                                             .OnListViewScrolled(this, &SPaginateListWidget<ItemType>::OnScroll));

    this->Limit = InArgs._Limit;
    this->PaginateScrollThreshold = InArgs._PaginateScrollThreshold;
    this->PaginationDirection = InArgs._PaginationDirection;
    this->DoPaginate = InArgs._DoPaginate;
    this->OnPaginating = InArgs._OnPaginating;
}
