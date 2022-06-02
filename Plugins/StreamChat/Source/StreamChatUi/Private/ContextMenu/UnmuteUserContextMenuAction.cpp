// Copyright 2022 Stream.IO, Inc. All Rights Reserved.

#include "ContextMenu/UnmuteUserContextMenuAction.h"

#include "Context/ClientContextWidget.h"
#include "ContextMenu/ContextMenuWidget.h"
#include "User/UserManager.h"

void UUnmuteUserContextMenuAction::OnPerform(const FMessage& Message, UWidget* OwningWidget)
{
    Client->UnmuteUser(Message.User);
}

bool UUnmuteUserContextMenuAction::OnShouldDisplay(const EMessageSide, const FMessage& Message) const
{
    if (Message.User.IsCurrent())
    {
        return false;
    }

    return UUserManager::Get()->GetCurrentUser().HasMutedUser(Message.User);
}
