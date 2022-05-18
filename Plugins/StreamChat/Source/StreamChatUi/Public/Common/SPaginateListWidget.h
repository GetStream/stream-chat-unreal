// Copyright 2022 Stream.IO, Inc. All Rights Reserved.

#pragma once

#include "Channel/ChatChannel.h"
#include "CoreMinimal.h"
#include "SStreamListView.h"

template <class ItemType>
class SPaginateListWidget : public SStreamListView<ItemType>
{
    using FSuperArguments = typename SStreamListView<ItemType>::FArguments;
    using FCreateListViewWidgetDelegate = typename SStreamListView<ItemType>::FCreateListViewWidgetDelegate;

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
    SLATE_EVENT(FCreateListViewWidgetDelegate, CreateListViewWidget);

    SLATE_END_ARGS()

    void Construct(const FArguments& InArgs);
    virtual STableViewBase::FReGenerateResults ReGenerateItems(const FGeometry& MyGeometry) override;

protected:
    void OnScroll(const double CurrentOffset);
    void ConditionallyPaginate();
    void Paginate(const EPaginationDirection Directions);

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
    void SetPaginationRequestState(const EHttpRequestState RequestState, const EPaginationDirection Direction);
    EPaginationDirection GetDirections() const;
    uint32 GetItemCount() const;

    EPaginationDirection EndedPaginationDirections = EPaginationDirection::None;
    EHttpRequestState PaginationRequestState = EHttpRequestState::Ended;
    TOptional<ItemType> FirstItem;
};

template <class ItemType>
void SPaginateListWidget<ItemType>::Construct(const FArguments& InArgs)
{
    SStreamListView<ItemType>::Construct(FSuperArguments()
                                             .ListItemsSource(InArgs._ListItemsSource)
                                             .CreateListViewWidget(InArgs._CreateListViewWidget)
                                             .OnListViewScrolled(this, &SPaginateListWidget<ItemType>::OnScroll));

    this->Limit = InArgs._Limit;
    this->PaginateScrollThreshold = InArgs._PaginateScrollThreshold;
    this->PaginationDirection = InArgs._PaginationDirection;
    this->DoPaginate = InArgs._DoPaginate;
    this->OnPaginating = InArgs._OnPaginating;
}

template <class ItemType>
STableViewBase::FReGenerateResults SPaginateListWidget<ItemType>::ReGenerateItems(const FGeometry& MyGeometry)
{
    const STableViewBase::FReGenerateResults Result = SListView<ItemType>::ReGenerateItems(MyGeometry);

    // Scroll to end on first population
    if (!FirstItem.IsSet() && Result.LengthOfGeneratedItems > 0.)
    {
        if (PaginationDirection == EPaginationDirection::Top)
        {
            this->ScrollToBottom();
        }
        else if (PaginationDirection == EPaginationDirection::Bottom)
        {
            this->ScrollToTop();
            OnScroll(this->CurrentScrollOffset);
        }
    }
    return Result;
}

template <class ItemType>
void SPaginateListWidget<ItemType>::OnScroll(const double CurrentOffset)
{
    const int32 FirstItemIndex = FMath::TruncToInt(CurrentOffset);
    FirstItem = this->ItemsSource->IsValidIndex(FirstItemIndex) ? (*this->ItemsSource)[FirstItemIndex] : TOptional<ItemType>{};

    ConditionallyPaginate();
}

template <class ItemType>
void SPaginateListWidget<ItemType>::ConditionallyPaginate()
{
    // Skip if a request is already in-flight
    if (!DoPaginate.IsBound() || PaginationRequestState == EHttpRequestState::Started)
    {
        return;
    }

    // Skip if currently no data
    if (this->GetItemCount() == 0)
    {
        return;
    }

    // Skip if not within the threshold of either end
    const EPaginationDirection Directions = GetDirections();
    if (Directions == EPaginationDirection::None)
    {
        return;
    }

    // Skip if the paginating in an unsupported direction
    if (!EnumHasAnyFlags(Directions, PaginationDirection))
    {
        return;
    }

    // Skip if no more items will exist in this direction
    if (EnumHasAllFlags(EndedPaginationDirections, Directions))
    {
        return;
    }

    Paginate(Directions);
}

template <class ItemType>
void SPaginateListWidget<ItemType>::Paginate(const EPaginationDirection Directions)
{
    SetPaginationRequestState(EHttpRequestState::Started, Directions);

    DoPaginate.Execute(
        Directions,
        [WeakThis = TWeakPtr<SWidget>(this->AsShared()), OrigWidgetCount = GetItemCount(), Directions]
        {
            if (!WeakThis.IsValid())
            {
                return;
            }
            TSharedPtr<SPaginateListWidget> This = StaticCastSharedPtr<SPaginateListWidget>(WeakThis.Pin());

            const uint32 DeltaChildrenCount = This->GetItemCount() - OrigWidgetCount;
            if (DeltaChildrenCount == 0 || DeltaChildrenCount < This->Limit)
            {
                // Don't need to paginate again in this direction in the future
                This->EndedPaginationDirections |= Directions;
            }

            This->SetPaginationRequestState(EHttpRequestState::Ended, Directions);

            if (This->FirstItem.IsSet())
            {
                const float FirstOffset = FMath::Fractional(This->DesiredScrollOffset);
                const int32 NewIndex = This->ItemsSource->Find(This->FirstItem.GetValue());
                This->ScrollTo(static_cast<float>(NewIndex) + FirstOffset);
            }
        });
}

template <class ItemType>
void SPaginateListWidget<ItemType>::SetPaginationRequestState(const EHttpRequestState RequestState, const EPaginationDirection Direction)
{
    PaginationRequestState = RequestState;
    OnPaginating.ExecuteIfBound(Direction, RequestState);
}

template <class ItemType>
EPaginationDirection SPaginateListWidget<ItemType>::GetDirections() const
{
    EPaginationDirection Directions = EPaginationDirection::None;
    const float Height = this->GetCachedGeometry().GetLocalSize().Y;
    if (Height == 0.f)
    {
        return Directions;
    }

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

template <class ItemType>
uint32 SPaginateListWidget<ItemType>::GetItemCount() const
{
    return this->ItemsSource->Num();
}
