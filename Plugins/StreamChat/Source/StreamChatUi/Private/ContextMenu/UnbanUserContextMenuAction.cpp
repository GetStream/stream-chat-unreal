// Copyright 2022 Stream.IO, Inc. All Rights Reserved.

#include "ContextMenu/UnbanUserContextMenuAction.h"

#include "ContextMenu/ContextMenuWidget.h"
#include "User/UserManager.h"

void UUnbanUserContextMenuAction::OnPerform(const FMessage& Message, UWidget* OwningWidget)
{
    Channel->UnbanMember(Message.User);
}

bool UUnbanUserContextMenuAction::OnShouldDisplay(const EMessageSide, const FMessage& Message) const
{
    if (Message.User.IsCurrent())
    {
        return false;
    }
    const FMember* Member = Channel->Properties.GetMember(Message.User);
    if (!Member)
    {
        return false;
    }
    if (!Member->bBanned)
    {
        return false;
    }

    // TODO this will be improved in the backend soon
    if (Member->ChannelRole == TEXT("admin") || Member->ChannelRole == TEXT("moderator"))
    {
        return true;
    }

    const FString& Role = UUserManager::Get()->GetCurrentUser().User->Role;
    return Role == TEXT("admin") || Role == TEXT("moderator");
}
