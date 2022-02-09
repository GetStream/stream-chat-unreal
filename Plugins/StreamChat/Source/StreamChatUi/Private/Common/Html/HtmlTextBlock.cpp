// Copyright 2021 Stream.IO, Inc. All Rights Reserved.

#include "Common/Html/HtmlTextBlock.h"

#include "Algo/Transform.h"
#include "Common/Html/HtmlParser.h"
#include "Widgets/Text/SRichTextBlock.h"

namespace
{
constexpr TCHAR DefaultStyleName[] = TEXT("Default");

template <class ObjectType>
struct TFDeferredDeleter final : FDeferredCleanupInterface
{
    explicit TFDeferredDeleter(ObjectType* InInnerObjectToDelete) : InnerObjectToDelete(InInnerObjectToDelete)
    {
    }

    virtual ~TFDeferredDeleter() override
    {
        delete InnerObjectToDelete;
    }

private:
    ObjectType* InnerObjectToDelete;
};

template <class ObjectType>
FORCEINLINE SharedPointerInternals::FRawPtrProxy<ObjectType> MakeShareableDeferredCleanup(ObjectType* InObject)
{
    return MakeShareable(InObject, [](ObjectType* ObjectToDelete) { BeginCleanup(new TFDeferredDeleter<ObjectType>(ObjectToDelete)); });
}
}    // namespace

TSharedRef<FHtmlRichTextMarkupParser> FHtmlRichTextMarkupParser::GetStaticInstance()
{
    static TSharedRef<FHtmlRichTextMarkupParser> Parser = MakeShareable(new FHtmlRichTextMarkupParser());
    return Parser;
}

void FHtmlRichTextMarkupParser::Process(TArray<FTextLineParseResults>& Results, const FString& Input, FString& Output)
{
    if (Input.IsEmpty())
    {
        return;
    }

    FHtmlParser Parser(
        Input,
        [&](const FStringView& Content, const FHtmlParser& Self)
        {
            if (!Results.IsValidIndex(Self.Line))
            {
                Results.Add(FTextLineParseResults{{Self.ParagraphStartIndex, INDEX_NONE}});
            }
            // Keep overwriting end index with each iteration, eventually leaving the correct result
            Results[Self.Line].Range.EndIndex = Input.Len();

            TSet<FStringView> ElementNames;
            Algo::Transform(Self.ElementStack, ElementNames, [](auto&& Element) { return Element.Name; });
            const FString Name = MakeRunName(ElementNames);

            // ContentRange indexes into the Output string
            const FTextRange ContentRange{Output.Len(), Output.Len() + Content.Len()};
            const FTextRunParseResults Run{Name, ContentRange, ContentRange};

            Results[Self.Line].Runs.Add(Run);

            Output.Append(Content);
        });
    Parser.Parse();
}

FHtmlRichTextMarkupParser::FHtmlRichTextMarkupParser()
{
}

FString FHtmlRichTextMarkupParser::MakeRunName(TSet<FStringView>& ElementNames)
{
    ElementNames.Remove(FHtmlParser::ParagraphTag);
    ElementNames.Remove(FHtmlParser::LineBreakTag);

    return FString::Join(ElementNames, TEXT("_"));
}

const FSlateWidgetStyle* FComboSlateStyleSet::GetWidgetStyleInternal(const FName DesiredTypeName, const FName StyleName) const
{
    ensure(WidgetStyleValues.Num() != 0);
    const TSharedRef<FSlateWidgetStyle>* StylePtr = WidgetStyleValues.Find(StyleName);

    if (!StylePtr)
    {
        StylePtr = const_cast<FComboSlateStyleSet*>(this)->GetComboStyle(StyleName);    // sorry :(
        if (!StylePtr)
        {
            Log(Warning,
                FText::Format(
                    FText::FromString(TEXT("Unable to find Slate Widget Style '{0}'. Using {1} defaults instead.")),
                    FText::FromName(StyleName),
                    FText::FromName(DesiredTypeName)));
            return nullptr;
        }
    }

    const TSharedRef<FSlateWidgetStyle> Style = *StylePtr;

    if (Style->GetTypeName() != DesiredTypeName)
    {
        Log(Error,
            FText::Format(
                FText::FromString(TEXT("The Slate Widget Style '{0}' is not of the desired type. Desired: '{1}', Actual: '{2}'")),
                FText::FromName(StyleName),
                FText::FromName(DesiredTypeName),
                FText::FromName(Style->GetTypeName())));
        return nullptr;
    }

    return &Style.Get();
}

