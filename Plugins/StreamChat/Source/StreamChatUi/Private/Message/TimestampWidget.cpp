// Copyright 2022 Stream.IO, Inc. All Rights Reserved.

#include "Message/TimestampWidget.h"

#include "Channel/ChannelState.h"
#include "Channel/ChatChannel.h"
#include "ThemeDataAsset.h"
#include "User/User.h"
#include "WidgetUtil.h"

UTimestampWidget::UTimestampWidget()
{
    bWantsTheme = true;
    bWantsChannel = true;
}

void UTimestampWidget::Setup(const FMessage& InMessage, const bool bInShowUserName, const bool bInShowMessageState)
{
    Message = InMessage;
    bShowUserName = bInShowUserName;
    bShowMessageState = bInShowMessageState;

    Super::Setup();
}

void UTimestampWidget::OnSetup()
{
    if (UserTextBlock)
    {
        if (bShowUserName)
        {
            UserTextBlock->SetVisibility(ESlateVisibility::SelfHitTestInvisible);
            UserTextBlock->SetText(FText::FromString(Message.User->Name.Len() > 0 ? Message.User->Name : Message.User->Id));
        }
        else
        {
            UserTextBlock->SetVisibility(ESlateVisibility::Collapsed);
        }
    }

    if (DateTimeTextBlock)
    {
        DateTimeTextBlock->SetText(GetTimestampText());
    }
}

void UTimestampWidget::OnTheme()
{
    if (UserTextBlock)
    {
        if (Theme->bColoredName)
        {
            const FLinearColor Color = WidgetUtil::ChooseColorForString(Message.User->Id);
            UserTextBlock->SetColorAndOpacity(Color);
        }
    }

    if (DateTimeTextBlock)
    {
        DateTimeTextBlock->SetColorAndOpacity(Theme->GetPaletteColor(Theme->TimestampTextColor));
    }
}

void UTimestampWidget::OnChannel()
{
    if (MessageStateIconImage)
    {
        if (bShowMessageState)
        {
            MessageStateIconImage->SetVisibility(ESlateVisibility::SelfHitTestInvisible);
            MessageStateIconImage->SetBrushFromTexture(GetStatusIcon(), true);
        }
        else
        {
            MessageStateIconImage->SetVisibility(ESlateVisibility::Collapsed);
        }
    }
}

UTexture2D* UTimestampWidget::GetStatusIcon() const
{
    // 1 check - Message sent, but not received by the recipient.
    // 2 blue checks - Message read by recipient.
    // No icons - App failed to send the message (coupled with exclamation mark badge).
    // Clock icon - Message pending to be sent, when user sends a message while there is no network.

    if (Message.State == EMessageSendState::Failed)
    {
        return nullptr;
    }
    if (Message.State != EMessageSendState::Sent)
    {
        return IconClock;
    }
    if (Channel->State.IsMessageRead(Message))
    {
        return IconCheckAll;
    }
    return IconCheck;
}

FText UTimestampWidget::GetTimestampText() const
{
    // TODO Should be able to use ICU to get internationalized relative dates for things like "Yesterday" and "Today"
    // https://github.com/unicode-org/icu/blob/main/icu4c/source/i18n/unicode/reldatefmt.h

    // Messages from today show the time.
    // Messages from yesterday show Yesterday.
    // Messages from last week show the week day (eg. Thursday).
    // Messages from before that, show the date with the system format (so either dd/mm/yy or mm/dd/yy).
    // In general we should use the system format for date and time.
    const FDateTime Now = FDateTime::UtcNow();
    const FDateTime Today = Now.GetDate();
    const FDateTime Date = Message.CreatedAt.GetDate();
    const FCultureRef Culture = FInternationalization::Get().GetCurrentLocale();
    if (Date == Today)
    {
        return FText::AsTime(Message.CreatedAt, EDateTimeStyle::Short, TEXT(""), Culture);
    }
    const FDateTime Yesterday = Today - FTimespan::FromDays(1);
    if (Date == Yesterday)
    {
        return FText::FromString(TEXT("Yesterday"));
    }
    if (Now - Message.CreatedAt < FTimespan::FromDays(7))
    {
        return GetDayOfWeek(Now);
    }
    return FText::AsDate(Message.CreatedAt, EDateTimeStyle::Short, TEXT(""), Culture);
}

FText UTimestampWidget::GetDayOfWeek(const FDateTime& DateTime)
{
    // TODO use ICU to do this in a translated way
    switch (DateTime.GetDayOfWeek())
    {
        case EDayOfWeek::Monday:
            return FText::FromString(TEXT("Monday"));
        case EDayOfWeek::Tuesday:
            return FText::FromString(TEXT("Tuesday"));
        case EDayOfWeek::Wednesday:
            return FText::FromString(TEXT("Wednesday"));
        case EDayOfWeek::Thursday:
            return FText::FromString(TEXT("Thursday"));
        case EDayOfWeek::Friday:
            return FText::FromString(TEXT("Friday"));
        case EDayOfWeek::Saturday:
            return FText::FromString(TEXT("Saturday"));
        case EDayOfWeek::Sunday:
            return FText::FromString(TEXT("Sunday"));
    }
    return FText::GetEmpty();
}
