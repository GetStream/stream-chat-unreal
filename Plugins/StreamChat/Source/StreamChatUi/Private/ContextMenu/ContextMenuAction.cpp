// Copyright 2022 Stream.IO, Inc. All Rights Reserved.

#include "ContextMenu/ContextMenuAction.h"

#include "Framework/Application/SlateApplication.h"

void UContextMenuAction::SetClient(UStreamChatClientComponent* InClient)
{
    Client = InClient;
}

void UContextMenuAction::SetChannel(UChatChannel* InChannel)
{
    Channel = InChannel;
}

void UContextMenuAction::Perform(const FMessage& Message, UWidget* OwningWidget)
{
    OnPerform(Message, OwningWidget);
    OnPerformBlueprint(Message, OwningWidget);

    FSlateApplication::Get().DismissMenuByWidget(OwningWidget->TakeWidget());
}

bool UContextMenuAction::ShouldDisplay(const EMessageSide Side, const FMessage& Message) const
{
    return OnShouldDisplay(Side, Message) && OnShouldDisplayBlueprint(Side, Message);
}

void UContextMenuAction::OnPerform(const FMessage&, UWidget* OwningWidget)
{
}

bool UContextMenuAction::OnShouldDisplay(EMessageSide, const FMessage&) const
{
    return true;
}

bool UContextMenuAction::OnShouldDisplayBlueprint_Implementation(EMessageSide Side, const FMessage& Message) const
{
    return true;
}
