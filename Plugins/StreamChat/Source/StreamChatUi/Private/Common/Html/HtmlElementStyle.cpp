// Copyright 2022 Stream.IO, Inc. All Rights Reserved.

#include "Common/Html/HtmlElementStyle.h"

#include "HtmlParser.h"

FHtmlElementStyle::FHtmlElementStyle()
    : bOverride_FontObject(0)
    , bOverride_FontMaterial(0)
    , bOverride_OutlineSettings(0)
    , bOverride_TypefaceFontName(0)
    , bOverride_Size(0)
    , bOverride_LetterSpacing(0)
    , bOverride_ColorAndOpacity(0)
    , bOverride_ShadowOffset(0)
    , bOverride_ShadowColorAndOpacity(0)
    , bOverride_SelectedBackgroundColor(0)
    , bOverride_HighlightColor(0)
    , bOverride_HighlightShape(0)
    , bOverride_StrikeBrush(0)
    , bOverride_UnderlineBrush(0)
{
}

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
    if (Tag == HtmlTag::Anchor)
    {
        return &A;
    }
    if (Tag == HtmlTag::Strong)
    {
        return &Strong;
    }
    if (Tag == HtmlTag::Emphasis)
    {
        return &Em;
    }
    if (Tag == HtmlTag::InlineCode)
    {
        return &Code;
    }
    if (Tag == HtmlTag::DeletedText)
    {
        return &Del;
    }
    if (Tag == HtmlTag::Heading1)
    {
        return &H1;
    }
    if (Tag == HtmlTag::Heading2)
    {
        return &H2;
    }
    if (Tag == HtmlTag::Heading3)
    {
        return &H3;
    }
    if (Tag == HtmlTag::Heading4)
    {
        return &H4;
    }
    if (Tag == HtmlTag::Heading5)
    {
        return &H5;
    }
    if (Tag == HtmlTag::Heading6)
    {
        return &H6;
    }
    if (Tag == HtmlTag::ListItem)
    {
        return &Li;
    }

    UE_LOG(LogTemp, Warning, TEXT("Unhandled HTML tag [Tag: %s]"), *Tag.ToString());
    return nullptr;
}

FTextBlockStyle FHtmlElementStyles::MakeCombinedStyle(const TArray<FName>& Tags) const
{
    FTextBlockStyle Result{Default};
    for (const FName& Tag : Tags)
    {
        if (Tag == HtmlTag::Paragraph || Tag == HtmlTag::LineBreak || Tag == HtmlTag::UnorderedList || Tag == HtmlTag::OrderedList)
        {
            continue;
        }

        if (const FHtmlElementStyle* Style = GetStyle(Tag))
        {
            Combine(Result, *Style);
        }
    }
    return Result;
}
