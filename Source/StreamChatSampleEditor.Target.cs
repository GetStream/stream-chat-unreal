// Copyright 2021 Stream.IO, Inc. All Rights Reserved.

// Copyright Stream.IO, Inc. All Rights Reserved.

using UnrealBuildTool;
using System.Collections.Generic;

public class StreamChatSampleEditorTarget : TargetRules
{
	public StreamChatSampleEditorTarget( TargetInfo Target) : base(Target)
	{
		Type = TargetType.Editor;
		DefaultBuildSettings = BuildSettingsVersion.V2;
		ExtraModuleNames.AddRange( new string[] { "StreamChatSample" } );
	}
}