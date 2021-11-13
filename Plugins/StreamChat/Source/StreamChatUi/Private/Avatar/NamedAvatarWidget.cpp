// Copyright Stream.IO, Inc. All Rights Reserved.

#include "Avatar/NamedAvatarWidget.h"

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
        TextBlock->SetText(FText::FromString(User->Name));
    }
}
