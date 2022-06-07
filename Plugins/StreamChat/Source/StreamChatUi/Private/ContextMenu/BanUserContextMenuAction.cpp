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
    if (Message.User.IsCurrent() || Message.User->bBanned)
    {
        return false;
    }

    const FString& Role = UUserManager::Get()->GetCurrentUser().User->Role;
    return Role == TEXT("admin") || Role == TEXT("moderator");
}
