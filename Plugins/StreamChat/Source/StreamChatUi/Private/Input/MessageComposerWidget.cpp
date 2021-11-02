// Copyright Stream.IO, Inc. All Rights Reserved.

#include "Input/MessageComposerWidget.h"

#include "Brushes/SlateImageBrush.h"
#include "Components/ButtonSlot.h"
#include "Context/ChannelContextWidget.h"
#include "Engine/Texture2D.h"
#include "TimerManager.h"

void UMessageComposerWidget::NativeOnInitialized()
{
    MessageInput->OnTextChanged.AddDynamic(this, &UMessageComposerWidget::OnInputTextChanged);
    MessageInput->OnTextCommitted.AddDynamic(this, &UMessageComposerWidget::OnInputTextCommit);
    CancelEditingButton->OnClicked.AddDynamic(this, &UMessageComposerWidget::OnCancelEditingButtonClicked);
    SendMessageButton->OnClicked.AddDynamic(this, &UMessageComposerWidget::OnSendButtonClicked);

    SendMessageButton->WidgetStyle.NormalPadding = {};
    SendMessageButton->WidgetStyle.PressedPadding = {};
    CancelEditingButton->WidgetStyle.NormalPadding = {};
    CancelEditingButton->WidgetStyle.PressedPadding = {};

    UpdateSendButtonAppearance(false);
    UpdateEditMessageAppearance(ESendButtonIconAppearance::Send);

    Super::NativeOnInitialized();
}

void UMessageComposerWidget::NativeConstruct()
{
    MessageInput->SetKeyboardFocus();
    Super::NativeConstruct();
}

void UMessageComposerWidget::EditMessage(const FMessage& Message)
{
    EditedMessage = Message;
    MessageInput->SetText(FText::FromString(Message.Text));
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
        GetWorld()->GetTimerManager().SetTimerForNextTick([&] { MessageInput->SetKeyboardFocus(); });
    }
}

void UMessageComposerWidget::OnCancelEditingButtonClicked()
{
    StopEditMessage();
}

void UMessageComposerWidget::OnSendButtonClicked()
{
    SendMessage();
}

void UMessageComposerWidget::SendMessage()
{
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
        Channel->SendMessage(Text);
        MessageInput->SetText(FText::GetEmpty());
    }
}

void UMessageComposerWidget::UpdateSendButtonAppearance(const bool bEnabled)
{
    UTexture2D* Texture = bEnabled ? ButtonTextureNormal : ButtonTextureDisabled;
    const FSlateImageBrush Brush = FSlateImageBrush(Texture, Texture->GetImportedSize());
    SendMessageButton->WidgetStyle.SetNormal(Brush);
    SendMessageButton->WidgetStyle.SetHovered(Brush);
    SendMessageButton->WidgetStyle.SetPressed(Brush);
}

void UMessageComposerWidget::StopEditMessage()
{
    EditedMessage.Reset();

    UpdateEditMessageAppearance(ESendButtonIconAppearance::Send);
    MessageInput->SetText(FText::GetEmpty());
}

void UMessageComposerWidget::UpdateEditMessageAppearance(const ESendButtonIconAppearance Appearance)
{
    UTexture2D* Texture = Appearance == ESendButtonIconAppearance::Send ? IconTextureSend : IconTextureConfirm;
    SendMessageIcon->SetBrushFromTexture(Texture, true);
    if (UButtonSlot* ButtonSlot = Cast<UButtonSlot>(SendMessageIcon->Slot))
    {
        const FMargin IconPadding =
            Appearance == ESendButtonIconAppearance::Send ? IconPaddingSend : IconPaddingConfirm;
        ButtonSlot->SetPadding(IconPadding);
    }

    const ESlateVisibility PanelVisibility = Appearance == ESendButtonIconAppearance::Send
                                                 ? ESlateVisibility::Collapsed
                                                 : ESlateVisibility::SelfHitTestInvisible;
    CancelEditingHeaderPanel->SetVisibility(PanelVisibility);
}
