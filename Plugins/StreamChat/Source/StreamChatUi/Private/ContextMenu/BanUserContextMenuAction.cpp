// Copyright 2022 Stream.IO, Inc. All Rights Reserved.

#include "ContextMenu/BanUserContextMenuAction.h"

#include "Context/ClientContextWidget.h"
#include "ContextMenu/ContextMenuWidget.h"
#include "User/UserManager.h"

void UBanUserContextMenuAction::OnPerform(const FMessage& Message, UWidget* OwningWidget)
{
    Client->BanUser(Message.User);
}

bool UBanUserContextMenuAction::OnShouldDisplay(const EMessageSide, const FMessage& Message) const
{
    if (Message.User.IsCurrent())
    {
        return false;
    }

    // TODO this will be improved in the backend soon
    const FString& Role = Message.User->Role;
    return Role == TEXT("admin") || Role == TEXT("moderator") || Role.IsEmpty();
}
