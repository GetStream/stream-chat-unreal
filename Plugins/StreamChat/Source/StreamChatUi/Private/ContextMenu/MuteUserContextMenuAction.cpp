// Copyright 2022 Stream.IO, Inc. All Rights Reserved.

#include "ContextMenu/MuteUserContextMenuAction.h"

#include "Context/ClientContextWidget.h"
#include "ContextMenu/ContextMenuWidget.h"
#include "User/UserManager.h"

void UMuteUserContextMenuAction::OnPerform(const FMessage& Message, UWidget* OwningWidget)
{
    Client->MuteUser(Message.User);
}

bool UMuteUserContextMenuAction::OnShouldDisplay(const EMessageSide, const FMessage& Message) const
{
    if (Message.User.IsCurrent())
    {
        return false;
    }

    return !UUserManager::Get()->GetCurrentUser().HasMutedUser(Message.User);
}
