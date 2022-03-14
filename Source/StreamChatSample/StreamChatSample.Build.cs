// Copyright 2022 Stream.IO, Inc. All Rights Reserved.

// Copyright Stream.IO, Inc. All Rights Reserved.

using UnrealBuildTool;

public class StreamChatSample : ModuleRules
{
	public StreamChatSample(ReadOnlyTargetRules Target) : base(Target)
	{
		PCHUsage = PCHUsageMode.UseExplicitOrSharedPCHs;

		PublicDependencyModuleNames.AddRange(new[] { "Core", "CoreUObject", "Engine", "InputCore" });

		PrivateDependencyModuleNames.AddRange(new[] { "StreamChat" });
	}
}
