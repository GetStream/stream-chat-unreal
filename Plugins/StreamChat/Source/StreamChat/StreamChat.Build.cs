// Copyright Epic Games, Inc. All Rights Reserved.

using UnrealBuildTool;

public class StreamChat : ModuleRules
{
	public StreamChat(ReadOnlyTargetRules Target) : base(Target)
	{
		PCHUsage = PCHUsageMode.UseExplicitOrSharedPCHs;

		PublicDependencyModuleNames.AddRange(
			new string[]
			{
				"Core",
				// ... add other public dependencies that you statically link with here ...
			}
			);


		PrivateDependencyModuleNames.AddRange(
			new string[]
			{
				"CoreUObject",
				"Engine",
				"WebSockets",
				"Json",
				"JsonUtilities",
				"StreamJson",
				"HttpRequests"
				// ... add private dependencies that you statically link with here ...
			}
			);
	}
}
