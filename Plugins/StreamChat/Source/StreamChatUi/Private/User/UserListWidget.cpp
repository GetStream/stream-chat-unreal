// Copyright 2022 Stream.IO, Inc. All Rights Reserved.

#include "User/UserListWidget.h"

#include "StreamChatClientComponent.h"

UUserListWidget::UUserListWidget()
{
    bWantsClient = true;
}

void UUserListWidget::SetQuery(const FFilter& UsersQueryFilter)
{
    Filter = UsersQueryFilter;

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
    const FUserSortOption Sort{EUserSortField::Custom, TEXT("name"), ESortDirection::Ascending};
    Client->QueryUsers(
        [WeakThis = TWeakObjectPtr<UUserListWidget>(this)](const TArray<FUserRef>& Users)
        {
            if (WeakThis.IsValid())
            {
                WeakThis->PopulateScrollBox(Users);
            }
        },
        Filter,
        {Sort});
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

    TCHAR PrevLetter = TEXT('\0');
    for (const FUserRef& User : Users)
    {
        const FString& Name = User->GetNameOrId();
        if (Name.IsEmpty())
        {
            continue;
        }
        const TCHAR Letter = FChar::ToUpper(Name[0]);
        if (Letter != PrevLetter)
        {
            USectionHeadingWidget* Widget = CreateWidget<USectionHeadingWidget>(this, SectionHeadingWidgetClass);
            Widget->Setup(FText::FromString(FString(1, &Letter)));
            ScrollBox->AddChild(Widget);
            PrevLetter = Letter;
        }

        UUserStatusWidget* Widget = CreateWidget<UUserStatusWidget>(this, UserStatusWidgetClass);
        Widget->Setup(User);
        Widget->OnUserStatusClicked.AddDynamic(this, &UUserListWidget::UserStatusClicked);
        ScrollBox->AddChild(Widget);
    }
}

void UUserListWidget::UserStatusClicked(const FUserRef& User, bool bSelected)
{
    OnUserClicked.Broadcast(User, bSelected);
}
