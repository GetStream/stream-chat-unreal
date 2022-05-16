// Copyright 2022 Stream.IO, Inc. All Rights Reserved.

#pragma once

#include "Components/NativeWidgetHost.h"
#include "CoreMinimal.h"
#include "Slate/SObjectTableRow.h"

template <class ItemType>
class TListViewGenerator : public TSharedFromThis<TListViewGenerator<ItemType>>
{
public:
    DECLARE_DELEGATE_RetVal_OneParam(UWidget*, FCreateListViewWidgetDelegate, const ItemType&);

    void CreateListView(const TArray<ItemType>* ItemsSource, FCreateListViewWidgetDelegate&& CreateItemWidgetCallback, UNativeWidgetHost* Host)
    {
        CreateItemWidget = MoveTemp(CreateItemWidgetCallback);
        SAssignNew(MyListView, SListView<ItemType>)
            .ListItemsSource(ItemsSource)
            .OnGenerateRow(this, &TListViewGenerator::HandleGenerateRow)
            .ListViewStyle(&FTableViewStyle::GetDefault());
        Host->SetContent(MyListView.ToSharedRef());
    }

private:
    class SNullTableRow : public STableRow<ItemType>
    {
    public:
        virtual const FSlateBrush* GetBorder() const override
        {
            return nullptr;
        }
    };

    TSharedRef<ITableRow> HandleGenerateRow(ItemType Item, const TSharedRef<STableViewBase>& OwnerTable)
    {
        UWidget* EntryWidget = CreateItemWidget.Execute(Item);
        const TSharedPtr<SWidget> CachedWidget = EntryWidget->TakeWidget();
        return SNew(SNullTableRow, OwnerTable)[CachedWidget.ToSharedRef()];
    }

    FCreateListViewWidgetDelegate CreateItemWidget;
    TSharedPtr<SListView<ItemType>> MyListView;
};
