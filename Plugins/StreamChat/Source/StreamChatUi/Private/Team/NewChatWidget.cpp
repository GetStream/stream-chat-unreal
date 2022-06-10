// Copyright 2022 Stream.IO, Inc. All Rights Reserved.

#include "Team/NewChatWidget.h"

#include "Context/ClientContextWidget.h"
#include "StreamChatClientComponent.h"
#include "User/UserManager.h"

UNewChatWidget::UNewChatWidget()
{
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
    if (GroupName)
    {
        GroupName->OnGroupNameChanged.AddDynamic(this, &UNewChatWidget::OnGroupNameChanged);
    }
    if (SelectedContacts)
    {
        SelectedContacts->OnSearchTextChanged.AddDynamic(this, &UNewChatWidget::OnSearchTextChanged);
    }
    if (Composer)
    {
        Composer->OnSendMessage.AddDynamic(this, &UNewChatWidget::OnSendMessage);
    }
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

void UNewChatWidget::OnGroupNameChanged(const FText& Text)
{
    if (!SelectedContacts || !Header)
    {
        return;
    }

    if (Text.IsEmpty())
    {
        SelectedContacts->SetGroupMode(false);
        Header->SetTitle(FText::FromString(TEXT("New Chat")));
    }
    else
    {
        SelectedContacts->SetGroupMode(true);
        Header->SetTitle(FText::FromString(TEXT("New Group Chat")));
    }
}

void UNewChatWidget::OnSearchTextChanged(const FText& Text)
{
    if (UserList)
    {
        if (Text.IsEmpty())
        {
            UserList->SetQuery();
        }
        else
        {
            UserList->SetQuery(FFilter::Autocomplete(TEXT("name"), Text.ToString()));
        }
    }
}

void UNewChatWidget::OnSendMessage(const FString& Text)
{
    if (!SelectedContacts)
    {
        return;
    }
    UClientContextWidget* Context = UClientContextWidget::Get(this);

    if (!Context)
    {
        return;
    }

    TArray<FUserRef> Members{SelectedContacts->GetUsers()};
    Members.Add(UUserManager::Get()->GetCurrentUser().User);
    FChannelProperties Props = FChannelProperties{TEXT("messaging")}.SetMembers(Members);
    const FText Group = GroupName->GetGroupName();
    if (!Group.IsEmpty())
    {
        Props.SetName(Group.ToString());
    }
    Context->GetClient()->CancelNewChat();
    Context->GetClient()->QueryChannel(
        Props,
        EChannelFlags::State | EChannelFlags::Watch,
        [Context, Text](UChatChannel* NewChannel)
        {
            NewChannel->SendMessage(FMessage{Text});
            Context->SelectChannel(NewChannel);
        });
}
