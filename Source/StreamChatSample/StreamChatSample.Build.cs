// Copyright 2022 Stream.IO, Inc. All Rights Reserved.

// Copyright Stream.IO, Inc. All Rights Reserved.

using UnrealBuildTool;

public class StreamChatSample : ModuleRules
{
	public StreamChatSample(ReadOnlyTargetRules Target) : base(Target)
	{
		PCHUsage = PCHUsageMode.UseExplicitOrSharedPCHs;
		CppStandard = CppStandardVersion.Cpp17;

		PublicDependencyModuleNames.AddRange(new string[] { "Core", "CoreUObject", "Engine", "InputCore" });

		PrivateDependencyModuleNames.AddRange(new string[] { "StreamChat" });
	}
}
