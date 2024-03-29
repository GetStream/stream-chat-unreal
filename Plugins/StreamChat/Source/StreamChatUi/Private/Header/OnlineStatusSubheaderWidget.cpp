// Copyright 2022 Stream.IO, Inc. All Rights Reserved.

#include "Header/OnlineStatusSubheaderWidget.h"

#include "Algo/Count.h"
#include "Context/ChannelContextWidget.h"
#include "TimerManager.h"
#include "User/User.h"

UOnlineStatusSubheaderWidget::UOnlineStatusSubheaderWidget()
{
}

void UOnlineStatusSubheaderWidget::NativePreConstruct()
{
    Super::NativePreConstruct();
    UpdateLabel();
}

void UOnlineStatusSubheaderWidget::NativeConstruct()
{
    Super::NativeConstruct();
    // TODO subscribe to user presence events

    GetWorld()->GetTimerManager().SetTimer(Handle, this, &UOnlineStatusSubheaderWidget::UpdateLabel, 60.f, true);
}

void UOnlineStatusSubheaderWidget::NativeDestruct()
{
    GetWorld()->GetTimerManager().ClearTimer(Handle);

    Super::NativeDestruct();
}

void UOnlineStatusSubheaderWidget::UpdateLabel() const
{
    if (TextBlock)
    {
        TextBlock->SetText(GetLabel());
    }
}

FText UOnlineStatusSubheaderWidget::GetLabel() const
{
    if (!GetChannel())
    {
        return FText::GetEmpty();
    }

    if (GetChannel()->Properties.Members.Num() <= 2)
    {
        if (const FMember* Member = GetChannel()->Properties.Members.FindByPredicate([this](const FMember& M) { return !M.User.IsCurrent(); }))
        {
            return GetSingleUserLabel(Member->User);
        }
    }
    return GetMultiUserLabel();
}

FText UOnlineStatusSubheaderWidget::GetSingleUserLabel(const FUserRef& User) const
{
    if (!User.IsValid())
    {
        return FText::GetEmpty();
    }

    return User->GetLastSeenText();
}

FText UOnlineStatusSubheaderWidget::GetMultiUserLabel() const
{
    const uint32 TotalCount = GetChannel()->Properties.Members.Num();
    const uint32 OnlineCount = Algo::CountIf(GetChannel()->Properties.Members, [](const FMember& M) { return M.User->bOnline; });

    FFormatNamedArguments Args;
    Args.Add(TEXT("TotalCount"), TotalCount);
    Args.Add(TEXT("OnlineCount"), OnlineCount);
    return FText::Format(FText::FromString(TEXT("{TotalCount} Members, {OnlineCount} Online")), Args);
}
