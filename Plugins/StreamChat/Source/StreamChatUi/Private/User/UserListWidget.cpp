// Copyright 2022 Stream.IO, Inc. All Rights Reserved.

#include "User/UserListWidget.h"

#include "StreamChatClientComponent.h"

UUserListWidget::UUserListWidget()
{
}

void UUserListWidget::SetQuery(const FFilter& UsersQueryFilter)
{
    Filter = UsersQueryFilter;

    Refetch();
}

void UUserListWidget::OnSetup()
{
}

void UUserListWidget::NativePreConstruct()
{
    Refetch();
}

void UUserListWidget::Paginate(const EPaginationDirection Directions, const TFunction<void()> Callback)
{
    if (!GetClient())
    {
        return;
    }
    const FUserSortOption SortName{EUserSortField::Custom, ESortDirection::Ascending, TEXT("name")};
    GetClient()->QueryUsers(
        Filter,
        {SortName},
        false,
        30,
        Users.Num(),
        [WeakThis = TWeakObjectPtr<UUserListWidget>(this), Callback](const TArray<FUserRef>& QueryUsers)
        {
            if (WeakThis.IsValid())
            {
                WeakThis->Users.Append(QueryUsers);
                WeakThis->PopulateScrollBox();
            }

            if (Callback)
            {
                Callback();
            }
        });
}

void UUserListWidget::Refetch()
{
    Users.Empty();
    Paginate(EPaginationDirection::Bottom, {});
}

void UUserListWidget::PopulateScrollBox()
{
    for (UWidget* Child : ScrollBox->GetAllChildren())
    {
        if (UUserStatusWidget* Widget = Cast<UUserStatusWidget>(Child))
        {
            Widget->OnUserStatusClicked.RemoveDynamic(this, &UUserListWidget::UserStatusClicked);
        }
    }

    TArray<UWidget*> Children;
    TCHAR PrevLetter = TEXT('\0');
    for (const FUserRef& User : Users)
    {
        // Skip users without a name
        if (User->Name.IsEmpty())
        {
            continue;
        }

        const TCHAR Letter = FChar::ToUpper(User->Name[0]);
        if (Letter != PrevLetter)
        {
            USectionHeadingWidget* Widget = CreateWidget<USectionHeadingWidget>(this, SectionHeadingWidgetClass);
            Widget->Setup(FText::FromString(FString(1, &Letter)));
            Children.Add(Widget);
            PrevLetter = Letter;
        }

        UUserStatusWidget* Widget = CreateWidget<UUserStatusWidget>(this, UserStatusWidgetClass);
        Widget->Setup(User);
        Widget->OnUserStatusClicked.AddDynamic(this, &UUserListWidget::UserStatusClicked);
        Children.Add(Widget);
    }

    SetChildren(Children);
}

void UUserListWidget::UserStatusClicked(const FUserRef& User, const bool bSelected)
{
    OnUserClicked.Broadcast(User, bSelected);
}
