// Copyright 2026 Stream.IO, Inc. All Rights Reserved.

#include "Input/AttachmentPicker.h"

#include "Async/Async.h"

FAttachmentPicker::FProvider FAttachmentPicker::Provider;

void FAttachmentPicker::SetProvider(FProvider InProvider)
{
    Provider = MoveTemp(InProvider);
}

void FAttachmentPicker::ClearProvider()
{
    Provider = nullptr;
}

bool FAttachmentPicker::HasProvider()
{
    return static_cast<bool>(Provider);
}

void FAttachmentPicker::Pick(FOnPicked OnPicked)
{
    if (!Provider || !OnPicked)
    {
        return;
    }

    Provider(
        [OnPicked = MoveTemp(OnPicked)](const TOptional<FPickedAttachment>& Picked)
        {
            if (IsInGameThread())
            {
                OnPicked(Picked);
                return;
            }

            // A platform picker reports back on whichever thread it pleases, and UIKit's main thread
            // is not the game thread. Everything downstream of here touches UObjects.
            AsyncTask(ENamedThreads::GameThread, [OnPicked, Copy = Picked]() { OnPicked(Copy); });
        });
}
