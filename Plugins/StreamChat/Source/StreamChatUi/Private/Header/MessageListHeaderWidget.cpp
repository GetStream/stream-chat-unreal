// Copyright 2022 Stream.IO, Inc. All Rights Reserved.

#include "Header/MessageListHeaderWidget.h"

#include "UiBlueprintLibrary.h"

UMessageListHeaderWidget::UMessageListHeaderWidget()
{
}

void UMessageListHeaderWidget::NativePreConstruct()
{
    Super::NativePreConstruct();

    if (GetChannel())
    {
        GetChannel()->OnTypingIndicator.AddDynamic(this, &UMessageListHeaderWidget::OnTypingIndicator);

        if (Header)
        {
            const FText Title = FText::FromString(UUiBlueprintLibrary::GetChannelTitle(GetChannel()));
            Header->SetTitle(Title);
            ShowOnlineStatusSubheader();
        }

        if (Avatar)
        {
            const TOptional<FString> Image = GetChannel()->Properties.GetImageUrl();
            if (Image.IsSet())
            {
                Avatar->SetupWithUrl(Image.GetValue());
            }
            else
            {
                Avatar->Setup(GetChannel()->Properties.GetOtherMemberUsers());
            }
        }
    }
}

void UMessageListHeaderWidget::OnTypingIndicator(const ETypingIndicatorState TypingState, const FUserRef& User)
{
    if (User.IsCurrent() || !Header)
    {
        return;
    }

    if (TypingState == ETypingIndicatorState::StartTyping)
    {
        if (!TypingIndicator)
        {
            TypingIndicator = CreateWidget<UTypingIndicatorWidget>(this, TypingIndicatorWidgetClass);
            Header->SetSubtitleContent(TypingIndicator);
        }
        TypingIndicator->AddUser(User);
    }
    else if (TypingState == ETypingIndicatorState::StopTyping)
    {
        if (TypingIndicator)
        {
            const bool bIsEmpty = TypingIndicator->RemoveUser(User);
            if (bIsEmpty)
            {
                ShowOnlineStatusSubheader();
            }
        }
    }
}

void UMessageListHeaderWidget::ShowOnlineStatusSubheader()
{
    if (!Header)
    {
        return;
    }

    UOnlineStatusSubheaderWidget* Widget = CreateWidget<UOnlineStatusSubheaderWidget>(this, StatusWidgetClass);
    Header->SetSubtitleContent(Widget);
    TypingIndicator = nullptr;
}
