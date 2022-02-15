// Copyright 2021 Stream.IO, Inc. All Rights Reserved.

#include "Common/Html/HtmlTextBlock.h"

#include "Algo/Transform.h"
#include "Common/Html/HtmlParser.h"
#include "HtmlDecorators.h"
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

TMap<FString, FTextRange> GenerateMetaData(const TArray<FHtmlParser::FElement>& ElementStack, const FString& OutputString)
{
    TMap<FString, FTextRange> MetaData;
    if (ElementStack.Num() > 0)
    {
        for (const FHtmlParser::FElement& Elem : ElementStack)
        {
            for (const auto& [AttribName, AttribValue] : Elem.Attributes)
            {
                const int32 BeginIndex = static_cast<int32>(AttribValue.GetData() - *OutputString);
                const int32 EndIndex = BeginIndex + AttribValue.Len();
                const FTextRange Range{BeginIndex, EndIndex};
                MetaData.Add(FString{AttribName}, Range);
            }
        }
    }
    return MetaData;
}

FString MakeRunName(const TArray<FHtmlParser::FElement>& ElementStack)
{
    TSet<FStringView> ElementNames;
    Algo::Transform(ElementStack, ElementNames, [](auto&& Element) { return Element.Name; });
    return FString::Join(ElementNames, TEXT("_"));
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
        [&](const FHtmlParser& Self)
        {
            while (!Results.IsValidIndex(Self.Line))
            {
                Results.Add(FTextLineParseResults{{Self.ParagraphStartIndex, INDEX_NONE}});
                if (Results.Num() > 1)
                {
                    Results.Last(1).Range.EndIndex = Self.ParagraphStartIndex;
                }
            }

            const FString Name = MakeRunName(Self.ElementStack);
            FTextRunParseResults Run{Name, Self.GetOriginalRange(), Self.GetContentRange()};
            Run.MetaData = GenerateMetaData(Self.ElementStack, Self.GetOutput());

            Results[Self.Line].Runs.Add(Run);
        });

    const bool bSuccess = Parser.Parse();
    ensure(bSuccess);

    Output = Parser.GetOutput();
    Results.Last().Range.EndIndex = Output.Len();
}

FHtmlRichTextMarkupParser::FHtmlRichTextMarkupParser()
{
}

const FSlateWidgetStyle* FComboSlateStyleSet::GetWidgetStyleInternal(const FName DesiredTypeName, const FName StyleName) const
{
    TArray<FString> Tags;
    StyleName.ToString().ParseIntoArray(Tags, TEXT("_"));
    TArray<FName> TagNames;
    Algo::Transform(Tags, TagNames, [](const FString& Str) { return FName{Str}; });

    static FName TextBlockStyle{TEXT("FTextBlockStyle")};
    static FName InlineTextImageStyle{TEXT("FInlineTextImageStyle")};
    if (DesiredTypeName == TextBlockStyle)
    {
        if (const FSlateWidgetStyle* Style = CombinedStyleCache.Find(StyleName))
        {
            return Style;
        }

        const FTextBlockStyle NewStyle = Styles.MakeCombinedStyle(TagNames);
        return &CombinedStyleCache.Add(StyleName, NewStyle);
    }
    if (DesiredTypeName == InlineTextImageStyle && TagNames.Contains("li"))
    {
        return &Styles.Bullet;
    }

    return nullptr;
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

void UHtmlTextBlock::CreateDecorators(TArray<TSharedRef<ITextDecorator>>& OutDecorators)
{
    OutDecorators = {
        MakeShared<FHyperlinkHtmlDecorator>(),
        MakeShared<FListItemHtmlDecorator>(),
    };
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
