// Copyright 2026 Stream.IO, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"

/**
 * @brief Registers this sample's file picker with the chat UI.
 *
 * The plugin has no picker of its own on purpose: it takes bytes, never a path, so that none of the
 * platform differences in choosing a file leak into it. This is the app's half of that bargain, and
 * it is where the one piece of Objective-C in the project lives.
 *
 * On iOS this puts up a PHPickerViewController. Everywhere else it registers nothing, and the
 * composer quietly leaves out its attach button.
 */
void RegisterSampleAttachmentPicker();
void UnregisterSampleAttachmentPicker();
