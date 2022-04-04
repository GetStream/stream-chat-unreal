// Copyright 2022 Stream.IO, Inc. All Rights Reserved.

#include "Contact/SelectedContactsWidget.h"

#include "ThemeDataAsset.h"
#include "TimerManager.h"

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
    SetTypingMode(false);
}

void USelectedContactsWidget::RemoveUser(const FUserRef& User)
{
    Contacts.Remove(User);
    PopulateWrapBox();
    SetTypingMode(false);
}

const TArray<FUserRef>& USelectedContactsWidget::GetUsers() const
{
    return Contacts;
}

void USelectedContactsWidget::SetGroupMode(const bool bIsGroupMode)
{
    if (bIsGroupMode)
    {
        TextBlock->SetText(FText::FromString(TEXT("MEMBERS")));
    }
    else
    {
        TextBlock->SetText(FText::FromString(TEXT("TO")));
    }
}

void USelectedContactsWidget::OnSetup()
{
    SetUsers(Contacts);

    if (AddUserButton)
    {
        AddUserButton->OnClicked.AddDynamic(this, &USelectedContactsWidget::OnAddUserClicked);
    }
    if (SearchText)
    {
        SearchText->OnTextCommitted.AddDynamic(this, &USelectedContactsWidget::OnSearchTextCommit);
    }

    GetWorld()->GetTimerManager().SetTimerForNextTick([this] { SetTypingMode(true); });
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
    if (SearchText)
    {
        SearchText->WidgetStyle.SetColorAndOpacity(Theme->GetPaletteColor(Theme->SelectedContactsInputTextColor));
    }
}

void USelectedContactsWidget::PopulateWrapBox()
{
    if (!WrapBox)
    {
        return;
    }

    if (Contacts.Num() == 0)
    {
        WrapBox->SetVisibility(ESlateVisibility::Collapsed);
        return;
    }

    WrapBox->SetVisibility(ESlateVisibility::SelfHitTestInvisible);

    WrapBox->ClearChildren();
    for (const FUserRef& User : Contacts)
    {
        USelectedContactWidget* Widget = CreateWidget<USelectedContactWidget>(this, SelectedContactWidgetClass);
        Widget->Setup(User);
        WrapBox->AddChildToWrapBox(Widget);
    }
}

void USelectedContactsWidget::SetTypingMode(const bool bNewTypingMode)
{
    if (bNewTypingMode == bTypingMode)
    {
        return;
    }
    bTypingMode = bNewTypingMode;

    if (!SearchText || !AddUserButton)
    {
        return;
    }

    if (bTypingMode)
    {
        SearchText->SetVisibility(ESlateVisibility::Visible);
        SearchText->SetKeyboardFocus();
        AddUserButton->SetEnabled(false);
    }
    else
    {
        SearchText->SetVisibility(ESlateVisibility::Collapsed);
        SearchText->SetText(FText::GetEmpty());
        OnSearchTextChanged.Broadcast(FText::GetEmpty());
        LastSearchText = FText::GetEmpty();
        AddUserButton->SetEnabled(true);
    }
}

void USelectedContactsWidget::OnAddUserClicked()
{
    if (bTypingMode)
    {
        return;
    }

    SetTypingMode(true);
}

void USelectedContactsWidget::OnSearchTextCommit(const FText& Text, const ETextCommit::Type Commit)
{
    if (!Text.EqualToCaseIgnored(LastSearchText))
    {
        OnSearchTextChanged.Broadcast(Text);
        LastSearchText = Text;
    }
}
