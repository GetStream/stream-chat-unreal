// Copyright 2021 Stream.IO, Inc. All Rights Reserved.

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
    Channel = InChannel;

    Super::Setup();
}

void UFadingMessageListWidget::OnSetup()
{
    if (Channel)
    {
        Channel->MessagesUpdated.AddUniqueDynamic(this, &UFadingMessageListWidget::MessagesUpdated);
    }

    if (ChatInputBox)
    {
        ChatInputBox->OnTextCommitted.AddUniqueDynamic(this, &UFadingMessageListWidget::TextCommitted);
    }

    FOnInputAction Delegate;
    Delegate.BindDynamic(this, &UFadingMessageListWidget::OpenChat);
    ListenForInputAction(OpenChatInputActionName, IE_Pressed, false, Delegate);
}

void UFadingMessageListWidget::MessagesUpdated(const TArray<FMessage>& Messages)
{
    if (!MessagesScrollBox)
    {
        return;
    }

    MessagesScrollBox->ClearChildren();
    for (const FMessage& Message : UUiBlueprintLibrary::FilterRecent(Messages, MessageLifetime))
    {
        UFadingMessageWidget* Widget = CreateWidget<UFadingMessageWidget>(this, FadingMessageWidgetClass);
        Widget->Setup(Message, MessageLifetime);
        MessagesScrollBox->AddChild(Widget);
    }

    MessagesScrollBox->ScrollToEnd();
}
void UFadingMessageListWidget::TextCommitted(const FText& Text, ETextCommit::Type CommitMethod)
{
    if (CommitMethod != ETextCommit::OnEnter)
    {
        return;
    }

    if (!Text.IsEmpty())
    {
        const FMessage Message{Text.ToString()};
        Channel->SendMessage(Message);
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
