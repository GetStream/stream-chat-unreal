#include "Message/TimestampWidget.h"

#include "Components/OverlaySlot.h"
#include "Components/PanelWidget.h"
#include "Framework/Application/MenuStack.h"
#include "Framework/Application/SlateApplication.h"

UTimestampWidget::UTimestampWidget(const FObjectInitializer& ObjectInitializer) : UUserWidget(ObjectInitializer)
{
    // Ensure hovering events are fired
    UUserWidget::SetVisibility(ESlateVisibility::Visible);
}

void UTimestampWidget::Setup(const FMessage& InMessage, const EBubbleStackSide InSide)
{
    Message = InMessage;
    Side = InSide;
}

void UTimestampWidget::NativeOnInitialized()
{
    OptionsButton->OnClicked.AddDynamic(this, &UTimestampWidget::OnOptionsButtonClicked);
    ReactionButton->OnClicked.AddDynamic(this, &UTimestampWidget::OnReactionButtonClicked);

    Super::NativeOnInitialized();
}

void UTimestampWidget::NativePreConstruct()
{
    HoverGroup->SetVisibility(ESlateVisibility::Collapsed);

    OptionsButton->WidgetStyle.NormalPadding = {};
    OptionsButton->WidgetStyle.PressedPadding = {};
    ReactionButton->WidgetStyle.NormalPadding = {};
    ReactionButton->WidgetStyle.PressedPadding = {};

    if (Side == EBubbleStackSide::Me)
    {
        UserTextBlock->SetVisibility(ESlateVisibility::Collapsed);
        MessageStateIconImage->SetVisibility(ESlateVisibility::SelfHitTestInvisible);

        // TODO This doesn't work
        HoverGroup->ReplaceChildAt(0, OptionsButton);
        HoverGroup->ReplaceChildAt(1, ReactionButton);

        if (UOverlaySlot* HoverGroupSlot = Cast<UOverlaySlot>(HoverGroup->Slot))
        {
            HoverGroupSlot->SetHorizontalAlignment(HAlign_Left);
        }
        if (UOverlaySlot* InfoGroupSlot = Cast<UOverlaySlot>(InfoGroup->Slot))
        {
            InfoGroupSlot->SetHorizontalAlignment(HAlign_Right);
        }

        MessageStateIconImage->SetBrushFromTexture(GetStatusIcon(), true);
    }
    else if (Side == EBubbleStackSide::You)
    {
        UserTextBlock->SetVisibility(ESlateVisibility::SelfHitTestInvisible);
        MessageStateIconImage->SetVisibility(ESlateVisibility::Collapsed);

        // TODO This doesn't work
        HoverGroup->ReplaceChildAt(0, ReactionButton);
        HoverGroup->ReplaceChildAt(1, OptionsButton);

        if (UOverlaySlot* HoverGroupSlot = Cast<UOverlaySlot>(HoverGroup->Slot))
        {
            HoverGroupSlot->SetHorizontalAlignment(HAlign_Right);
        }
        if (UOverlaySlot* InfoGroupSlot = Cast<UOverlaySlot>(InfoGroup->Slot))
        {
            InfoGroupSlot->SetHorizontalAlignment(HAlign_Left);
        }

        UserTextBlock->SetText(FText::FromString(Message.User.Id));
    }

    DateTimeTextBlock->SetText(GetTimestampText());

    Super::NativePreConstruct();
}

void UTimestampWidget::OnOptionsButtonClicked()
{
    UWidget* Widget = CreateWidget(this, ContextMenuWidgetClass);
    static constexpr bool bFocusImmediately = true;
    TSharedPtr<IMenu> ContextMenu = FSlateApplication::Get().PushMenu(
        TakeWidget(),
        {},
        Widget->TakeWidget(),
        OptionsButton->GetCachedGeometry().GetAbsolutePosition(),
        FPopupTransitionEffect(FPopupTransitionEffect::ContextMenu),
        bFocusImmediately);
}

void UTimestampWidget::OnReactionButtonClicked()
{
    UWidget* Widget = CreateWidget(this, ReactionPickerWidgetClass);
    static constexpr bool bFocusImmediately = true;
    TSharedPtr<IMenu> ContextMenu = FSlateApplication::Get().PushMenu(
        TakeWidget(),
        {},
        Widget->TakeWidget(),
        ReactionButton->GetCachedGeometry().GetAbsolutePosition(),
        FPopupTransitionEffect(FPopupTransitionEffect::ContextMenu),
        bFocusImmediately);
}

void UTimestampWidget::NativeOnMouseEnter(const FGeometry& InGeometry, const FPointerEvent& InMouseEvent)
{
    HoverGroup->SetVisibility(ESlateVisibility::SelfHitTestInvisible);
    Super::NativeOnMouseEnter(InGeometry, InMouseEvent);
}

void UTimestampWidget::NativeOnMouseLeave(const FPointerEvent& InMouseEvent)
{
    HoverGroup->SetVisibility(ESlateVisibility::Collapsed);
    Super::NativeOnMouseLeave(InMouseEvent);
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
            return FText::FromString(TEXT("Thurday"));
        case EDayOfWeek::Friday:
            return FText::FromString(TEXT("Friday"));
        case EDayOfWeek::Saturday:
            return FText::FromString(TEXT("Saturday"));
        case EDayOfWeek::Sunday:
            return FText::FromString(TEXT("Sunday"));
    }
    return FText::GetEmpty();
}
