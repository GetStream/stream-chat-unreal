// Copyright 2022 Stream.IO, Inc. All Rights Reserved.

using UnrealBuildTool;

public class HttpRequests : ModuleRules
{
    public HttpRequests(ReadOnlyTargetRules Target) : base(Target)
    {
        PCHUsage = PCHUsageMode.UseExplicitOrSharedPCHs;

        PublicDependencyModuleNames.AddRange(new[]
        {
			"HTTP",
			"StreamJson",
        });

        PrivateDependencyModuleNames.AddRange(new[] {
			"Core",
            "CoreUObject",
            // For stream chat plugin version
			"Projects",
        });
    }
}
