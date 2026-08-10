// Copyright 2026 Stream.IO, Inc. All Rights Reserved.

using UnrealBuildTool;

public class StreamChatSample : ModuleRules
{
	public StreamChatSample(ReadOnlyTargetRules Target) : base(Target)
	{
		PCHUsage = PCHUsageMode.UseExplicitOrSharedPCHs;

		PublicDependencyModuleNames.AddRange(new[] { "Core", "CoreUObject", "Engine", "InputCore" });

		// The module is flat, so a quoted include normally resolves against the including file's own
		// directory. IOS/ breaks that, and it has to live in a folder named for the platform: UBT
		// excludes source by directory name, and a .mm left in the module root is handed to whatever
		// compiler the host uses, MSVC included.
		PrivateIncludePaths.Add(ModuleDirectory);

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
