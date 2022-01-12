// Copyright 2021 Stream.IO, Inc. All Rights Reserved.

#include "Input/MessageComposerWidget.h"

#include "Brushes/SlateImageBrush.h"
#include "Context/ChannelContextWidget.h"
#include "ThemeDataAsset.h"
#include "TimerManager.h"

void UMessageComposerWidget::NativeOnInitialized()
{
    if (MessageInput)
    {
        MessageInput->OnTextChanged.AddDynamic(this, &UMessageComposerWidget::OnInputTextChanged);
        MessageInput->OnTextCommitted.AddDynamic(this, &UMessageComposerWidget::OnInputTextCommit);
    }
    if (CancelEditingButton)
    {
        CancelEditingButton->OnClicked.AddDynamic(this, &UMessageComposerWidget::OnCancelEditingButtonClicked);
    }
    if (SendMessageButton)
    {
        SendMessageButton->OnClicked.AddDynamic(this, &UMessageComposerWidget::OnSendButtonClicked);
    }

    Super::NativeOnInitialized();
}

void UMessageComposerWidget::NativeConstruct()
{
    // Can only find the ChannelContextWidget once this widget is added to the UI hierarchy (Construct)
    UChannelContextWidget::Get(this)->OnStartEditMessage.AddDynamic(this, &UMessageComposerWidget::EditMessage);

    if (MessageInput)
    {
        MessageInput->SetKeyboardFocus();
    }

    if (const UThemeDataAsset* Theme = UThemeDataAsset::Get(this))
    {
        if (BackgroundBorder)
        {
            BackgroundBorder->SetBrushColor(Theme->GetPaletteColor(Theme->MessageComposerBackgroundColor));
        }
        if (EditMessageTextBlock)
        {
            EditMessageTextBlock->SetColorAndOpacity(Theme->GetPaletteColor(Theme->MessageComposerHeaderTextColor));
        }
        if (TopBorderImage)
        {
            TopBorderImage->SetColorAndOpacity(Theme->GetPaletteColor(Theme->MessageComposerBorderColor));
        }
    }

    UpdateSendButtonAppearance(false);
    UpdateEditMessageAppearance(ESendButtonIconAppearance::Send);

    Super::NativeConstruct();
}

void UMessageComposerWidget::NativeDestruct()
{
    UChannelContextWidget::Get(this)->OnStartEditMessage.RemoveDynamic(this, &UMessageComposerWidget::EditMessage);
    Super::NativeDestruct();
}

void UMessageComposerWidget::EditMessage(const FMessage& Message)
{
    EditedMessage = Message;
    if (MessageInput)
    {
        MessageInput->SetText(FText::FromString(Message.Text));
    }
    UpdateEditMessageAppearance(ESendButtonIconAppearance::Confirm);
}

void UMessageComposerWidget::OnInputTextChanged(const FText& Text)
{
    if (UChatChannel* Channel = UChannelContextWidget::GetChannel(this); ensure(Channel))
    {
        Channel->KeyStroke();
    }

    UpdateSendButtonAppearance(!Text.IsEmpty());
}

void UMessageComposerWidget::OnInputTextCommit(const FText&, ETextCommit::Type CommitMethod)
{
    if (CommitMethod == ETextCommit::OnEnter)
    {
        SendMessage();

        if (MessageInput)
        {
            GetWorld()->GetTimerManager().SetTimerForNextTick([&] { MessageInput->SetKeyboardFocus(); });
        }
    }

    if (UChatChannel* Channel = UChannelContextWidget::GetChannel(this))
    {
        Channel->StopTyping();
    }
}

void UMessageComposerWidget::OnCancelEditingButtonClicked()
{
    StopEditMessage();
}

void UMessageComposerWidget::OnSendButtonClicked()
{
    SendMessage();

    if (UChatChannel* Channel = UChannelContextWidget::GetChannel(this))
    {
        Channel->StopTyping();
    }
}

void UMessageComposerWidget::SendMessage()
{
    if (!MessageInput)
    {
        return;
    }

    const FString Text = MessageInput->GetText().ToString();
    if (Text.IsEmpty())
    {
        return;
    }
    UChatChannel* Channel = UChannelContextWidget::GetChannel(this);
    if (!Channel)
    {
        return;
    }

    if (EditedMessage)
    {
        EditedMessage->Text = Text;
        Channel->UpdateMessage(*EditedMessage);
        StopEditMessage();
    }
    else
    {
        Channel->SendMessage(FMessage{Text});
        MessageInput->SetText(FText::GetEmpty());
    }
}

void UMessageComposerWidget::UpdateSendButtonAppearance(const bool bEnabled)
{
    SendMessageButton->SetEnabled(bEnabled);
}

void UMessageComposerWidget::StopEditMessage()
{
    EditedMessage.Reset();

    UpdateEditMessageAppearance(ESendButtonIconAppearance::Send);

    if (MessageInput)
    {
        MessageInput->SetText(FText::GetEmpty());
    }
}

void UMessageComposerWidget::UpdateEditMessageAppearance(const ESendButtonIconAppearance Appearance)
{
    if (SendMessageButton)
    {
        UTexture2D* Texture = Appearance == ESendButtonIconAppearance::Send ? IconTextureSend : IconTextureConfirm;
        SendMessageButton->SetIconFromTexture(Texture);
        const FMargin IconPadding = Appearance == ESendButtonIconAppearance::Send ? IconPaddingSend : IconPaddingConfirm;
        SendMessageButton->SetIconPadding(IconPadding);
    }

    if (CancelEditingHeaderPanel)
    {
        const ESlateVisibility PanelVisibility =
            Appearance == ESendButtonIconAppearance::Send ? ESlateVisibility::Collapsed : ESlateVisibility::SelfHitTestInvisible;
        CancelEditingHeaderPanel->SetVisibility(PanelVisibility);
    }
}
