// Copyright 2026 Stream.IO, Inc. All Rights Reserved.

#include "ContextMenu/ResendMessageContextMenuAction.h"

#include "Channel/ChatChannel.h"

UResendMessageContextMenuAction::UResendMessageContextMenuAction()
{
    Label = NSLOCTEXT("StreamChat", "ResendMessageAction", "Send anyway");
}

void UResendMessageContextMenuAction::OnPerform(const FMessage& Message, UWidget* OwningWidget)
{
    if (Channel)
    {
        Channel->ResendMessage(Message);
    }
}

bool UResendMessageContextMenuAction::OnShouldDisplay(const EMessageSide, const FMessage& Message) const
{
    // Only a bounced message can be sent again: anything else either reached the channel already or
    // failed for a reason resending will not fix.
    return Message.IsModerationError();
}
