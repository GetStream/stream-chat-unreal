// Copyright 2026 Stream.IO, Inc. All Rights Reserved.

using UnrealBuildTool;

public class StreamChatEditor : ModuleRules
{
    public StreamChatEditor(ReadOnlyTargetRules Target) : base(Target)
    {
        PCHUsage = PCHUsageMode.UseExplicitOrSharedPCHs;

        PrivateDependencyModuleNames.AddRange(new[]
        {
            "Core",
            "CoreUObject",
            "Engine",
            "Settings",
            "StreamChat",
            "StreamChatWebSocket",
        });
    }
}
