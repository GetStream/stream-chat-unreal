// Copyright 2022 Stream.IO, Inc. All Rights Reserved.

#include "ContextMenu/FlagMessageContextMenuAction.h"

#include "Context/ClientContextWidget.h"
#include "ContextMenu/ContextMenuWidget.h"
#include "User/UserManager.h"

void UFlagMessageContextMenuAction::OnPerform(const FMessage& Message, UWidget* OwningWidget)
{
    Client->FlagMessage(Message);
}

bool UFlagMessageContextMenuAction::OnShouldDisplay(const EMessageSide, const FMessage& Message) const
{
    if (Message.User.IsCurrent())
    {
        return false;
    }

    return Channel && Channel->Properties.OwnCapabilities.Contains(TEXT("flag-message"));
}
