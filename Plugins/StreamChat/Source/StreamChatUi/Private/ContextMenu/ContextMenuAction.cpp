// Copyright 2021 Stream.IO, Inc. All Rights Reserved.

#include "ContextMenu/ContextMenuAction.h"

#include "Framework/Application/SlateApplication.h"

void UContextMenuAction::Perform(const FMessage& Message, UWidget* OwningWidget)
{
    OnPerform(Message, OwningWidget);
    OnPerformBlueprint(Message, OwningWidget);

    FSlateApplication::Get().DismissMenuByWidget(OwningWidget->TakeWidget());
}

bool UContextMenuAction::ShouldDisplay(const EMessageSide Side, const FMessage& Message) const
{
    return OnShouldDisplay(Side, Message);
}

void UContextMenuAction::OnPerform(const FMessage&, UWidget*)
{
}

bool UContextMenuAction::OnShouldDisplay(EMessageSide, const FMessage&) const
{
    return true;
}
