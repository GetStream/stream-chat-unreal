// Copyright Stream.IO, Inc. All Rights Reserved.

#include "ContextMenu/DeleteMessageContextMenuAction.h"

#include "Context/ChannelContextWidget.h"
#include "ContextMenu/ContextMenuWidget.h"

void UDeleteMessageContextMenuAction::OnPerform(const FMessage& Message, UWidget* OwningWidget)
{
    if (UChatChannel* Channel = UChannelContextWidget::GetChannel(OwningWidget))
    {
        Channel->DeleteMessage(Message);
    }
}
