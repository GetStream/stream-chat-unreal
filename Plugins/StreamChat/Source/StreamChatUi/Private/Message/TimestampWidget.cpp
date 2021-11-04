#include "Message/TimestampWidget.h"

#include "Components/OverlaySlot.h"

void UTimestampWidget::Setup(const FMessage& InMessage, const EMessageSide InSide)
{
    Message = InMessage;
    Side = InSide;

    Super::Setup();
}

void UTimestampWidget::OnSetup()
{
    if (Side == EMessageSide::Me)
    {
        UserTextBlock->SetVisibility(ESlateVisibility::Collapsed);
        MessageStateIconImage->SetVisibility(ESlateVisibility::SelfHitTestInvisible);

        MessageStateIconImage->SetBrushFromTexture(GetStatusIcon(), true);
    }
    else if (Side == EMessageSide::You)
    {
        UserTextBlock->SetVisibility(ESlateVisibility::SelfHitTestInvisible);
        MessageStateIconImage->SetVisibility(ESlateVisibility::Collapsed);

        UserTextBlock->SetText(FText::FromString(Message.User.Id));
    }

    for (UPanelSlot* PanelSlot : OuterOverlay->GetSlots())
    {
        UOverlaySlot* OverlaySlot = CastChecked<UOverlaySlot>(PanelSlot);
        if (Side == EMessageSide::Me)
        {
            OverlaySlot->SetHorizontalAlignment(HAlign_Right);
        }
        else if (Side == EMessageSide::You)
        {
            OverlaySlot->SetHorizontalAlignment(HAlign_Left);
        }
    }

    DateTimeTextBlock->SetText(GetTimestampText());
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
    if (Message.bIsRead)
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
