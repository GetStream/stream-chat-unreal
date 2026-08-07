// Copyright 2026 Stream.IO, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"

/**
 * @brief A file the app picked, ready to be uploaded and attached to a message.
 * @ingroup StreamChatUi
 */
struct FPickedAttachment
{
    /// Original filename, shown to recipients and used by the backend to infer the MIME type
    FString FileName;

    /// Raw bytes of the file
    TArray<uint8> Content;

    /// Upload through the image endpoint, so recipients get a thumbnail and render it inline
    bool bIsImage = false;
};

/**
 * @brief Supplies the message composer with a way to pick a file to attach.
 *
 * The SDK ships no picker of its own. Picking a file is a platform decision -- a photo library on a
 * handset, a file dialog on desktop, a baked-in asset in a game -- and the plugin stays portable by
 * not making it. Register a provider and the composer grows an attach button; register nothing and
 * it does not.
 *
 * @code
 * FAttachmentPicker::SetProvider([](FAttachmentPicker::FOnPicked OnPicked) {
 *     // Show your picker, then, whenever the user is done:
 *     FPickedAttachment Picked{TEXT("shot.png"), MoveTemp(Bytes), true};
 *     OnPicked(Picked);   // or OnPicked({}) if they cancelled
 * });
 * @endcode
 *
 * @ingroup StreamChatUi
 */
class STREAMCHATUI_API FAttachmentPicker
{
public:
    /// Called by a provider once the user has chosen a file, or with nothing if they cancelled.
    /// Always delivered on the game thread, whichever thread the provider calls it from.
    using FOnPicked = TFunction<void(const TOptional<FPickedAttachment>&)>;

    /// Shows a picker and reports back what the user chose
    using FProvider = TFunction<void(FOnPicked)>;

    /// Register the app's picker. Replaces any previously registered one.
    static void SetProvider(FProvider);
    static void ClearProvider();
    static bool HasProvider();

    /// Ask the registered provider for a file. Does nothing if none is registered.
    static void Pick(FOnPicked);

private:
    static FProvider Provider;
};
