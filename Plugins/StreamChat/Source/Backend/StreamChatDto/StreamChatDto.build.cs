// Copyright 2026 Stream.IO, Inc. All Rights Reserved.

using UnrealBuildTool;

public class StreamChatDto : ModuleRules
{
    public StreamChatDto(ReadOnlyTargetRules Target) : base(Target)
    {
        PCHUsage = PCHUsageMode.UseExplicitOrSharedPCHs;

        PrivateDependencyModuleNames.AddRange(new[] {
            "CoreUObject",
            "Json",
        });

        PublicDependencyModuleNames.AddRange(new[] {
            "Core",
            "JsonUtilities",
            "StreamJson",
        });
    }
}
