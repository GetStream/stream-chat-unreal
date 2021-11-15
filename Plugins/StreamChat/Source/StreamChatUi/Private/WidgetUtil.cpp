#include "WidgetUtil.h"

#include "Engine/Texture2DDynamic.h"
#include "Fonts/FontMeasure.h"
#include "IImageWrapper.h"

UWidget* WidgetUtil::GetTypedParentWidget(const UWidget* Widget, const TSubclassOf<UWidget> Type)
{
    UWidget* NextOuter = const_cast<UWidget*>(Widget);
    while (NextOuter)
    {
        if (NextOuter->IsA(Type))
        {
            return NextOuter;
        }

        if (NextOuter->Slot)
        {
            NextOuter = NextOuter->Slot->Parent;
            continue;
        }

        if (const UObject* WidgetTree = NextOuter->GetOuter())
        {
            if (UUserWidget* UserWidget = Cast<UUserWidget>(WidgetTree->GetOuter()))
            {
                NextOuter = UserWidget;
                continue;
            }
        }
        break;
    }
    return nullptr;
}

FString WidgetUtil::TruncateWithEllipsis(const FString& Input, const uint32 MaxWidth, const FSlateFontInfo& FontInfo)
{
    const TSharedRef<FSlateFontMeasure> Service = FSlateApplication::Get().GetRenderer()->GetFontMeasureService();
    if (Service->Measure(Input, FontInfo).X < MaxWidth)
    {
        return Input;
    }

    // Step forward as long as textWidth allows.
    int32 End, NewEnd = 0;
    bool bSingleWord = true;
    float TextSize;
    do
    {
        End = NewEnd;
        NewEnd = Input.Find(TEXT(" "), ESearchCase::IgnoreCase, ESearchDir::FromStart, End + 1);
        if (NewEnd == INDEX_NONE)
        {
            if (bSingleWord)
            {
                // Input string was only a single word - split by character
                NewEnd = End + 1;
            }
            else
            {
                NewEnd = Input.Len();
            }
        }
        else
        {
            bSingleWord = false;
        }
        const FString Shortened = Input.Left(NewEnd) + TEXT("…");
        TextSize = Service->Measure(Shortened, FontInfo).X;

    } while (TextSize < MaxWidth);

    return Input.Left(End) + TEXT("…");
}

uint32 WidgetUtil::HashStringWithMax(const FString& String, const uint32 Max)
{
    const uint32 Hash = GetTypeHash(String);
    const uint32 Reduced = Hash & (Max - 1);
    check(Reduced < Max);
    return Reduced;
}
