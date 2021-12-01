// Copyright 2021 Stream.IO, Inc. All Rights Reserved.

#include "Avatar/AvatarWidget.h"

#include "Algo/Transform.h"
#include "Avatar/ProfilePicWidget.h"
#include "Components/GridSlot.h"
#include "Materials/MaterialInstanceDynamic.h"
#include "User/User.h"
#include "WidgetUtil.h"

void UAvatarWidget::Setup(const TArray<FUserRef>& InUsers, const int32 InSize)
{
    Users = InUsers;
    if (InSize > 0)
    {
        Size = InSize;
    }

    Super::Setup();
}

void UAvatarWidget::OnSetup()
{
    if (SizeBox)
    {
        SizeBox->SetHeightOverride(Size);
        SizeBox->SetWidthOverride(Size);
    }

    if (Users.Num() == 1)
    {
        UpdateOnlineStatus(Users[0]->bOnline);
        Users[0].OnUpdate().AddUniqueDynamic(this, &UAvatarWidget::OnUserUpdated);
    }
    else
    {
        UpdateOnlineStatus(false);
    }

    CreateProfilePics();
}

void UAvatarWidget::UpdateOnlineStatus(const bool bOnline)
{
    if (!RetainerBox || !EffectMaterial)
    {
        return;
    }

    if (!EffectMaterialDynamic)
    {
        EffectMaterialDynamic = UMaterialInstanceDynamic::Create(EffectMaterial, this);
        RetainerBox->SetEffectMaterial(EffectMaterialDynamic);
    }

    EffectMaterialDynamic->SetScalarParameterValue(OnlineStatusMaterialParameterName, bOnline ? 1.f : 0.f);
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
        UGridSlot* GridSlot = Grid->AddChildToGrid(CreateProfilePic(Users[1]));
        GridSlot->SetColumn(1);
        Grid->SetColumnFill(0, 1.f);
        Grid->SetColumnFill(1, 1.f);
        Grid->SetRowFill(0, 1.f);
        return;
    }

    if (Users.Num() == 3)
    {
        {
            UGridSlot* GridSlot = Grid->AddChildToGrid(CreateProfilePic(Users[0]));
            GridSlot->SetRowSpan(2);
        }
        {
            UGridSlot* GridSlot = Grid->AddChildToGrid(CreateProfilePic(Users[1]));
            GridSlot->SetColumn(1);
        }
        {
            UGridSlot* GridSlot = Grid->AddChildToGrid(CreateProfilePic(Users[2]));
            GridSlot->SetRow(1);
            GridSlot->SetColumn(1);
        }
        Grid->SetColumnFill(0, 1.f);
        Grid->SetColumnFill(1, 1.f);
        Grid->SetRowFill(0, 1.f);
        Grid->SetRowFill(1, 1.f);
        return;
    }

    Grid->AddChildToGrid(CreateProfilePic(Users[0]));
    {
        UGridSlot* GridSlot = Grid->AddChildToGrid(CreateProfilePic(Users[1]));
        GridSlot->SetColumn(1);
    }
    {
        UGridSlot* GridSlot = Grid->AddChildToGrid(CreateProfilePic(Users[2]));
        GridSlot->SetRow(1);
    }
    {
        UGridSlot* GridSlot = Grid->AddChildToGrid(CreateProfilePic(Users[3]));
        GridSlot->SetRow(1);
        GridSlot->SetColumn(1);
    }
    Grid->SetColumnFill(0, 1.f);
    Grid->SetColumnFill(1, 1.f);
    Grid->SetRowFill(0, 1.f);
    Grid->SetRowFill(1, 1.f);
}

UProfilePicWidget* UAvatarWidget::CreateProfilePic(const FUserRef& User)
{
    UProfilePicWidget* Widget = CreateWidget<UProfilePicWidget>(this, ProfilePicWidgetClass);
    Widget->Setup(User);
    return Widget;
}

void UAvatarWidget::OnUserUpdated()
{
    if (Users.Num() == 1)
    {
        UpdateOnlineStatus(Users[0]->bOnline);
    }
}
