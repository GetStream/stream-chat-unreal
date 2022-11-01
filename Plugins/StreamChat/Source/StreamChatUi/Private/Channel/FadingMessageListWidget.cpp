// Copyright 2022 Stream.IO, Inc. All Rights Reserved.

#include "Channel/FadingMessageListWidget.h"

#include "UiBlueprintLibrary.h"

void UFadingMessageListWidget::Setup(UChatChannel* InChannel, const FTimespan& InMessageLifetime)
{
    if (InMessageLifetime.GetTicks() != 0)
    {
        MessageLifetime = InMessageLifetime;
    }

    Setup(InChannel);
}

void UFadingMessageListWidget::Setup(UChatChannel* InChannel)
{
    MyChannel = InChannel;

    Super::Setup();
}

void UFadingMessageListWidget::NativeConstruct()
{
    Super::NativeConstruct();
    if (MyChannel)
    {
        MyChannel->MessagesUpdated.AddDynamic(this, &UFadingMessageListWidget::MessagesUpdated);
    }

    if (ChatInputBox)
    {
        ChatInputBox->OnTextCommitted.AddDynamic(this, &UFadingMessageListWidget::TextCommitted);
    }
    FOnInputAction Delegate;
    Delegate.BindDynamic(this, &UFadingMessageListWidget::OpenChat);
    ListenForInputAction(OpenChatInputActionName, IE_Pressed, false, Delegate);
}

void UFadingMessageListWidget::NativeDestruct()
{
    if (MyChannel)
    {
        MyChannel->MessagesUpdated.RemoveDynamic(this, &UFadingMessageListWidget::MessagesUpdated);
    }

    if (ChatInputBox)
    {
        ChatInputBox->OnTextCommitted.RemoveDynamic(this, &UFadingMessageListWidget::TextCommitted);
    }
    StopListeningForInputAction(OpenChatInputActionName, IE_Pressed);
    Super::NativeDestruct();
}

void UFadingMessageListWidget::MessagesUpdated()
{
    if (!MessagesScrollBox || !MyChannel)
    {
        return;
    }

    MessagesScrollBox->ClearChildren();
    for (const FMessageRef& Message : MyChannel->State.Messages.FilterRecent(MessageLifetime))
    {
        UFadingMessageWidget* Widget = CreateWidget<UFadingMessageWidget>(this, FadingMessageWidgetClass);
        Widget->Setup(*Message, MessageLifetime);
        MessagesScrollBox->AddChild(Widget);
    }

    MessagesScrollBox->ScrollToEnd();
}
void UFadingMessageListWidget::TextCommitted(const FText& Text, const ETextCommit::Type CommitMethod)
{
    if (CommitMethod != ETextCommit::OnEnter)
    {
        return;
    }

    if (!Text.IsEmpty())
    {
        const FMessage Message{Text.ToString()};
        MyChannel->SendMessage(Message);
    }

    CloseChat();
}

void UFadingMessageListWidget::OpenChat()
{
    if (ChatInputBox)
    {
        FInputModeUIOnly Mode;
        Mode.SetWidgetToFocus(ChatInputBox->TakeWidget());
        Mode.SetLockMouseToViewportBehavior(EMouseLockMode::DoNotLock);
        GetOwningPlayer()->SetInputMode(Mode);
    }

    if (ChatInputVisual)
    {
        ChatInputVisual->SetVisibility(ESlateVisibility::Visible);
    }
}

void UFadingMessageListWidget::CloseChat()
{
    GetOwningPlayer()->SetInputMode(FInputModeGameOnly());

    if (ChatInputVisual)
    {
        ChatInputVisual->SetVisibility(ESlateVisibility::Hidden);
    }

    if (ChatInputBox)
    {
        ChatInputBox->SetText(FText::GetEmpty());
    }
}
