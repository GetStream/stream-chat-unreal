// Copyright 2021 Stream.IO, Inc. All Rights Reserved.

using UnrealBuildTool;

public class TokenProvider : ModuleRules
{
    public TokenProvider(ReadOnlyTargetRules Target) : base(Target)
    {
        PCHUsage = PCHUsageMode.UseExplicitOrSharedPCHs;
        CppStandard = CppStandardVersion.Cpp17;

        PublicDependencyModuleNames.AddRange(new[] {
            "Core",
            "CoreUObject",
            "Engine",
        });
    }
}
