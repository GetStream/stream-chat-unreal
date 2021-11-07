// Copyright Stream.IO, Inc. All Rights Reserved.

#include "Avatar/AvatarWidget.h"

#include "Algo/Transform.h"
#include "Avatar/ProfilePicWidget.h"
#include "Components/GridSlot.h"
#include "Context/ChannelContextWidget.h"
#include "Materials/MaterialInstanceDynamic.h"
#include "WidgetUtil.h"

void UAvatarWidget::Setup(const TArray<FString>& InUserIds, const int32 InSize)
{
    UserIds = InUserIds;
    Size = InSize;

    Super::Setup();
}

void UAvatarWidget::OnSetup()
{
    if (SizeBox)
    {
        SizeBox->SetHeightOverride(Size);
        SizeBox->SetWidthOverride(Size);
    }

    if (UChatChannel* Channel = UChannelContextWidget::GetChannel(this))
    {
        // TODO subscribe to status events
        const auto Matching = [&](const FMember& M)
        {
            return UserIds.Contains(M.User.Id);
        };
        constexpr auto ToUser = [](const FMember& M)
        {
            return M.User;
        };
        TArray<FUser> Users;
        Algo::TransformIf(Channel->State.Members, Users, Matching, ToUser);

        if (Users.Num() == 1)
        {
            UpdateOnlineStatus(Users[0].bOnline);
        }
        else
        {
            UpdateOnlineStatus(false);
        }

        CreateProfilePics(Users);
    }
}

void UAvatarWidget::UpdateOnlineStatus(const bool bOnline) const
{
    if (!RetainerBox)
    {
        return;
    }
    if (UMaterialInstanceDynamic* Mat = RetainerBox->GetEffectMaterial())
    {
        Mat->SetScalarParameterValue(OnlineStatusMaterialParameterName, bOnline ? 1.f : 0.f);
    }
}

void UAvatarWidget::CreateProfilePics(const TArray<FUser>& Users)
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
