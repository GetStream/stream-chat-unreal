// Copyright Stream.IO, Inc. All Rights Reserved.

#include "ContextMenu/EditMessageContextMenuAction.h"

#include "Context/ChannelContextWidget.h"
#include "ContextMenu/ContextMenuWidget.h"

void UEditMessageContextMenuAction::OnPerform(const FMessage& Message, UWidget* OwningWidget)
{
    if (const UChannelContextWidget* Context = UChannelContextWidget::Get(OwningWidget))
    {
        Context->OnStartEditMessage.Broadcast(Message);
    }
}
