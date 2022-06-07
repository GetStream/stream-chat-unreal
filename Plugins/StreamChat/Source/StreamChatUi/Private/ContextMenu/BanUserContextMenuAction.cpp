// Copyright 2022 Stream.IO, Inc. All Rights Reserved.

#include "ContextMenu/BanUserContextMenuAction.h"

#include "Context/ClientContextWidget.h"
#include "ContextMenu/ContextMenuWidget.h"
#include "User/UserManager.h"

void UBanUserContextMenuAction::OnPerform(const FMessage& Message, UWidget* OwningWidget)
{
    Client->BanUser(Message.User);
}

bool UBanUserContextMenuAction::OnShouldDisplay(const EMessageSide, const FMessage& Message) const
{
    if (Message.User.IsCurrent())
    {
        return false;
    }

    // TODO this will be improved in the backend soon
    if (const FMember* Member = Channel->Properties.GetCurrentUserMember())
    {
        if (Member->ChannelRole == TEXT("admin") || Member->ChannelRole == TEXT("moderator"))
        {
            return true;
        }
    }

    const FString& Role = UUserManager::Get()->GetCurrentUser().User->Role;
    return Role == TEXT("admin") || Role == TEXT("moderator");
}
