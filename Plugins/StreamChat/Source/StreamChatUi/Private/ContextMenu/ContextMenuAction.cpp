// Copyright Stream.IO, Inc. All Rights Reserved.

#include "ContextMenu/ContextMenuAction.h"

void UContextMenuAction::Perform(const FMessage& Message)
{
    OnPerform(Message);
    OnPerformBlueprint(Message);
}

void UContextMenuAction::OnPerform(const FMessage&)
{
}
