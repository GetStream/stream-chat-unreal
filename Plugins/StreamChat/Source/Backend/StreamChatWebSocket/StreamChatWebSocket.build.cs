// Copyright 2022 Stream.IO, Inc. All Rights Reserved.

using UnrealBuildTool;

public class StreamChatWebSocket : ModuleRules
{
    public StreamChatWebSocket(ReadOnlyTargetRules Target) : base(Target)
    {
        PCHUsage = PCHUsageMode.UseExplicitOrSharedPCHs;
        CppStandard = CppStandardVersion.Cpp17;

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
