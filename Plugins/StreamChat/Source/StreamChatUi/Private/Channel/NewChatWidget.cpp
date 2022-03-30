// Copyright 2022 Stream.IO, Inc. All Rights Reserved.

#include "Channel/NewChatWidget.h"

UNewChatWidget::UNewChatWidget()
{
    bWantsTheme = true;
}

void UNewChatWidget::Setup()
{
    Super::Setup();
}

void UNewChatWidget::OnSetup()
{
    if (UserList)
    {
        UserList->SetQuery();
        UserList->OnUserClicked.AddDynamic(this, &UNewChatWidget::OnUserClicked);
    }
}

void UNewChatWidget::OnTheme()
{
}

void UNewChatWidget::OnUserClicked(const FUserRef& User, const bool bSelected)
{
    if (SelectedContacts)
    {
        if (bSelected)
        {
            SelectedContacts->AddUser(User);
        }
        else
        {
            SelectedContacts->RemoveUser(User);
        }
    }
}
