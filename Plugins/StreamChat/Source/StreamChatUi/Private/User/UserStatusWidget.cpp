#include "User/UserStatusWidget.h"

#include "ThemeDataAsset.h"
#include "User/User.h"

UUserStatusWidget::UUserStatusWidget()
{
    bWantsTheme = true;
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
        Header->SetTitle(FText::FromString(User->GetNameOrId()));
        Header->OnHeaderButtonClicked.AddDynamic(this, &UUserStatusWidget::ButtonClicked);
    }
    if (SubtitleTextBlock)
    {
        SubtitleTextBlock->SetText(User->GetLastSeenText());
    }
}

void UUserStatusWidget::OnTheme()
{
    if (SubtitleTextBlock)
    {
        SubtitleTextBlock->SetColorAndOpacity(Theme->GetPaletteColor(Theme->StatusSubtitleTextColor));
    }
}

void UUserStatusWidget::ButtonClicked()
{
    bSelected = !bSelected;
    OnUserStatusClicked.Broadcast(User, bSelected);
    if (Selected)
    {
        Selected->SetVisibility(bSelected ? ESlateVisibility::SelfHitTestInvisible : ESlateVisibility::Collapsed);
    }
}
