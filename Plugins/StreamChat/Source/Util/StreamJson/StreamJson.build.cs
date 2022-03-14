// Copyright 2022 Stream.IO, Inc. All Rights Reserved.

using UnrealBuildTool;

public class StreamJson : ModuleRules
{
    public StreamJson(ReadOnlyTargetRules Target) : base(Target)
    {
        PCHUsage = PCHUsageMode.UseExplicitOrSharedPCHs;

        PublicDependencyModuleNames.AddRange(new[] { "Json", "Engine" });

        PrivateDependencyModuleNames.AddRange(new[] {
            "Core",
            "CoreUObject",
            "JsonUtilities"
        });
    }
}
