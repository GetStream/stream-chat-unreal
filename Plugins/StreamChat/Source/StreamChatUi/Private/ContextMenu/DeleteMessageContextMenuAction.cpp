// Copyright 2022 Stream.IO, Inc. All Rights Reserved.

#include "ContextMenu/DeleteMessageContextMenuAction.h"

#include "ContextMenu/ContextMenuWidget.h"

void UDeleteMessageContextMenuAction::OnPerform(const FMessage& Message, UWidget* OwningWidget)
{
    Channel->DeleteMessage(Message);
}

bool UDeleteMessageContextMenuAction::OnShouldDisplay(const EMessageSide Side, const FMessage& Message) const
{
    if (Side == EMessageSide::You)
    {
        return false;
    }

    return Message.Type != EMessageType::Deleted;
}
