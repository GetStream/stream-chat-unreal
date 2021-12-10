// Copyright 2021 Stream.IO, Inc. All Rights Reserved.

#include "Header/MessageListHeaderWidget.h"

#include "Algo/Transform.h"
#include "Context/ChannelContextWidget.h"
#include "UiBlueprintLibrary.h"

void UMessageListHeaderWidget::OnChannel(UChatChannel*)
{
    Channel->OnTypingIndicator.AddDynamic(this, &UMessageListHeaderWidget::OnTypingIndicator);

    if (Header)
    {
        const FText Title = FText::FromString(UUiBlueprintLibrary::GetChannelTitle(Channel));
        Header->SetTitle(Title);
        ShowOnlineStatusSubheader();
    }

    if (Avatar)
    {
        // TODO Group chat
        Avatar->Setup(Channel->State.GetOtherMemberUsers());
    }
}

void UMessageListHeaderWidget::NativeDestruct()
{
    if (Channel)
    {
        Channel->OnTypingIndicator.RemoveDynamic(this, &UMessageListHeaderWidget::OnTypingIndicator);
    }
    Super::NativeDestruct();
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
