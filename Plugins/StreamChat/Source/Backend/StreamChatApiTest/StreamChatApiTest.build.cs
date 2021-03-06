// Copyright 2022 Stream.IO, Inc. All Rights Reserved.

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
            "StreamJson",
            "Json",
            "StreamChat",
        });

        PublicDependencyModuleNames.AddRange(new[] {
            "Core",
        });
    }
}
