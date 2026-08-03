// Copyright 2026 Stream.IO, Inc. All Rights Reserved.

using UnrealBuildTool;

public class StreamChatWebSocket : ModuleRules
{
    public StreamChatWebSocket(ReadOnlyTargetRules Target) : base(Target)
    {
        PCHUsage = PCHUsageMode.UseExplicitOrSharedPCHs;

        PrivateDependencyModuleNames.AddRange(new[] {
            "CoreUObject",
            "WebSockets",
            "TokenProvider"
        });

        PublicDependencyModuleNames.AddRange(new[]
        {
            "Core",
            "Json",
            "StreamJson",
            "StreamChatDto",
        });
    }
}
