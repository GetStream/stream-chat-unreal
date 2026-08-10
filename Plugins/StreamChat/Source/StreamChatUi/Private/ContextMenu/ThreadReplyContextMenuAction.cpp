// Copyright 2026 Stream.IO, Inc. All Rights Reserved.

#include "ContextMenu/ThreadReplyContextMenuAction.h"

#include "Context/ChannelContextWidget.h"

UThreadReplyContextMenuAction::UThreadReplyContextMenuAction()
{
    Label = NSLOCTEXT("StreamChat", "ReplyInThreadAction", "Reply in thread");
}

void UThreadReplyContextMenuAction::OnPerform(const FMessage& Message, UWidget* OwningWidget)
{
    if (UChannelContextWidget* Context = UChannelContextWidget::Get(OwningWidget))
    {
        Context->OpenThread(Message);
    }
}

bool UThreadReplyContextMenuAction::OnShouldDisplay(EMessageSide, const FMessage& Message) const
{
    // Threads do not nest, and a deleted message has nothing to reply to
    return !Message.IsThreadReply() && Message.Type != EMessageType::Deleted;
}
