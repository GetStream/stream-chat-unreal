// Copyright 2022 Stream.IO, Inc. All Rights Reserved.

#include "Avatar/NamedAvatarWidget.h"

#include "ThemeDataAsset.h"
#include "User/User.h"
#include "WidgetUtil.h"

UNamedAvatarWidget::UNamedAvatarWidget()
{
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
            TextBlock->SetText(FText::FromString(User->Name.Len() > 0 ? User->Name : User->Id));
        }
    }
}

void UNamedAvatarWidget::NativePreConstruct()
{
    Super::NativePreConstruct();
    if (!User.IsCurrent() && GetTheme()->bColoredName)
    {
        const FLinearColor Color = WidgetUtil::ChooseColorForString(User->Id);
        TextBlock->SetColorAndOpacity(Color);
    }
    else
    {
        TextBlock->SetColorAndOpacity(GetTheme()->GetPaletteColor(GetTheme()->NamedAvatarTextColor));
    }
}
