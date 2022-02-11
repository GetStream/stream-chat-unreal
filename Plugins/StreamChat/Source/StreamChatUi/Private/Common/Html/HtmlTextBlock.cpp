// Copyright 2021 Stream.IO, Inc. All Rights Reserved.

#include "Common/Html/HtmlTextBlock.h"

#include "Algo/Transform.h"
#include "Common/Html/HtmlParser.h"
#include "RenderingThread.h"
#include "Widgets/Text/SRichTextBlock.h"

namespace
{
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
        [&](const FString& Content, const FHtmlParser& Self)
        {
            while (!Results.IsValidIndex(Self.Line))
            {
                if (Self.Line > 0)
                {
                    Output.AppendChar(TEXT('\n'));
                }
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
    const bool bSuccess = Parser.Parse();
    ensure(bSuccess);
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
    if (const FSlateWidgetStyle* Style = CombinedStyleCache.Find(StyleName))
    {
        return Style;
    }

    TArray<FString> Tags;
    StyleName.ToString().ParseIntoArray(Tags, TEXT("_"));
    TArray<FName> TagNames;
    Algo::Transform(Tags, TagNames, [](const FString& Str) { return FName{Str}; });

    const FTextBlockStyle NewStyle = Styles.MakeCombinedStyle(TagNames);
    return &CombinedStyleCache.Add(StyleName, NewStyle);
}

void UHtmlTextBlock::SetHtmlStyles(const FHtmlElementStyles& InStyles)
{
    Styles = InStyles;

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
    StyleInstance = MakeShareableDeferredCleanup(new FComboSlateStyleSet(TEXT("HtmlTextStyle"), Styles));
    DefaultTextStyle = Styles.Default;
}
