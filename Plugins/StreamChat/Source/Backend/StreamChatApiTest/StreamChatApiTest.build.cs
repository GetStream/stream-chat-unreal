// Copyright 2026 Stream.IO, Inc. All Rights Reserved.

using UnrealBuildTool;

public class StreamChatApiTest : ModuleRules
{
    public StreamChatApiTest(ReadOnlyTargetRules Target) : base(Target)
    {
        PCHUsage = PCHUsageMode.UseExplicitOrSharedPCHs;

        // The specs capture 'this' through a '=' default, which C++20 requires. Projects on UE 5.5
        // with DefaultBuildSettings below V4 still default to C++17, so ask for C++20 explicitly.
        CppStandard = CppStandardVersion.Cpp20;

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
