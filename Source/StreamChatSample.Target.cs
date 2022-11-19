// Copyright 2022 Stream.IO, Inc. All Rights Reserved.

using UnrealBuildTool;

public class StreamChatSampleTarget : TargetRules
{
	public StreamChatSampleTarget(TargetInfo Target) : base(Target)
	{
#if UE_5_1_OR_LATER
		IncludeOrderVersion = EngineIncludeOrderVersion.Latest;
#endif

		Type = TargetType.Game;
		DefaultBuildSettings = BuildSettingsVersion.V2;
		ExtraModuleNames.AddRange( new[] { "StreamChatSample" } );
	}
}
