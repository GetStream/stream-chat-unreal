// Copyright Stream.IO, Inc. All Rights Reserved.

#include "ContextMenu/ContextMenuAction.h"

#include "Framework/Application/SlateApplication.h"

void UContextMenuAction::Perform(const FMessage& Message, UWidget* OwningWidget)
{
    OnPerform(Message, OwningWidget);
    OnPerformBlueprint(Message, OwningWidget);

    FSlateApplication::Get().DismissMenuByWidget(OwningWidget->TakeWidget());
}

void UContextMenuAction::OnPerform(const FMessage&, UWidget*)
{
}
