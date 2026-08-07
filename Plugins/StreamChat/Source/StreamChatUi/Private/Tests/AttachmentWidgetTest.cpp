// Copyright 2026 Stream.IO, Inc. All Rights Reserved.

#include "Blueprint/WidgetTree.h"
#include "Components/Border.h"
#include "Components/Image.h"
#include "Components/SizeBox.h"
#include "Components/TextBlock.h"
#include "CoreTypes.h"
#include "Message/AttachmentLayout.h"
#include "Message/AttachmentWidget.h"
#include "Misc/AutomationTest.h"

#if WITH_DEV_AUTOMATION_TESTS

namespace
{
/**
 * Build the widget the way the message list does, minus the viewport.
 *
 * CreateWidget wants a world, which an offline test has no business standing up, so the two steps it
 * would do are done by hand. Initialize() is public on UUserWidget and private in UStreamWidget, so
 * it is called through a base pointer.
 */
UAttachmentWidget* MakeAttachmentWidget(const FAttachment& Attachment)
{
    // Defaulted outer rather than GetTransientPackage(): it is the same transient package, without
    // needing UPackage to be a complete type in a translation unit that has no other use for it.
    UAttachmentWidget* Widget = NewObject<UAttachmentWidget>();
    static_cast<UUserWidget*>(Widget)->Initialize();
    Widget->Setup(Attachment, EMessageSide::You);
    return Widget;
}

template <class T>
T* FindFirst(const UAttachmentWidget& Widget)
{
    T* Found = nullptr;
    Widget.WidgetTree->ForEachWidget(
        [&Found](UWidget* Child)
        {
            if (!Found)
            {
                Found = Cast<T>(Child);
            }
        });
    return Found;
}

/// Every text block in the widget, in tree order: the filename first, then the type and size
TArray<UTextBlock*> FindTextBlocks(const UAttachmentWidget& Widget)
{
    TArray<UTextBlock*> Found;
    Widget.WidgetTree->ForEachWidget(
        [&Found](UWidget* Child)
        {
            if (UTextBlock* AsText = Cast<UTextBlock>(Child))
            {
                Found.Add(AsText);
            }
        });
    return Found;
}

bool IsShown(const UWidget* Widget)
{
    return Widget && Widget->GetVisibility() != ESlateVisibility::Collapsed && Widget->GetVisibility() != ESlateVisibility::Hidden;
}
}    // namespace

IMPLEMENT_SIMPLE_AUTOMATION_TEST(
    FAttachmentWidgetTest,
    "StreamChat.Ui.AttachmentWidget",
    EAutomationTestFlags_ApplicationContextMask | EAutomationTestFlags::ProductFilter)

bool FAttachmentWidgetTest::RunTest(const FString& Parameters)
{
    // An image is drawn inline, and the row naming the file stays out of the way
    {
        FAttachment Attachment;
        Attachment.Type = EAttachmentType::Image;
        Attachment.ImageUrl = TEXT("https://example.com/shot.png");
        Attachment.Title = TEXT("shot.png");

        const UAttachmentWidget* Widget = MakeAttachmentWidget(Attachment);
        TestTrue("Image is shown", IsShown(FindFirst<USizeBox>(*Widget)));
        TestFalse("File row is hidden", IsShown(FindFirst<UBorder>(*Widget)));
    }

    // Anything we cannot draw gets the row instead
    {
        FAttachment Attachment;
        Attachment.Type = EAttachmentType::File;
        Attachment.AssetUrl = TEXT("https://example.com/report.pdf");
        Attachment.Title = TEXT("report.pdf");
        Attachment.MimeType = TEXT("application/pdf");
        Attachment.FileSize = 2048;

        const UAttachmentWidget* Widget = MakeAttachmentWidget(Attachment);
        TestFalse("Image is hidden", IsShown(FindFirst<USizeBox>(*Widget)));
        TestTrue("File row is shown", IsShown(FindFirst<UBorder>(*Widget)));

        const TArray<UTextBlock*> Texts = FindTextBlocks(*Widget);
        if (TestEqual("Title and subtitle", Texts.Num(), 2))
        {
            TestEqual("Title is the filename", Texts[0]->GetText().ToString(), TEXT("report.pdf"));
            TestTrue("Subtitle names the type", Texts[1]->GetText().ToString().Contains(TEXT("application/pdf")));
            TestTrue("Subtitle is shown", IsShown(Texts[1]));
        }
    }

    // An image type with no URL cannot be drawn, so it must not leave an empty box behind
    {
        FAttachment Attachment;
        Attachment.Type = EAttachmentType::Image;

        const UAttachmentWidget* Widget = MakeAttachmentWidget(Attachment);
        TestFalse("Image is hidden", IsShown(FindFirst<USizeBox>(*Widget)));
        TestTrue("File row is shown", IsShown(FindFirst<UBorder>(*Widget)));
    }

    // A scraped link arrives with no title at all, so the filename comes from the URL, and the query
    // string CDNs sign their URLs with must not end up in it
    {
        FAttachment Attachment;
        Attachment.Type = EAttachmentType::File;
        Attachment.AssetUrl = TEXT("https://cdn.example.com/1146612/files/report.pdf?Key-Pair-Id=ABC&Signature=xyz");

        const UAttachmentWidget* Widget = MakeAttachmentWidget(Attachment);
        const TArray<UTextBlock*> Texts = FindTextBlocks(*Widget);
        if (TestEqual("Title and subtitle", Texts.Num(), 2))
        {
            TestEqual("Title from URL", Texts[0]->GetText().ToString(), TEXT("report.pdf"));
            TestFalse("No subtitle to show", IsShown(Texts[1]));
        }
    }

    // Setting the widget up twice must replace the previous contents rather than stack onto them,
    // because UStreamWidget::Initialize already ran Setup once with an empty attachment
    {
        FAttachment Attachment;
        Attachment.Type = EAttachmentType::File;
        Attachment.Title = TEXT("first.txt");

        UAttachmentWidget* Widget = MakeAttachmentWidget(Attachment);

        FAttachment Replacement;
        Replacement.Type = EAttachmentType::File;
        Replacement.Title = TEXT("second.txt");
        Widget->Setup(Replacement, EMessageSide::Me);

        const TArray<UTextBlock*> Texts = FindTextBlocks(*Widget);
        if (TestEqual("No leftovers from the first setup", Texts.Num(), 2))
        {
            TestEqual("Shows the second attachment", Texts[0]->GetText().ToString(), TEXT("second.txt"));
        }
    }

    return true;
}

