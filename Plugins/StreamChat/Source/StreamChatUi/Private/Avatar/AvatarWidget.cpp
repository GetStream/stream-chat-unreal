// Copyright Stream.IO, Inc. All Rights Reserved.

#include "Avatar/AvatarWidget.h"

#include "Avatar/ProfilePicWidget.h"
#include "Components/GridSlot.h"
#include "WidgetUtil.h"

void UAvatarWidget::Setup(const TArray<FUser>& InUsers)
{
    Users = InUsers;

    Super::Setup();
}

void UAvatarWidget::OnSetup()
{
    CreateProfilePics();
}

void UAvatarWidget::CreateProfilePics()
{
    if (!Grid || Users.Num() == 0)
    {
        return;
    }

    Grid->ClearChildren();
    if (Users.Num() == 1)
    {
        Grid->AddChildToGrid(CreateProfilePic(Users[0]));
        Grid->SetColumnFill(0, 1.f);
        Grid->SetRowFill(0, 1.f);
        return;
    }

    if (Users.Num() == 2)
    {
        Grid->AddChildToGrid(CreateProfilePic(Users[0]));
        UGridSlot* Slot = Grid->AddChildToGrid(CreateProfilePic(Users[1]));
        Slot->SetColumn(1);
        Grid->SetColumnFill(0, 1.f);
        Grid->SetColumnFill(1, 1.f);
        Grid->SetRowFill(0, 1.f);
        return;
    }

    if (Users.Num() == 3)
    {
        {
            UGridSlot* Slot = Grid->AddChildToGrid(CreateProfilePic(Users[0]));
            Slot->SetRowSpan(2);
        }
        {
            UGridSlot* Slot = Grid->AddChildToGrid(CreateProfilePic(Users[1]));
            Slot->SetColumn(1);
        }
        {
            UGridSlot* Slot = Grid->AddChildToGrid(CreateProfilePic(Users[2]));
            Slot->SetRow(1);
            Slot->SetColumn(1);
        }
        Grid->SetColumnFill(0, 1.f);
        Grid->SetColumnFill(1, 1.f);
        Grid->SetRowFill(0, 1.f);
        Grid->SetRowFill(1, 1.f);
    }

    Grid->AddChildToGrid(CreateProfilePic(Users[0]));
    {
        UGridSlot* Slot = Grid->AddChildToGrid(CreateProfilePic(Users[1]));
        Slot->SetColumn(1);
    }
    {
        UGridSlot* Slot = Grid->AddChildToGrid(CreateProfilePic(Users[2]));
        Slot->SetRow(1);
    }
    {
        UGridSlot* Slot = Grid->AddChildToGrid(CreateProfilePic(Users[3]));
        Slot->SetRow(1);
        Slot->SetColumn(1);
    }
    Grid->SetColumnFill(0, 1.f);
    Grid->SetColumnFill(1, 1.f);
    Grid->SetRowFill(0, 1.f);
    Grid->SetRowFill(1, 1.f);
}

UProfilePicWidget* UAvatarWidget::CreateProfilePic(const FUser& User)
{
    UProfilePicWidget* Widget = CreateWidget<UProfilePicWidget>(this, ProfilePicWidgetClass);
    Widget->Setup(User);
    return Widget;
}
