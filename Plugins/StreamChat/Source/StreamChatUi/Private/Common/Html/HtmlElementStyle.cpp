// Copyright 2021 Stream.IO, Inc. All Rights Reserved.

#include "Common/Html/HtmlElementStyle.h"

void Combine(FTextBlockStyle& TargetTextBlockStyle, const FHtmlElementStyle& OverridingStyle)
{
    if (OverridingStyle.bOverride_FontMaterial)
    {
        TargetTextBlockStyle.Font.FontMaterial = OverridingStyle.FontMaterial;
    }
    if (OverridingStyle.bOverride_FontObject)
    {
        TargetTextBlockStyle.Font.FontObject = OverridingStyle.FontObject;
    }
    if (OverridingStyle.bOverride_LetterSpacing)
    {
        TargetTextBlockStyle.Font.LetterSpacing = OverridingStyle.LetterSpacing;
    }
    if (OverridingStyle.bOverride_OutlineSettings)
    {
        TargetTextBlockStyle.Font.OutlineSettings = OverridingStyle.OutlineSettings;
    }
    if (OverridingStyle.bOverride_Size)
    {
        TargetTextBlockStyle.SetFontSize(OverridingStyle.Size);
    }
    if (OverridingStyle.bOverride_HighlightColor)
    {
        TargetTextBlockStyle.SetHighlightColor(OverridingStyle.HighlightColor);
    }
    if (OverridingStyle.bOverride_HighlightShape)
    {
        TargetTextBlockStyle.SetHighlightShape(OverridingStyle.HighlightShape);
    }
    if (OverridingStyle.bOverride_ShadowOffset)
    {
        TargetTextBlockStyle.SetShadowOffset(OverridingStyle.ShadowOffset);
    }
    if (OverridingStyle.bOverride_StrikeBrush)
    {
        TargetTextBlockStyle.SetStrikeBrush(OverridingStyle.StrikeBrush);
    }
    if (OverridingStyle.bOverride_UnderlineBrush)
    {
        TargetTextBlockStyle.SetUnderlineBrush(OverridingStyle.UnderlineBrush);
    }
    if (OverridingStyle.bOverride_ColorAndOpacity)
    {
        TargetTextBlockStyle.SetColorAndOpacity(OverridingStyle.ColorAndOpacity);
    }
    if (OverridingStyle.bOverride_SelectedBackgroundColor)
    {
        TargetTextBlockStyle.SetSelectedBackgroundColor(OverridingStyle.SelectedBackgroundColor);
    }
    if (OverridingStyle.bOverride_TypefaceFontName)
    {
        TargetTextBlockStyle.SetTypefaceFontName(OverridingStyle.TypefaceFontName);
    }
    if (OverridingStyle.bOverride_ShadowColorAndOpacity)
    {
        TargetTextBlockStyle.SetShadowColorAndOpacity(OverridingStyle.ShadowColorAndOpacity);
    }
}

const FHtmlElementStyle* FHtmlElementStyles::GetStyle(const FName& Tag) const
{
    if (Tag == TEXT("A"))
    {
        return &A;
    }
    if (Tag == TEXT("strong"))
    {
        return &Strong;
    }
    if (Tag == TEXT("em"))
    {
        return &Em;
    }
    if (Tag == TEXT("Code"))
    {
        return &Code;
    }
    if (Tag == TEXT("del"))
    {
        return &Del;
    }
    if (Tag == TEXT("H1"))
    {
        return &H1;
    }
    if (Tag == TEXT("H2"))
    {
        return &H2;
    }
    if (Tag == TEXT("H3"))
    {
        return &H3;
    }
    if (Tag == TEXT("H4"))
    {
        return &H4;
    }
    if (Tag == TEXT("H5"))
    {
        return &H5;
    }
    if (Tag == TEXT("H6"))
    {
        return &H6;
    }
    UE_LOG(LogTemp, Warning, TEXT("Unhandled HTML tag [Tag: %s]"), *Tag.ToString());
    return nullptr;
}

FTextBlockStyle FHtmlElementStyles::MakeCombinedStyle(const TArray<FName>& Tags) const
{
    FTextBlockStyle Result{Default};
    for (const FName& Tag : Tags)
    {
        if (const FHtmlElementStyle* Style = GetStyle(Tag))
        {
            Combine(Result, *Style);
        }
    }
    return Result;
}
