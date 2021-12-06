// Copyright 2021 Stream.IO, Inc. All Rights Reserved.

using UnrealBuildTool;

public class StreamChatDto : ModuleRules
{
    public StreamChatDto(ReadOnlyTargetRules Target) : base(Target)
    {
        PCHUsage = PCHUsageMode.UseExplicitOrSharedPCHs;
        CppStandard = CppStandardVersion.Cpp17;

        PrivateDependencyModuleNames.AddRange(new[] {
            "CoreUObject",
            "Json",
            "StreamJson",
        });

        PublicDependencyModuleNames.AddRange(new[] {
            "Core",
            "JsonUtilities",
        });
    }
}
