#include "Header/TypingIndicatorWidget.h"

#include "UiBlueprintLibrary.h"

void UTypingIndicatorWidget::AddUser(const FUser& User)
{
    // Add uniquely
    Users.RemoveAll([&User](const FUser& U) { return U.Id == User.Id; });
    Users.Add(User);
    UpdateLabel();
}

bool UTypingIndicatorWidget::RemoveUser(const FUser& User)
{
    Users.RemoveAll([&User](const FUser& U) { return U.Id == User.Id; });
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
        Args.Add(TEXT("User"), FText::FromString(Users[0].Name));

        return FText::Format(FText::FromString(TEXT("{User} is typing")), Args);
    }

    FFormatNamedArguments Args;
    Args.Add(TEXT("User"), FText::FromString(Users.Last().Name));
    Args.Add(TEXT("Extra"), Users.Num() - 1);

    return FText::Format(FText::FromString(TEXT("{User} and {Extra} more are typing")), Args);
}
