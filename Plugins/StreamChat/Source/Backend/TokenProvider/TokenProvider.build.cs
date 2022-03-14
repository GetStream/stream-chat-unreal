// Copyright 2022 Stream.IO, Inc. All Rights Reserved.

using UnrealBuildTool;

public class TokenProvider : ModuleRules
{
    public TokenProvider(ReadOnlyTargetRules Target) : base(Target)
    {
        PCHUsage = PCHUsageMode.UseExplicitOrSharedPCHs;

        PublicDependencyModuleNames.AddRange(new[] {
            "Core",
            "CoreUObject",
        });
    }
}
