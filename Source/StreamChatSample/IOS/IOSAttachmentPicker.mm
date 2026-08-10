// Copyright 2026 Stream.IO, Inc. All Rights Reserved.

#include "SampleAttachmentPicker.h"

// Compiled on every Apple platform but only meant for the handset. The matching no-op lives in
// SampleAttachmentPicker.cpp, which takes over wherever this is empty or not compiled at all.
#if PLATFORM_IOS

#include "IOS/IOSAppDelegate.h"
#include "Input/AttachmentPicker.h"

#import <PhotosUI/PhotosUI.h>
#import <UIKit/UIKit.h>

namespace
{
constexpr CGFloat JpegQuality = 0.9;

FString ToFString(NSString* String)
{
    return String ? FString{UTF8_TO_TCHAR([String UTF8String])} : FString{};
}
}    // namespace

/**
 * Keeps the SDK's callback alive while the system picker is on screen, and turns whatever the user
 * chose into the filename and bytes the SDK asked for.
 *
 * PHPickerViewController holds its delegate weakly, so this object owns itself between being handed
 * to the picker and reporting back. UE builds Objective-C++ without ARC, hence the explicit release.
 */
@interface FStreamSamplePhotoPicker : NSObject <PHPickerViewControllerDelegate>
- (instancetype)initWithCallback:(FAttachmentPicker::FOnPicked)Callback;
@end

@implementation FStreamSamplePhotoPicker
{
    FAttachmentPicker::FOnPicked OnPicked;
}

- (instancetype)initWithCallback:(FAttachmentPicker::FOnPicked)Callback
{
    self = [super init];
    if (self)
    {
        OnPicked = MoveTemp(Callback);
    }
    return self;
}

/// Hand the result back exactly once, then give up the reference that kept us alive
- (void)report:(const TOptional<FPickedAttachment>&)Picked
{
    if (OnPicked)
    {
        OnPicked(Picked);
        OnPicked.Reset();
    }
    [self release];
}

- (void)picker:(PHPickerViewController*)Picker didFinishPicking:(NSArray<PHPickerResult*>*)Results
{
    [Picker dismissViewControllerAnimated:YES completion:nil];

    PHPickerResult* Result = Results.firstObject;
    if (!Result || ![Result.itemProvider canLoadObjectOfClass:[UIImage class]])
    {
        // Cancelled, or gave us something we cannot read
        [self report:TOptional<FPickedAttachment>{}];
        return;
    }

    NSString* SuggestedName = Result.itemProvider.suggestedName;
    [Result.itemProvider
        loadObjectOfClass:[UIImage class]
        completionHandler:^(__kindof id<NSItemProviderReading> Object, NSError* Error)
        {
            UIImage* Photo = [Object isKindOfClass:[UIImage class]] ? (UIImage*)Object : nil;

            // Re-encode as JPEG rather than uploading what the library holds. An iPhone stores HEIC,
            // Stream serves back what it was given, and the SDK's image cache only decodes PNG, JPEG
            // and BMP -- so a HEIC photo would upload cleanly and then fail to draw in the message.
            NSData* Jpeg = Photo ? UIImageJPEGRepresentation(Photo, JpegQuality) : nil;
            if (!Jpeg || Jpeg.length == 0)
            {
                [self report:TOptional<FPickedAttachment>{}];
                return;
            }

            const FString BaseName = ToFString(SuggestedName);

            FPickedAttachment Picked;
            Picked.FileName = (BaseName.IsEmpty() ? FString{TEXT("photo")} : BaseName) + TEXT(".jpg");
            Picked.bIsImage = true;
            Picked.Content.Append(static_cast<const uint8*>(Jpeg.bytes), static_cast<int32>(Jpeg.length));

            [self report:TOptional<FPickedAttachment>{MoveTemp(Picked)}];
        }];
}

@end

namespace
{
void PresentPhotoPicker(FAttachmentPicker::FOnPicked OnPicked)
{
    dispatch_async(
        dispatch_get_main_queue(),
        ^{
            // IOSAppDelegate.h only forward declares IOSViewController, so the compiler cannot see
            // that it descends from UIViewController. It always does -- directly on tvOS/visionOS
            // and via GCEventViewController everywhere else -- so the cast is safe, and it saves
            // pulling IOSView.h and the GameController framework in behind it.
            UIViewController* Presenter = (UIViewController*)[IOSAppDelegate GetDelegate].IOSController;
            if (!Presenter)
            {
                OnPicked({});
                return;
            }

            PHPickerConfiguration* Configuration = [[[PHPickerConfiguration alloc] init] autorelease];
            Configuration.selectionLimit = 1;
            Configuration.filter = [PHPickerFilter imagesFilter];

            PHPickerViewController* Picker = [[[PHPickerViewController alloc] initWithConfiguration:Configuration] autorelease];

            // The +1 from alloc is deliberate: the picker's delegate reference is weak, so this is
            // what keeps the delegate alive until it reports back and releases itself.
            Picker.delegate = [[FStreamSamplePhotoPicker alloc] initWithCallback:OnPicked];

            [Presenter presentViewController:Picker animated:YES completion:nil];
        });
}
}    // namespace

void RegisterSampleAttachmentPicker()
{
    FAttachmentPicker::SetProvider([](FAttachmentPicker::FOnPicked OnPicked) { PresentPhotoPicker(MoveTemp(OnPicked)); });
}

void UnregisterSampleAttachmentPicker()
{
    FAttachmentPicker::ClearProvider();
}

#endif    // PLATFORM_IOS
