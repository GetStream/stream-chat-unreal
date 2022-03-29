// Copyright 2022 Stream.IO, Inc. All Rights Reserved.

#include "Contact/SelectedContactsWidget.h"

#include "ThemeDataAsset.h"

USelectedContactsWidget::USelectedContactsWidget()
{
    bAutoSetup = true;
    bWantsTheme = true;
}

void USelectedContactsWidget::SetUsers(const TArray<FUserRef>& Users)
{
    Contacts = Users;
    PopulateWrapBox();
}

void USelectedContactsWidget::AddUser(const FUserRef& User)
{
    Contacts.Add(User);
    PopulateWrapBox();
}

void USelectedContactsWidget::RemoveUser(const FUserRef& User)
{
    Contacts.Remove(User);
    PopulateWrapBox();
}

void USelectedContactsWidget::OnSetup()
{
    SetUsers(Contacts);
}

void USelectedContactsWidget::OnTheme()
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
}

void USelectedContactsWidget::PopulateWrapBox()
{
    WrapBox->ClearChildren();
    for (const FUserRef& User : Contacts)
    {
        USelectedContactWidget* Widget = CreateWidget<USelectedContactWidget>(this, SelectedContactWidgetClass);
        Widget->Setup(User);
        WrapBox->AddChildToWrapBox(Widget);
    }
}
