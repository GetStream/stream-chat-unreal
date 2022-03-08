// Copyright 2022 Stream.IO, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Styling/SlateStyle.h"
#include "Styling/SlateTypes.h"

#include "HtmlElementStyle.generated.h"

USTRUCT(BlueprintType)
struct FHtmlElementStyle
{
    GENERATED_BODY()

    /** The font object (valid when used from UMG or a Slate widget style asset) */
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = SlateStyleRules, meta = (AllowedClasses = "Font", DisplayName = "Font Family", editcondition = "bOverride_FontObject"))
    const UObject* FontObject = nullptr;

    /** The name of the font to use from the default typeface (None will use the first entry) */
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = SlateStyleRules, meta = (DisplayName = "Typeface", editcondition = "bOverride_TypefaceFontName"))
    FName TypefaceFontName;

    /**
     * The font size is a measure in point values.  The conversion of points to Slate Units is done at 96 dpi.  So if
     * you're using a tool like Photoshop to prototype layouts and UI mock ups, be sure to change the default dpi
     * measurements from 72 dpi to 96 dpi.
     */
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = SlateStyleRules, meta = (ClampMin = 1, ClampMax = 1000, editcondition = "bOverride_Size"))
    int32 Size = 24;

    /** The uniform spacing (or tracking) between all characters in the text. */
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = SlateStyleRules, meta = (ClampMin = -1000, ClampMax = 10000, editcondition = "bOverride_LetterSpacing"))
    int32 LetterSpacing = 0;

    /** The color and opacity of this text */
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Appearance, meta = (DisplayName = "Color", editcondition = "bOverride_ColorAndOpacity"))
    FSlateColor ColorAndOpacity = FLinearColor::White;

    /** The brush used to draw an strike through the text (if any) */
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Appearance, meta = (editcondition = "bOverride_StrikeBrush"))
    FSlateBrush StrikeBrush;

    /** The brush used to draw an underline under the text (if any) */
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Appearance, meta = (editcondition = "bOverride_UnderlineBrush"))
    FSlateBrush UnderlineBrush;

    /** The material to use when rendering this font */
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = SlateStyleRules, meta = (AllowedClasses = "MaterialInterface", editcondition = "bOverride_FontMaterial"))
    UObject* FontMaterial = nullptr;

    /** Settings for applying an outline to a font */
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = SlateStyleRules, meta = (editcondition = "bOverride_OutlineSettings"))
    FFontOutlineSettings OutlineSettings;

    /** How much should the shadow be offset? An offset of 0 implies no shadow. */
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Appearance, meta = (editcondition = "bOverride_ShadowOffset"))
    FVector2D ShadowOffset;

    /** The color and opacity of the shadow */
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Appearance, meta = (editcondition = "bOverride_ShadowColorAndOpacity"))
    FLinearColor ShadowColorAndOpacity;

    /** The background color of selected text */
    UPROPERTY(EditAnywhere, Category = Appearance, meta = (editcondition = "bOverride_SelectedBackgroundColor"))
    FSlateColor SelectedBackgroundColor;

    /** The color of highlighted text */
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Appearance, meta = (editcondition = "bOverride_HighlightColor"))
    FLinearColor HighlightColor;

    /** The shape of highlighted text */
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Appearance, meta = (editcondition = "bOverride_HighlightShape"))
    FSlateBrush HighlightShape;

    UPROPERTY(EditAnywhere, Category = Appearance, meta = (InlineEditConditionToggle))
    uint8 bOverride_FontObject : 1;

    UPROPERTY(EditAnywhere, Category = Appearance, meta = (InlineEditConditionToggle))
    uint8 bOverride_FontMaterial : 1;

    UPROPERTY(EditAnywhere, Category = Appearance, meta = (InlineEditConditionToggle))
    uint8 bOverride_OutlineSettings : 1;

    UPROPERTY(EditAnywhere, Category = Appearance, meta = (InlineEditConditionToggle))
    uint8 bOverride_TypefaceFontName : 1;

    UPROPERTY(EditAnywhere, Category = Appearance, meta = (InlineEditConditionToggle))
    uint8 bOverride_Size : 1;

    UPROPERTY(EditAnywhere, Category = Appearance, meta = (InlineEditConditionToggle))
    uint8 bOverride_LetterSpacing : 1;

    UPROPERTY(EditAnywhere, Category = Appearance, meta = (InlineEditConditionToggle))
    uint8 bOverride_ColorAndOpacity : 1;

    UPROPERTY(EditAnywhere, Category = Appearance, meta = (InlineEditConditionToggle))
    uint8 bOverride_ShadowOffset : 1;

    UPROPERTY(EditAnywhere, Category = Appearance, meta = (InlineEditConditionToggle))
    uint8 bOverride_ShadowColorAndOpacity : 1;

    UPROPERTY(EditAnywhere, Category = Appearance, meta = (InlineEditConditionToggle))
    uint8 bOverride_SelectedBackgroundColor : 1;

    UPROPERTY(EditAnywhere, Category = Appearance, meta = (InlineEditConditionToggle))
    uint8 bOverride_HighlightColor : 1;

    UPROPERTY(EditAnywhere, Category = Appearance, meta = (InlineEditConditionToggle))
    uint8 bOverride_HighlightShape : 1;

    UPROPERTY(EditAnywhere, Category = Appearance, meta = (InlineEditConditionToggle))
    uint8 bOverride_StrikeBrush : 1;

    UPROPERTY(EditAnywhere, Category = Appearance, meta = (InlineEditConditionToggle))
    uint8 bOverride_UnderlineBrush : 1;
};

