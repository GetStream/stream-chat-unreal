// Copyright 2022 Stream.IO, Inc. All Rights Reserved.

#include "Input/MessageInputWidget.h"

#include "ThemeDataAsset.h"

UMessageInputWidget::UMessageInputWidget()
{
    bIsFocusable = true;
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

void UMessageInputWidget::OnInputTextChanged(const FText& Text)
{
    OnTextChanged.Broadcast(Text);
}

void UMessageInputWidget::OnInputTextCommit(const FText& Text, ETextCommit::Type CommitMethod)
{
    OnTextCommitted.Broadcast(Text, CommitMethod);
}

void UMessageInputWidget::NativePreConstruct()
{
    Super::NativePreConstruct();
    if (TextBox)
    {
        TextBox->WidgetStyle.SetForegroundColor(GetTheme()->GetPaletteColor(GetTheme()->MessageInputTextColor));
        TextBox->WidgetStyle.SetBackgroundColor(GetTheme()->GetPaletteColor(GetTheme()->MessageInputBackgroundColor));
    }
    if (Border)
    {
        Border->SetBrushColor(GetTheme()->GetPaletteColor(GetTheme()->MessageInputBorderColor));
    }
}

void UMessageInputWidget::NativeConstruct()
{
    Super::NativeConstruct();
    if (TextBox)
    {
        TextBox->OnTextChanged.AddDynamic(this, &UMessageInputWidget::OnInputTextChanged);
        TextBox->OnTextCommitted.AddDynamic(this, &UMessageInputWidget::OnInputTextCommit);
    }
}

void UMessageInputWidget::NativeDestruct()
{
    if (TextBox)
    {
        TextBox->OnTextChanged.RemoveDynamic(this, &UMessageInputWidget::OnInputTextChanged);
        TextBox->OnTextCommitted.RemoveDynamic(this, &UMessageInputWidget::OnInputTextCommit);
    }
    Super::NativeDestruct();
}

FReply UMessageInputWidget::NativeOnFocusReceived(const FGeometry& InGeometry, const FFocusEvent& InFocusEvent)
{
    TextBox->SetKeyboardFocus();
    return Super::NativeOnFocusReceived(InGeometry, InFocusEvent);
}
