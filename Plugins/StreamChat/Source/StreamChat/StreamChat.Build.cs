// Copyright 2021 Stream.IO, Inc. All Rights Reserved.

// Copyright Stream.IO, Inc. All Rights Reserved.

using UnrealBuildTool;

public class StreamChat : ModuleRules
{
	public StreamChat(ReadOnlyTargetRules Target) : base(Target)
	{
		PCHUsage = PCHUsageMode.UseExplicitOrSharedPCHs;
		CppStandard = CppStandardVersion.Cpp17;

		PrivateDependencyModuleNames.AddRange(new[]
		{
			"Json", // Filter
			"JsonUtilities", // Filter
			"StreamJson",
			"Jwt",
			"TokenProvider",
		});

		PublicDependencyModuleNames.AddRange(new[]
		{
			"Core",
			"CoreUObject",
			"Engine",
			"StreamChatWebSocket",
			"StreamChatApi", // Send event template
			"StreamChatDto", // Send event template
		});
	}
}
