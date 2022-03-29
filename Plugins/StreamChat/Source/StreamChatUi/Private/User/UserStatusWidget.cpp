#include "User/UserStatusWidget.h"

#include "User/User.h"

UUserStatusWidget::UUserStatusWidget()
{
}

void UUserStatusWidget::Setup(const FUserRef& InUser)
{
    User = InUser;

    Super::Setup();
}

void UUserStatusWidget::OnSetup()
{
    if (Avatar)
    {
        Avatar->Setup({User});
    }
    if (Header)
    {
        Header->SetTitle(FText::FromString(User->Name.IsEmpty() ? User->Id : User->Name));
        Header->OnHeaderButtonClicked.AddDynamic(this, &UUserStatusWidget::ButtonClicked);
    }
    if (SubtitleTextBlock)
    {
        SubtitleTextBlock->SetText(FText::FromString(TEXT("Last online at some point")));
    }
}

void UUserStatusWidget::ButtonClicked()
{
    OnUserStatusClicked.Broadcast(User);
}
