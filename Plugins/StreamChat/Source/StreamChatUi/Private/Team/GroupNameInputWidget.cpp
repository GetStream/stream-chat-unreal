// Copyright 2022 Stream.IO, Inc. All Rights Reserved.

#include "Team/GroupNameInputWidget.h"

#include "ThemeDataAsset.h"

UGroupNameInputWidget::UGroupNameInputWidget()
{
    bAutoSetup = true;
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

void UGroupNameInputWidget::NativePreConstruct()
{
    Super::NativePreConstruct();

    if (Border)
    {
        Border->SetBrushColor(GetTheme()->GetPaletteColor(GetTheme()->SelectedContactsBackgroundColor));
    }
    if (TextBlock)
    {
        TextBlock->SetColorAndOpacity(GetTheme()->GetPaletteColor(GetTheme()->SelectedContactsLabelTextColor));
    }
    if (Divider)
    {
        Divider->SetColorAndOpacity(GetTheme()->GetPaletteColor(GetTheme()->SelectedContactsDividerColor));
    }
    if (GroupName)
    {
        GroupName->WidgetStyle.SetColorAndOpacity(GetTheme()->GetPaletteColor(GetTheme()->SelectedContactsInputTextColor));
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
