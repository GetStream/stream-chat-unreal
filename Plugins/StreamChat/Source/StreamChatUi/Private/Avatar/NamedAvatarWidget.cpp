// Copyright 2022 Stream.IO, Inc. All Rights Reserved.

#include "Avatar/NamedAvatarWidget.h"

#include "ThemeDataAsset.h"
#include "User/User.h"
#include "WidgetUtil.h"

UNamedAvatarWidget::UNamedAvatarWidget()
{
    bWantsTheme = true;
}

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

void UNamedAvatarWidget::OnTheme()
{
    if (!User.IsCurrent() && Theme->bColoredName)
    {
        const FLinearColor Color = WidgetUtil::ChooseColorForString(User->Id);
        TextBlock->SetColorAndOpacity(Color);
    }
    else
    {
        TextBlock->SetColorAndOpacity(Theme->GetPaletteColor(Theme->NamedAvatarTextColor));
    }
}
