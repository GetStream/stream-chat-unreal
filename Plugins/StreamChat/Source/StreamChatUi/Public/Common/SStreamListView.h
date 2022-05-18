// Copyright 2022 Stream.IO, Inc. All Rights Reserved.

#pragma once

#include "Components/NativeWidgetHost.h"
#include "CoreMinimal.h"
#include "Launch/Resources/Version.h"
#include "Slate/SObjectTableRow.h"

template <class ItemType>
class SStreamListView : public SListView<ItemType>
{
public:
    using FSuperArguments = typename SListView<ItemType>::FArguments;

    DECLARE_DELEGATE_RetVal_OneParam(UWidget*, FCreateListViewWidgetDelegate, const ItemType&);

    SLATE_BEGIN_ARGS(SStreamListView<ItemType>) : _ListItemsSource(nullptr), _CreateListViewWidget(), _OnListViewScrolled()
    {
    }

    SLATE_ARGUMENT(const TArray<ItemType>*, ListItemsSource)
    SLATE_EVENT(FCreateListViewWidgetDelegate, CreateListViewWidget);
    SLATE_EVENT(FOnTableViewScrolled, OnListViewScrolled)

    SLATE_END_ARGS()

    void Construct(const FArguments& InArgs);

private:
    class SNullTableRow : public STableRow<ItemType>
    {
    public:
        virtual const FSlateBrush* GetBorder() const override
        {
            return nullptr;
        }

    protected:
        virtual ESelectionMode::Type GetSelectionMode() const override
        {
            return ESelectionMode::None;
        }
    };

    TSharedRef<ITableRow> HandleGenerateRow(ItemType Item, const TSharedRef<STableViewBase>& OwnerTable)
    {
        UWidget* EntryWidget = CreateItemWidget.Execute(Item);
        const TSharedPtr<SWidget> CachedWidget = EntryWidget->TakeWidget();
        return SNew(SNullTableRow, OwnerTable)[CachedWidget.ToSharedRef()];
    }

    FCreateListViewWidgetDelegate CreateItemWidget;
};

template <class ItemType>
void SStreamListView<ItemType>::Construct(const FArguments& InArgs)
{
    SListView<ItemType>::Construct(FSuperArguments()
#if ENGINE_MAJOR_VERSION == 5 && ENGINE_MINOR_VERSION >= 0
                                       // UE5 adds a default background brush which we want to disable
                                       .ListViewStyle(&FTableViewStyle::GetDefault())
#endif
                                       .OnGenerateRow(this, &SStreamListView::HandleGenerateRow)
                                       .ListItemsSource(InArgs._ListItemsSource)
                                       .OnListViewScrolled(InArgs._OnListViewScrolled));

    this->CreateItemWidget = InArgs._CreateListViewWidget;
}
