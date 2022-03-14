// Copyright 2022 Stream.IO, Inc. All Rights Reserved.

using UnrealBuildTool;

public class StreamChatApi : ModuleRules
{
    public StreamChatApi(ReadOnlyTargetRules Target) : base(Target)
    {
        PCHUsage = PCHUsageMode.UseExplicitOrSharedPCHs;

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
