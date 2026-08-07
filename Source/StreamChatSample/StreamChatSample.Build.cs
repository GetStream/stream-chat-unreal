// Copyright 2026 Stream.IO, Inc. All Rights Reserved.

using UnrealBuildTool;

public class StreamChatSample : ModuleRules
{
	public StreamChatSample(ReadOnlyTargetRules Target) : base(Target)
	{
		PCHUsage = PCHUsageMode.UseExplicitOrSharedPCHs;

		PublicDependencyModuleNames.AddRange(new[] { "Core", "CoreUObject", "Engine", "InputCore" });

		// StreamChatUi is here only for FAttachmentPicker, which the sample fills in with the photo
		// picker below. The plugin itself never touches a platform framework.
		PrivateDependencyModuleNames.AddRange(new[] { "StreamChat", "StreamChatUi" });

		if (Target.Platform == UnrealTargetPlatform.IOS)
		{
			PrivateDependencyModuleNames.Add("ApplicationCore");
			PublicFrameworks.AddRange(new[] { "Photos", "PhotosUI" });
		}
	}
}
