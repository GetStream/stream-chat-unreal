// Copyright 2026 Stream.IO, Inc. All Rights Reserved.

#include "Header/MessageListHeaderWidget.h"

#include "Context/ChannelContextWidget.h"
#include "UiBlueprintLibrary.h"

UMessageListHeaderWidget::UMessageListHeaderWidget()
{
}

void UMessageListHeaderWidget::NativePreConstruct()
{
    Super::NativePreConstruct();

    if (GetChannel())
    {
        if (Header)
        {
            RefreshTitle();
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

void UMessageListHeaderWidget::NativeConstruct()
{
    Super::NativeConstruct();
    if (GetChannel())
    {
        GetChannel()->OnTypingIndicator.AddDynamic(this, &UMessageListHeaderWidget::OnTypingIndicator);
    }
    if (UChannelContextWidget* Context = UChannelContextWidget::TryGet(this))
    {
        Context->OnThreadChanged.AddDynamic(this, &UMessageListHeaderWidget::OnThreadChanged);
    }
}

void UMessageListHeaderWidget::NativeDestruct()
{
    if (GetChannel())
    {
        GetChannel()->OnTypingIndicator.RemoveDynamic(this, &UMessageListHeaderWidget::OnTypingIndicator);
    }
    if (UChannelContextWidget* Context = UChannelContextWidget::TryGet(this))
    {
        Context->OnThreadChanged.RemoveDynamic(this, &UMessageListHeaderWidget::OnThreadChanged);
    }
    Super::NativeDestruct();
}

void UMessageListHeaderWidget::OnThreadChanged(bool, const FMessage&)
{
    RefreshTitle();
}

void UMessageListHeaderWidget::RefreshTitle()
{
    if (!Header || !GetChannel())
    {
        return;
    }

    const UChannelContextWidget* Context = UChannelContextWidget::TryGet(this);
    const bool bThreadOpen = Context && Context->IsThreadOpen();
    // Being in a thread is easy to lose track of, and the composer banner alone is easy to miss
    const FText Title = bThreadOpen ? ThreadTitleText : FText::FromString(UUiBlueprintLibrary::GetChannelTitle(GetChannel()));
    Header->SetTitle(Title);
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