void Combine(FTextBlockStyle& TargetTextBlockStyle, const FHtmlElementStyle& OverridingStyle);

USTRUCT(BlueprintType)
struct FHtmlElementStyles
{
    GENERATED_BODY()

    FTextBlockStyle MakeCombinedStyle(const TArray<FName>& Tags) const;
    const FHtmlElementStyle* GetStyle(const FName& Tag) const;

    /// The default text style
    UPROPERTY(EditAnywhere, Category = Styles)
    FTextBlockStyle Default;

    /// Style for text surrounded with the <strong> tag
    UPROPERTY(EditAnywhere, Category = Styles)
    FHtmlElementStyle Strong;

    /// Style for text surrounded with the <em> tag
    UPROPERTY(EditAnywhere, Category = Styles)
    FHtmlElementStyle Em;

    /// Style for text surrounded with the <del> tag
    UPROPERTY(EditAnywhere, Category = Styles)
    FHtmlElementStyle Del;

    /// Style for text surrounded with the <a> tag
    UPROPERTY(EditAnywhere, Category = Styles)
    FHtmlElementStyle A;

    /// Style for text surrounded with the <h1> tag
    UPROPERTY(EditAnywhere, Category = Styles)
    FHtmlElementStyle H1;

    /// Style for text surrounded with the <h2> tag
    UPROPERTY(EditAnywhere, Category = Styles)
    FHtmlElementStyle H2;

    /// Style for text surrounded with the <h3> tag
    UPROPERTY(EditAnywhere, Category = Styles)
    FHtmlElementStyle H3;

    /// Style for text surrounded with the <h4> tag
    UPROPERTY(EditAnywhere, Category = Styles)
    FHtmlElementStyle H4;

    /// Style for text surrounded with the <h5> tag
    UPROPERTY(EditAnywhere, Category = Styles)
    FHtmlElementStyle H5;

    /// Style for text surrounded with the <h6> tag
    UPROPERTY(EditAnywhere, Category = Styles)
    FHtmlElementStyle H6;

    /// Style for text surrounded with the <code> tag
    UPROPERTY(EditAnywhere, Category = Styles)
    FHtmlElementStyle Code;

    /// Style for text surrounded with the <li> tag
    UPROPERTY(EditAnywhere, Category = Styles)
    FHtmlElementStyle Li;

    /// Brush for bullet in unordered lists
    UPROPERTY(EditAnywhere, Category = Styles)
    FInlineTextImageStyle HorizontalRule;
};
