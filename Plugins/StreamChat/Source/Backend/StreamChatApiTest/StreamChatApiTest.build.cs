// Copyright 2021 Stream.IO, Inc. All Rights Reserved.

using UnrealBuildTool;

public class StreamChatApiTest : ModuleRules
{
    public StreamChatApiTest(ReadOnlyTargetRules Target) : base(Target)
    {
        PCHUsage = PCHUsageMode.UseExplicitOrSharedPCHs;
        CppStandard = CppStandardVersion.Cpp17;

        PrivateDependencyModuleNames.AddRange(new[] {
            "StreamChatApi",
            "StreamChatDto",
            "StreamChatWebSocket",
            "TokenProvider",
            "Jwt",
            "StreamJson",
        });

        PublicDependencyModuleNames.AddRange(new[] {
            "Core",
        });
    }
}