IMPLEMENT_SIMPLE_AUTOMATION_TEST(
    FAttachmentLayoutTest,
    "StreamChat.Ui.AttachmentLayout",
    EAutomationTestFlags_ApplicationContextMask | EAutomationTestFlags::ProductFilter)

bool FAttachmentLayoutTest::RunTest(const FString& Parameters)
{
    constexpr float Budget = 260.f;
    constexpr float MaxAspect = 2.f;

    // A photo off a phone camera. It must come out at the budget width, not its own 4284 pixels:
    // drawn at source size it covered the entire message list.
    {
        const FVector2D Size = AttachmentLayout::FitToBudget(FIntPoint{4284, 5712}, Budget, MaxAspect, 2.f);
        TestEqual("Fitted to the budget width", Size.X, 260.0);
        TestEqual("Height follows the aspect ratio", Size.Y, 347.0);
    }

    // Aspect ratio is never distorted to make something fit
    {
        const FVector2D Size = AttachmentLayout::FitToBudget(FIntPoint{1000, 250}, Budget, MaxAspect, 1.f);
        TestEqual("Fitted to the budget width", Size.X, 260.0);
        TestEqual("Stays 4:1", Size.Y, 65.0);
    }

    // Something smaller than the budget is left alone rather than blown up into a blurry mess
    {
        const FVector2D Size = AttachmentLayout::FitToBudget(FIntPoint{120, 80}, Budget, MaxAspect, 1.f);
        TestEqual("Kept at its own width", Size.X, 120.0);
        TestEqual("Kept at its own height", Size.Y, 80.0);
    }

    // A very tall image shrinks as a whole, so it cannot run off the bottom of the list
    {
        const FVector2D Size = AttachmentLayout::FitToBudget(FIntPoint{1000, 10000}, Budget, MaxAspect, 1.f);
        TestEqual("Height capped at the budget times the aspect limit", Size.Y, 520.0);
        TestEqual("Width shrunk to match, not squashed", Size.X, 52.0);
    }

    // Nothing to measure yet, before the backend has told us anything about the image
    {
        const FVector2D Size = AttachmentLayout::FitToBudget(FIntPoint{0, 0}, Budget, MaxAspect, 1.f);
        TestEqual("Reserves the budget width", Size.X, 260.0);
        TestTrue("Reserves some height", Size.Y > 0.0);
    }

    // Stream's CDN resizes on request, which is what keeps a 25 megapixel photo from being decoded
    // in full on a handset. The signed URL already has a query string, so this has to append to it.
    {
        const FString Signed = TEXT("https://dublin.stream-io-cdn.com/1146612/images/a.jpg?Key-Pair-Id=ABC&Signature=xyz");
        const FString Resized = AttachmentLayout::WithResizeQuery(Signed, FIntPoint{512, 1024});
        TestTrue("Appends to the existing query", Resized.StartsWith(Signed + TEXT("&")));
        TestTrue("Fits within the box without distorting", Resized.EndsWith(TEXT("w=512&h=1024&resize=clip")));
    }

    // Any other host is left alone: the parameters are Stream's, and a stranger may reject them
    {
        const FString Foreign = TEXT("https://upload.wikimedia.org/wikipedia/commons/a.jpg");
        TestEqual("Untouched", AttachmentLayout::WithResizeQuery(Foreign, FIntPoint{512, 1024}), Foreign);
    }

    return true;
}
#endif
