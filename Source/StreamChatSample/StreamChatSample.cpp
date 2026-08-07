// Copyright 2026 Stream.IO, Inc. All Rights Reserved.

#include "StreamChatSample.h"

#include "Modules/ModuleManager.h"
#include "SampleAttachmentPicker.h"

/**
 * The sample hands the chat UI a file picker as the module starts, well before any widget is built,
 * because the composer decides whether to offer an attach button by asking whether one exists.
 */
class FStreamChatSampleModule final : public FDefaultGameModuleImpl
{
    virtual void StartupModule() override
    {
        FDefaultGameModuleImpl::StartupModule();
        RegisterSampleAttachmentPicker();
    }

    virtual void ShutdownModule() override
    {
        UnregisterSampleAttachmentPicker();
        FDefaultGameModuleImpl::ShutdownModule();
    }
};

IMPLEMENT_PRIMARY_GAME_MODULE(FStreamChatSampleModule, StreamChatSample, "StreamChatSample");
