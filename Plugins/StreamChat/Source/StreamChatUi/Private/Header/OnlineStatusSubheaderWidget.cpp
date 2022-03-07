// Copyright 2022 Stream.IO, Inc. All Rights Reserved.

#include "Header/OnlineStatusSubheaderWidget.h"

#include "Algo/Count.h"
#include "Context/ChannelContextWidget.h"
#include "TimerManager.h"
#include "User/User.h"

UOnlineStatusSubheaderWidget::UOnlineStatusSubheaderWidget()
{
    bWantsChannel = true;
}

void UOnlineStatusSubheaderWidget::OnChannel()
{
    // TODO subscribe to user presence events

    UpdateLabel();
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
    if (Channel->Properties.Members.Num() <= 2)
    {
        if (const FMember* Member = Channel->Properties.Members.FindByPredicate([this](const FMember& M) { return !M.User.IsCurrent(); }))
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

    if (User->bOnline)
    {
        return FText::FromString(TEXT("Online"));
    }

    const FDateTime& LastActive = GetLastActive(User);
    const FDateTime Now = FDateTime::UtcNow();
    const FTimespan Delta = Now - LastActive;
    if (Delta < FTimespan::FromMinutes(1.f))
    {
        return FText::FromString(TEXT("Last seen just now"));
    }
    // TODO RelativeDateTimeFormatter
    FFormatNamedArguments Args;
    if (Delta < FTimespan::FromHours(1.f))
    {
        Args.Add(TEXT("Num"), FMath::FloorToInt(Delta.GetTotalMinutes()));
        Args.Add(TEXT("Unit"), FText::FromString(TEXT("minute")));
        Args.Add(TEXT("Units"), FText::FromString(TEXT("minutes")));
    }
    else if (Delta < FTimespan::FromDays(1.f))
    {
        Args.Add(TEXT("Num"), FMath::FloorToInt(Delta.GetTotalHours()));
        Args.Add(TEXT("Unit"), FText::FromString(TEXT("hour")));
        Args.Add(TEXT("Units"), FText::FromString(TEXT("hours")));
    }
    else
    {
        Args.Add(TEXT("Num"), FMath::FloorToInt(Delta.GetTotalDays()));
        Args.Add(TEXT("Unit"), FText::FromString(TEXT("day")));
        Args.Add(TEXT("Units"), FText::FromString(TEXT("days")));
    }

    return FText::Format(FText::FromString(TEXT("Last seen {Num} {Num}|plural(one={Unit},other={Units}) ago")), Args);
}

FText UOnlineStatusSubheaderWidget::GetMultiUserLabel() const
{
    const uint32 TotalCount = Channel->Properties.Members.Num();
    const uint32 OnlineCount = Algo::CountIf(Channel->Properties.Members, [](const FMember& M) { return M.User->bOnline; });

    FFormatNamedArguments Args;
    Args.Add(TEXT("TotalCount"), TotalCount);
    Args.Add(TEXT("OnlineCount"), OnlineCount);
    return FText::Format(FText::FromString(TEXT("{TotalCount} Members, {OnlineCount} Online")), Args);
}

const FDateTime& UOnlineStatusSubheaderWidget::GetLastActive(const FUserRef& User)
{
    static FDateTime None{0};
    if (!User.IsValid())
    {
        return None;
    }

    if (User->LastActive.GetTicks() > 0)
    {
        return User->LastActive;
    }
    if (User->UpdatedAt.GetTicks() > 0)
    {
        return User->LastActive;
    }
    return User->CreatedAt;
}
