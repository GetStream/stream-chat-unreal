// Copyright 2021 Stream.IO, Inc. All Rights Reserved.

#include "ContextMenu/CopyMessageContextMenuAction.h"

#include "ContextMenu/ContextMenuWidget.h"
#include "HAL/PlatformApplicationMisc.h"

void UCopyMessageContextMenuAction::OnPerform(const FMessage& Message, UWidget* OwningWidget)
{
    FPlatformApplicationMisc::ClipboardCopy(*Message.Text);
}
