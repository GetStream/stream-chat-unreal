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

void UProfilePicWidget::OnSetup()
{
    const FString Initials = User->GetInitials();
    if (InitialsTextBlock)
    {
        // Always initialize avatar with initials, until real image downloads
        InitialsTextBlock->SetText(FText::FromString(Initials.IsEmpty() ? TEXT("?") : Initials));
    }

    if (Image)
    {
        Image->SetColorAndOpacity(WidgetUtil::ChooseColorForString(User->Id));

        if (!User->Image.IsEmpty())
        {
            FetchRemoteImage();
        }
    }
}

void UProfilePicWidget::FetchRemoteImage()
{
    UImageDownloadSubsystem* Subsystem = UGameInstance::GetSubsystem<UImageDownloadSubsystem>(GetGameInstance());
    if (!Subsystem)
    {
        return;
    }

    // HACK: Make sure # is %23, needed due to limitations in avatars.dicebear.com api
    const FString Url = User->Image.Replace(TEXT("#"), TEXT("%23"));
    Subsystem->DownloadImage(
        Url,
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
