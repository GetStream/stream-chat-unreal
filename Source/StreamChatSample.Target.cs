// Copyright 2026 Stream.IO, Inc. All Rights Reserved.

using UnrealBuildTool;

public class StreamChatSampleTarget : TargetRules
{
	public StreamChatSampleTarget(TargetInfo Target) : base(Target)
	{
		IncludeOrderVersion = EngineIncludeOrderVersion.Latest;

		Type = TargetType.Game;
		DefaultBuildSettings = BuildSettingsVersion.Latest;
		ExtraModuleNames.AddRange( new[] { "StreamChatSample" } );
	}
}
