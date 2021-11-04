// Copyright Stream.IO, Inc. All Rights Reserved.

#include "AvatarWidget.h"

#include "Engine/Texture2DDynamic.h"
#include "WidgetUtil.h"

void UAvatarWidget::Setup(const FUser& InUser)
{
    User = InUser;

    Super::Setup();
}

void UAvatarWidget::OnSetup()
{
    if (InitialsTextBlock)
    {
        // Always initialize avatar with initials, until real image downloads
        InitialsTextBlock->SetText(FText::FromString(User.GetInitials()));
    }

    if (Image)
    {
        Image->SetBrushFromMaterial(Material);
        Image->SetColorAndOpacity(FColor::Red);

        if (!User.Image.IsEmpty())
        {
            WidgetUtil::DownloadImage(
                User.Image,
                [this](UTexture2DDynamic* Texture)
                {
                    if (UMaterialInstanceDynamic* DynamicMaterialInstance = Image->GetDynamicMaterial())
                    {
                        DynamicMaterialInstance->SetTextureParameterValue(MaterialTextureParameterName, Texture);
                    }
                    Image->SetColorAndOpacity(FLinearColor::White);
                    if (InitialsTextBlock)
                    {
                        InitialsTextBlock->SetVisibility(ESlateVisibility::Collapsed);
                    }
                });
        }
    }
}
