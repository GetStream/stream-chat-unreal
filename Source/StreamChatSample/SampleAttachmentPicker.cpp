// Copyright 2026 Stream.IO, Inc. All Rights Reserved.

#include "SampleAttachmentPicker.h"

// The iOS picker is Objective-C and lives in IOSAttachmentPicker.mm, which only Apple platforms
// compile. This provides the same two symbols everywhere else, so the game module links whatever it
// is built for.
#if !PLATFORM_IOS

void RegisterSampleAttachmentPicker()
{
}

void UnregisterSampleAttachmentPicker()
{
}

#endif    // !PLATFORM_IOS
