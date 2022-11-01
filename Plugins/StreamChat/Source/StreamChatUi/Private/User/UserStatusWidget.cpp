#include "User/UserStatusWidget.h"

#include "ThemeDataAsset.h"
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
        Header->SetTitle(FText::FromString(User->GetNameOrId()));
    }
    if (SubtitleTextBlock)
    {
        SubtitleTextBlock->SetText(User->GetLastSeenText());
    }
}

void UUserStatusWidget::NativePreConstruct()
{
    Super::NativePreConstruct();
    if (SubtitleTextBlock)
    {
        SubtitleTextBlock->SetColorAndOpacity(GetTheme()->GetPaletteColor(GetTheme()->StatusSubtitleTextColor));
    }
}

void UUserStatusWidget::NativeConstruct()
{
    Super::NativeConstruct();
    if (Header)
    {
        Header->OnHeaderButtonClicked.AddDynamic(this, &UUserStatusWidget::ButtonClicked);
    }
}

void UUserStatusWidget::NativeDestruct()
{
    if (Header)
    {
        Header->OnHeaderButtonClicked.RemoveDynamic(this, &UUserStatusWidget::ButtonClicked);
    }
    Super::NativeDestruct();
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
