// Copyright Stream.IO, Inc. All Rights Reserved.

#include "AvatarWidget.h"

#include "Engine/Texture2DDynamic.h"
#include "WidgetUtil.h"

namespace
{
// https://www.figma.com/file/oeHiSMDoutuVbJ9WiVyhcG/Chat-Design-Kit-1.5-Android---Work-in-Progress?node-id=17021%3A102778
FLinearColor Colors[16] = {
    FColor::FromHex(TEXT("#B64E4E")),
    FColor::FromHex(TEXT("#B4774B")),
    FColor::FromHex(TEXT("#B4A34B")),
    FColor::FromHex(TEXT("#9AB44B")),
    FColor::FromHex(TEXT("#6EB44B")),
    FColor::FromHex(TEXT("#4BB453")),
    FColor::FromHex(TEXT("#4BB47F")),
    FColor::FromHex(TEXT("#4BB4AC")),
    FColor::FromHex(TEXT("#4B91B4")),
    FColor::FromHex(TEXT("#4B65B4")),
    FColor::FromHex(TEXT("#5C4BB4")),
    FColor::FromHex(TEXT("#884BB4")),
    FColor::FromHex(TEXT("#B44BB4")),
    FColor::FromHex(TEXT("#B44B88")),
    FColor::FromHex(TEXT("#926D73")),
    FColor::FromHex(TEXT("#6E8B91")),
};

}    // namespace

void UAvatarWidget::Setup(const FUser& InUser)
{
    User = InUser;

    Super::Setup();
}

void UAvatarWidget::OnSetup()
{
    const FString Initials = User.GetInitials();
    if (InitialsTextBlock)
    {
        // Always initialize avatar with initials, until real image downloads
        InitialsTextBlock->SetText(FText::FromString(Initials));
    }

    if (Image)
    {
        Image->SetBrushFromMaterial(Material);
        Image->SetColorAndOpacity(ChooseColorForString(Initials));

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

FLinearColor UAvatarWidget::ChooseColorForString(const FString& String)
{
    return Colors[WidgetUtil::HashStringWithMax(String, 16)];
}
