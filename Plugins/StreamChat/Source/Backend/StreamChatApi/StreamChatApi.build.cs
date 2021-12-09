// Copyright 2021 Stream.IO, Inc. All Rights Reserved.

using UnrealBuildTool;

public class StreamChatApi : ModuleRules
{
    public StreamChatApi(ReadOnlyTargetRules Target) : base(Target)
    {
        PCHUsage = PCHUsageMode.UseExplicitOrSharedPCHs;
        CppStandard = CppStandardVersion.Cpp17;

        PrivateDependencyModuleNames.AddRange(new[]
        {
            "CoreUObject",
            "HttpRequests",
            "TokenProvider",
            "Json",
        });

        PublicDependencyModuleNames.AddRange(new[]
        {
            "Core",
            "JsonUtilities",
            "StreamJson",
            "StreamChatDto",
        });
    }
}
