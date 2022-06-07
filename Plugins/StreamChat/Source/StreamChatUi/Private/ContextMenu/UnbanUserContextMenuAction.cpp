// Copyright 2022 Stream.IO, Inc. All Rights Reserved.

#include "ContextMenu/UnbanUserContextMenuAction.h"

#include "Context/ClientContextWidget.h"
#include "ContextMenu/ContextMenuWidget.h"
#include "User/UserManager.h"

void UUnbanUserContextMenuAction::OnPerform(const FMessage& Message, UWidget* OwningWidget)
{
    Client->UnbanUser(Message.User);
}

bool UUnbanUserContextMenuAction::OnShouldDisplay(const EMessageSide, const FMessage& Message) const
{
    if (Message.User.IsCurrent() || !Message.User->bBanned)
    {
        return false;
    }

    const FString& Role = UUserManager::Get()->GetCurrentUser().User->Role;
    return Role == TEXT("admin") || Role == TEXT("moderator");
}
