// Copyright 2022 Stream.IO, Inc. All Rights Reserved.

#include "Header/TypingIndicatorWidget.h"

#include "UiBlueprintLibrary.h"
#include "User/User.h"

void UTypingIndicatorWidget::AddUser(const FUserRef& User)
{
    // Add uniquely
    Users.RemoveAll([&User](const FUserRef& U) { return U == User; });
    Users.Add(User);
    UpdateLabel();
}

bool UTypingIndicatorWidget::RemoveUser(const FUserRef& User)
{
    Users.RemoveAll([&User](const FUserRef& U) { return U == User; });
    UpdateLabel();
    return Users.Num() == 0;
}

void UTypingIndicatorWidget::UpdateLabel() const
{
    if (TextBlock)
    {
        TextBlock->SetText(GetLabel());
    }
}

FText UTypingIndicatorWidget::GetLabel() const
{
    if (Users.Num() == 0)
    {
        return FText::GetEmpty();
    }

    if (Users.Num() == 1)
    {
        FFormatNamedArguments Args;
        Args.Add(TEXT("User"), FText::FromString(Users[0]->Name));

        return FText::Format(FText::FromString(TEXT("{User} is typing")), Args);
    }

    FFormatNamedArguments Args;
    Args.Add(TEXT("User"), FText::FromString(Users.Last()->Name));
    Args.Add(TEXT("Extra"), Users.Num() - 1);

    return FText::Format(FText::FromString(TEXT("{User} and {Extra} more are typing")), Args);
}
