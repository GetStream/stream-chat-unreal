// Copyright Stream.IO, Inc. All Rights Reserved.

#include "ContextMenu/CopyMessageContextMenuAction.h"

#include "HAL/PlatformApplicationMisc.h"

void UCopyMessageContextMenuAction::OnPerform(const FMessage& Message)
{
    FPlatformApplicationMisc::ClipboardCopy(*Message.Text);
}
