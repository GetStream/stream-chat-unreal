// Copyright 2026 Stream.IO, Inc. All Rights Reserved.

using UnrealBuildTool;

public class StreamChatUi : ModuleRules
{
    public StreamChatUi(ReadOnlyTargetRules Target) : base(Target)
    {
        PCHUsage = PCHUsageMode.UseExplicitOrSharedPCHs;

        PrivateDependencyModuleNames.AddRange(new[] {
            "Slate",
            "InputCore",
            "ApplicationCore",
            "HTTP",
            "RenderCore",
            "RHI",
            "ImageWrapper",
            "Json",
            "XmlParser"
        });

        PublicDependencyModuleNames.AddRange(new[] {
            "Core",
            "CoreUObject",
            "Engine",
            "UMG",
            "SlateCore",
            "StreamChat",
            "StreamJson",
        });
    }
}
