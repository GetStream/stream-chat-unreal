// Copyright 2022 Stream.IO, Inc. All Rights Reserved.

#include "Team/GroupNameInputWidget.h"

#include "ThemeDataAsset.h"

FText UGroupNameInputWidget::GetGroupName() const
{
    if (GroupName)
    {
        return GroupName->GetText();
    }
    return FText::GetEmpty();
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

void UGroupNameInputWidget::NativeConstruct()
{
    Super::NativeConstruct();
    if (ClearButton)
    {
        ClearButton->OnClicked.AddDynamic(this, &UGroupNameInputWidget::OnClearClicked);
    }
    if (GroupName)
    {
        GroupName->OnTextChanged.AddDynamic(this, &UGroupNameInputWidget::OnGroupNameChange);
    }
}

void UGroupNameInputWidget::NativeDestruct()
{
    if (ClearButton)
    {
        ClearButton->OnClicked.RemoveDynamic(this, &UGroupNameInputWidget::OnClearClicked);
    }
    if (GroupName)
    {
        GroupName->OnTextChanged.RemoveDynamic(this, &UGroupNameInputWidget::OnGroupNameChange);
    }
    Super::NativeDestruct();
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
