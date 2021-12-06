// Copyright 2021 Stream.IO, Inc. All Rights Reserved.

#include "Avatar/NamedAvatarWidget.h"

#include "User/User.h"
#include "WidgetUtil.h"

void UNamedAvatarWidget::Setup(const FUserRef& InUser, bool bInColoredName)
{
    User = InUser;
    bColoredName = bInColoredName;

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
            if (bColoredName)
            {
                const FLinearColor Color = WidgetUtil::ChooseColorForString(User->Id);
                TextBlock->SetColorAndOpacity(Color);
            }
        }
    }
}
