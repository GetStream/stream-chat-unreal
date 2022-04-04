// Copyright 2022 Stream.IO, Inc. All Rights Reserved.

#include "Team/GroupNameInputWidget.h"

#include "ThemeDataAsset.h"

UGroupNameInputWidget::UGroupNameInputWidget()
{
    bAutoSetup = true;
    bWantsTheme = true;
}

FText UGroupNameInputWidget::GetGroupName() const
{
    if (GroupName)
    {
        return GroupName->GetText();
    }
    return FText::GetEmpty();
}

void UGroupNameInputWidget::OnSetup()
{
    if (ClearButton)
    {
        ClearButton->OnClicked.AddDynamic(this, &UGroupNameInputWidget::OnClearClicked);
    }
    if (GroupName)
    {
        GroupName->OnTextChanged.AddDynamic(this, &UGroupNameInputWidget::OnGroupNameChange);
    }
}

void UGroupNameInputWidget::OnTheme()
{
    if (Border)
    {
        Border->SetBrushColor(Theme->GetPaletteColor(Theme->SelectedContactsBackgroundColor));
    }
    if (TextBlock)
    {
        TextBlock->SetColorAndOpacity(Theme->GetPaletteColor(Theme->SelectedContactsLabelTextColor));
    }
    if (Divider)
    {
        Divider->SetColorAndOpacity(Theme->GetPaletteColor(Theme->SelectedContactsDividerColor));
    }
    if (GroupName)
    {
        GroupName->WidgetStyle.SetColorAndOpacity(Theme->GetPaletteColor(Theme->SelectedContactsInputTextColor));
    }
}

void UGroupNameInputWidget::OnClearClicked()
{
    GroupName->SetText(FText::GetEmpty());
    OnGroupNameChanged.Broadcast(FText::GetEmpty());
}

void UGroupNameInputWidget::OnGroupNameChange(const FText& Text)
{
    if (!Text.IsEmpty())
    {
        ClearButton->SetVisibility(ESlateVisibility::Visible);
    }
    else
    {
        ClearButton->SetVisibility(ESlateVisibility::Hidden);
    }

    if (!Text.EqualToCaseIgnored(LastText))
    {
        OnGroupNameChanged.Broadcast(Text);
        LastText = Text;
    }
}
