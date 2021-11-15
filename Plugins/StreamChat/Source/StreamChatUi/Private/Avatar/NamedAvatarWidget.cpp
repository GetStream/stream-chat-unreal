// Copyright Stream.IO, Inc. All Rights Reserved.

#include "Avatar/NamedAvatarWidget.h"

#include "User/User.h"

void UNamedAvatarWidget::Setup(const FUserRef& InUser)
{
    User = InUser;

    Super::Setup();
}

void UNamedAvatarWidget::OnSetup()
{
    if (Avatar)
    {
        Avatar->Setup({User});
    }

    if (TextBlock)
    {
        if (User.IsCurrent())
        {
            TextBlock->SetText(FText::FromString(TEXT("You")));
        }
        else
        {
            TextBlock->SetText(FText::FromString(User->Name));
        }
    }
}