template <class T, class U>
void OverwriteNonDefault(const U& Default, U& Target, const U& New, T U::*MemPtr)
{
    if (New.*MemPtr != Default.*MemPtr)
    {
        Target.*MemPtr = New.*MemPtr;
    }
}

const TSharedRef<FSlateWidgetStyle>* FComboSlateStyleSet::GetComboStyle(const FName& StyleName)
{
    const TSharedRef<FSlateWidgetStyle>* DefaultPtr = WidgetStyleValues.Find(DefaultStyleName);
    if (!DefaultPtr)
    {
        return nullptr;
    }
    const TSharedRef<FTextBlockStyle> Default = StaticCastSharedRef<FTextBlockStyle>(*DefaultPtr);

    TArray<FString> Tags;
    StyleName.ToString().ParseIntoArray(Tags, TEXT("_"));

    const TSharedRef<FTextBlockStyle> ComboStyle = MakeShared<FTextBlockStyle>(*Default);
    for (const FString& Tag : Tags)
    {
        const FName TagName{Tag};
        if (TagName == DefaultStyleName)
        {
            continue;
        }

        const TSharedRef<FSlateWidgetStyle>* StylePtr = WidgetStyleValues.Find(TagName);
        if (!StylePtr)
        {
            continue;
        }

        const TSharedRef<FTextBlockStyle> Style = StaticCastSharedRef<FTextBlockStyle>(*StylePtr);
        OverwriteNonDefault(Default->Font, ComboStyle->Font, Style->Font, &FSlateFontInfo::FontObject);
        OverwriteNonDefault(Default->Font, ComboStyle->Font, Style->Font, &FSlateFontInfo::FontMaterial);
        OverwriteNonDefault(Default->Font, ComboStyle->Font, Style->Font, &FSlateFontInfo::CompositeFont);
        OverwriteNonDefault(Default->Font, ComboStyle->Font, Style->Font, &FSlateFontInfo::TypefaceFontName);
        OverwriteNonDefault(Default->Font, ComboStyle->Font, Style->Font, &FSlateFontInfo::Size);
        OverwriteNonDefault(Default->Font, ComboStyle->Font, Style->Font, &FSlateFontInfo::LetterSpacing);
        // OverwriteNonDefault(Default->Font, ComboStyle->Font, Style->Font, &FSlateFontInfo::OutlineSettings);

        OverwriteNonDefault(*Default, *ComboStyle, *Style, &FTextBlockStyle::ColorAndOpacity);
        OverwriteNonDefault(*Default, *ComboStyle, *Style, &FTextBlockStyle::ShadowOffset);
        OverwriteNonDefault(*Default, *ComboStyle, *Style, &FTextBlockStyle::ShadowColorAndOpacity);
        OverwriteNonDefault(*Default, *ComboStyle, *Style, &FTextBlockStyle::SelectedBackgroundColor);
        OverwriteNonDefault(*Default, *ComboStyle, *Style, &FTextBlockStyle::HighlightColor);
        OverwriteNonDefault(*Default, *ComboStyle, *Style, &FTextBlockStyle::HighlightShape);
        OverwriteNonDefault(*Default, *ComboStyle, *Style, &FTextBlockStyle::StrikeBrush);
        OverwriteNonDefault(*Default, *ComboStyle, *Style, &FTextBlockStyle::UnderlineBrush);
    }
    return &WidgetStyleValues.Add(StyleName, ComboStyle);
}

void UHtmlTextBlock::SetTextStyles(const TMap<FName, FTextBlockStyle>& InTextStyles)
{
    TextStyles = InTextStyles;

    UpdateStyleData();

    if (MyRichTextBlock.IsValid())
    {
        MyRichTextBlock->SetDecoratorStyleSet(StyleInstance.Get());
        ApplyUpdatedDefaultTextStyle();
    }
}

TSharedPtr<IRichTextMarkupParser> UHtmlTextBlock::CreateMarkupParser()
{
    return FHtmlRichTextMarkupParser::GetStaticInstance();
}

void UHtmlTextBlock::UpdateStyleData()
{
    StyleInstance = MakeShareableDeferredCleanup(new FComboSlateStyleSet(TEXT("HtmlTextStyle")));

    for (const auto& [Name, Style] : TextStyles)
    {
        if (Name == DefaultStyleName)
        {
            DefaultTextStyle = Style;
        }

        StyleInstance->Set(Name, Style);
    }
}
