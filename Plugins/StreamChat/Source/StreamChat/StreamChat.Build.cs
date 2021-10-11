// Copyright Epic Games, Inc. All Rights Reserved.

using UnrealBuildTool;

public class StreamChat : ModuleRules
{
	public StreamChat(ReadOnlyTargetRules Target) : base(Target)
	{
		PCHUsage = PCHUsageMode.UseExplicitOrSharedPCHs;
		CppStandard = CppStandardVersion.Cpp17;

		PrivateDependencyModuleNames.AddRange(
			new[]
			{
				"Core",
				"CoreUObject",
				"Engine",
				"WebSockets",
				"StreamJson",
				"HttpRequests"
			}
			);
	}
}
