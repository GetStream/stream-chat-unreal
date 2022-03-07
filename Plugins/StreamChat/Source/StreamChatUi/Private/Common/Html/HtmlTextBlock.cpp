// Copyright 2022 Stream.IO, Inc. All Rights Reserved.

#include "Common/Html/HtmlTextBlock.h"

#include "Algo/Transform.h"
#include "HtmlDecorators.h"
#include "HtmlParser.h"
#include "HtmlRichTextMarkupParser.h"
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

const FSlateWidgetStyle* FHtmlSlateStyleSet::GetWidgetStyleInternal(const FName DesiredTypeName, const FName StyleName) const
{
    TArray<FString> Tags;
    StyleName.ToString().ParseIntoArray(Tags, TEXT("_"));
    TArray<FName> TagNames;
    Algo::Transform(Tags, TagNames, [](const FString& Str) { return FName{Str}; });

    static const FName TextBlockStyle{TEXT("FTextBlockStyle")};
    static const FName InlineTextImageStyle{TEXT("FInlineTextImageStyle")};
    if (DesiredTypeName == TextBlockStyle)
    {
        if (const FSlateWidgetStyle* Style = CombinedStyleCache.Find(StyleName))
        {
            return Style;
        }

        const FTextBlockStyle NewStyle = Styles.MakeCombinedStyle(TagNames);
        return &CombinedStyleCache.Add(StyleName, NewStyle);
    }
    if (DesiredTypeName == InlineTextImageStyle && TagNames.Contains(HtmlTag::HorizontalRule))
    {
        return &Styles.HorizontalRule;
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
        MakeShared<FHorizontalRuleHtmlDecorator>(),
    };
}

TSharedPtr<IRichTextMarkupParser> UHtmlTextBlock::CreateMarkupParser()
{
    return FHtmlRichTextMarkupParser::GetStaticInstance();
}

void UHtmlTextBlock::UpdateStyleData()
{
    StyleInstance = MakeShareableDeferredCleanup(new FHtmlSlateStyleSet(TEXT("HtmlTextStyle"), Styles));
    DefaultTextStyle = Styles.Default;
}
