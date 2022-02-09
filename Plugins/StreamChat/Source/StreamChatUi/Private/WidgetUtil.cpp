// Copyright 2021 Stream.IO, Inc. All Rights Reserved.

#include "WidgetUtil.h"

#include "Engine/Texture2DDynamic.h"
#include "Fonts/FontMeasure.h"
#include "IImageWrapper.h"

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

UWidget* WidgetUtil::GetTypedChildWidget(const UWidget* Widget, const TSubclassOf<UWidget> Type)
{
    if (const UUserWidget* UserWidget = Cast<UUserWidget>(Widget))
    {
        if (UserWidget->WidgetTree)
        {
            UWidget* FoundWidget = nullptr;

            UserWidget->WidgetTree->ForEachWidget(
                [&](UWidget* W)
                {
                    if (W->IsA(Type))
                    {
                        FoundWidget = W;
                    }
                });

            return FoundWidget;
        }
    }
    return nullptr;
}

FString WidgetUtil::TruncateWithEllipsis(const FString& Input, const uint32 MaxWidth, const FSlateFontInfo& FontInfo)
{
    const TSharedRef<FSlateFontMeasure> Service = FSlateApplication::Get().GetRenderer()->GetFontMeasureService();
    if (Service->Measure(Input, FontInfo).X < MaxWidth)
    {
        int32 NewlineIndex;
        const bool bHasNewline = Input.FindChar(TEXT('\n'), NewlineIndex);
        return bHasNewline ? Input.Left(NewlineIndex) + TEXT("…") : Input;
    }

    int32 End, NewEnd = 0;
    bool bSingleWord = true;
    float TextSize;
    // Step forward as long as MaxWidth allows.
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

FLinearColor WidgetUtil::ChooseColorForString(const FString& String)
{
    return Colors[WidgetUtil::HashStringWithMax(String, 16)];
}
