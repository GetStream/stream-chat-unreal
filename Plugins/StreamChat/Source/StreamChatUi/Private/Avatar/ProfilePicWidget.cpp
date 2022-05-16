// Copyright 2022 Stream.IO, Inc. All Rights Reserved.

#include "Avatar/ProfilePicWidget.h"

#include "ImageDownloadSubsystem.h"
#include "User/User.h"
#include "WidgetUtil.h"

void UProfilePicWidget::Setup(const FUserRef& InUser)
{
    User = InUser;

    Super::Setup();
}

void UProfilePicWidget::SetupWithUrl(const FString& InImageUrl)
{
    ImageUrl = InImageUrl;

    Super::Setup();
}

void UProfilePicWidget::OnSetup()
{
    if (InitialsTextBlock)
    {
        if (User.IsValid())
        {
            const FString Initials = User->GetInitials();
            // Always initialize avatar with initials, until real image downloads
            InitialsTextBlock->SetVisibility(ESlateVisibility::SelfHitTestInvisible);
            InitialsTextBlock->SetText(FText::FromString(Initials.IsEmpty() ? TEXT("?") : Initials));
        }
        else
        {
            InitialsTextBlock->SetVisibility(ESlateVisibility::Collapsed);
        }
    }

    if (Image)
    {
        Image->SetColorAndOpacity(WidgetUtil::ChooseColorForString(User.IsValid() ? User->Id : ImageUrl));

        FetchRemoteImage();
    }
}

FString UProfilePicWidget::GetUrl() const
{
    if (User.IsValid())
    {
        // HACK: Make sure # is %23, needed due to limitations in avatars.dicebear.com api
        return User->Image.Replace(TEXT("#"), TEXT("%23"));
    }
    return ImageUrl;
}

void UProfilePicWidget::FetchRemoteImage()
{
    UImageDownloadSubsystem* Subsystem = GEngine->GetEngineSubsystem<UImageDownloadSubsystem>();
    if (!Subsystem)
    {
        return;
    }

    Subsystem->DownloadImage(
        GetUrl(),
        [WeakThis = TWeakObjectPtr<UProfilePicWidget>(this)](UTexture2DDynamic* Texture)
        {
            if (!WeakThis.IsValid() || !WeakThis->Image || !WeakThis->InitialsTextBlock || !Texture)
            {
                return;
            }

            WeakThis->Image->SetBrushFromTextureDynamic(Texture, true);
            WeakThis->Image->SetColorAndOpacity(FLinearColor::White);
            WeakThis->InitialsTextBlock->SetVisibility(ESlateVisibility::Collapsed);
            // Ensure retainer widget re-renders (if present)
            WeakThis->Invalidate(EInvalidateWidgetReason::ChildOrder);
        });
}
