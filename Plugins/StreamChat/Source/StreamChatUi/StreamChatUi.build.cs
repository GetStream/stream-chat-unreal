// Copyright 2021 Stream.IO, Inc. All Rights Reserved.

using UnrealBuildTool;

public class StreamChatUi : ModuleRules
{
    public StreamChatUi(ReadOnlyTargetRules Target) : base(Target)
    {
        PCHUsage = PCHUsageMode.UseExplicitOrSharedPCHs;
        CppStandard = CppStandardVersion.Cpp17;

        PrivateDependencyModuleNames.AddRange(new[] {
            "Slate",
            "InputCore",
            "ApplicationCore",
            "Http",
            "RenderCore",
            "RHI",
            "Json"
        });

        PublicDependencyModuleNames.AddRange(new[] {
            "Core",
            "CoreUObject",
            "Engine",
            "UMG",
            "SlateCore",
            "StreamChat"
        });
    }
}
