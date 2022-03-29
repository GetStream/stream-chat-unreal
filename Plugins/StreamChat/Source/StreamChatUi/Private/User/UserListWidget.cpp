// Copyright 2022 Stream.IO, Inc. All Rights Reserved.

#include "User/UserListWidget.h"

#include "StreamChatClientComponent.h"

UUserListWidget::UUserListWidget()
{
    bWantsClient = true;
}

void UUserListWidget::SetQuery(const FFilter& UsersQueryFilter, const TArray<FUserSortOption>& UserQuerySort)
{
    Filter = UsersQueryFilter;
    Sort = UserQuerySort;

    QueryUsers();
}

void UUserListWidget::OnSetup()
{
}

void UUserListWidget::OnClient()
{
    QueryUsers();
}

void UUserListWidget::QueryUsers()
{
    if (!Client)
    {
        return;
    }
    Client->QueryUsers(
        [WeakThis = TWeakObjectPtr<UUserListWidget>(this)](const TArray<FUserRef>& Users)
        {
            if (WeakThis.IsValid())
            {
                WeakThis->PopulateScrollBox(Users);
            }
        },
        Filter,
        Sort);
}

void UUserListWidget::PopulateScrollBox(const TArray<FUserRef>& Users)
{
    for (UWidget* Child : ScrollBox->GetAllChildren())
    {
        if (UUserStatusWidget* Widget = Cast<UUserStatusWidget>(Child))
        {
            Widget->OnUserStatusClicked.RemoveDynamic(this, &UUserListWidget::UserStatusClicked);
        }
    }
    ScrollBox->ClearChildren();

    for (const FUserRef& User : Users)
    {
        UUserStatusWidget* Widget = CreateWidget<UUserStatusWidget>(this, UserStatusWidgetClass);
        Widget->Setup(User);
        Widget->OnUserStatusClicked.AddDynamic(this, &UUserListWidget::UserStatusClicked);
        ScrollBox->AddChild(Widget);
    }
}

void UUserListWidget::UserStatusClicked(const FUserRef& User)
{
    OnUserClicked.Broadcast(User);
}
