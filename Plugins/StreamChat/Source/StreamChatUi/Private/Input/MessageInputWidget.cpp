// Copyright 2022 Stream.IO, Inc. All Rights Reserved.

#include "Input/MessageInputWidget.h"

#include "ThemeDataAsset.h"

UMessageInputWidget::UMessageInputWidget()
{
    bIsFocusable = true;
    bWantsTheme = true;
}

void UMessageInputWidget::SynchronizeProperties()
{
    Super::SynchronizeProperties();

    TextBox->SetHintText(HintText);
}

FText UMessageInputWidget::GetText() const
{
    return TextBox->GetText();
}

void UMessageInputWidget::SetText(const FText InText)
{
    TextBox->SetText(InText);
}

void UMessageInputWidget::NativeOnInitialized()
{
    Super::NativeOnInitialized();

    TextBox->OnTextChanged.AddUniqueDynamic(this, &UMessageInputWidget::OnInputTextChanged);
    TextBox->OnTextCommitted.AddUniqueDynamic(this, &UMessageInputWidget::OnInputTextCommit);
}

void UMessageInputWidget::OnInputTextChanged(const FText& Text)
{
    OnTextChanged.Broadcast(Text);
}

void UMessageInputWidget::OnInputTextCommit(const FText& Text, ETextCommit::Type CommitMethod)
{
    OnTextCommitted.Broadcast(Text, CommitMethod);
}

void UMessageInputWidget::OnTheme()
{
    if (TextBox)
    {
        TextBox->WidgetStyle.SetForegroundColor(Theme->GetPaletteColor(Theme->MessageInputTextColor));
        TextBox->WidgetStyle.SetBackgroundColor(Theme->GetPaletteColor(Theme->MessageInputBackgroundColor));
    }
    if (Border)
    {
        Border->SetBrushColor(Theme->GetPaletteColor(Theme->MessageInputBorderColor));
    }
}

FReply UMessageInputWidget::NativeOnFocusReceived(const FGeometry& InGeometry, const FFocusEvent& InFocusEvent)
{
    TextBox->SetKeyboardFocus();
    return Super::NativeOnFocusReceived(InGeometry, InFocusEvent);
}
