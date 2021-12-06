// Copyright 2021 Stream.IO, Inc. All Rights Reserved.

// Copyright Stream.IO, Inc. All Rights Reserved.

using UnrealBuildTool;
using System.Collections.Generic;

public class StreamChatSampleTarget : TargetRules
{
	public StreamChatSampleTarget( TargetInfo Target) : base(Target)
	{
		Type = TargetType.Game;
		DefaultBuildSettings = BuildSettingsVersion.V2;
		ExtraModuleNames.AddRange( new string[] { "StreamChatSample" } );
	}
}
