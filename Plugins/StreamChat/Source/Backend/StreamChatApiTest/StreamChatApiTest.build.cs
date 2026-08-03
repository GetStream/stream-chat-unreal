// Copyright 2026 Stream.IO, Inc. All Rights Reserved.

using UnrealBuildTool;

public class StreamChatApiTest : ModuleRules
{
    public StreamChatApiTest(ReadOnlyTargetRules Target) : base(Target)
    {
        PCHUsage = PCHUsageMode.UseExplicitOrSharedPCHs;

        PrivateDependencyModuleNames.AddRange(new[] {
            "StreamChatApi",
            "StreamChatDto",
            "StreamChatWebSocket",
            "TokenProvider",
            "StreamJson",
            "Json",
            "StreamChat",
        });

        PublicDependencyModuleNames.AddRange(new[] {
            "Core",
        });
    }
}
