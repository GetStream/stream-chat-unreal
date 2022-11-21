// Copyright 2022 Stream.IO, Inc. All Rights Reserved.

using UnrealBuildTool;

public class StreamChatSampleEditorTarget : TargetRules
{
	public StreamChatSampleEditorTarget(TargetInfo Target) : base(Target)
	{
#if UE_4_27_OR_EARLIER
		bOverrideBuildEnvironment = true;
		AdditionalCompilerArguments = "-Wno-unused-but-set-variable";
#elif UE_5_1_OR_LATER
		IncludeOrderVersion = EngineIncludeOrderVersion.Latest;
#endif

		Type = TargetType.Editor;
		DefaultBuildSettings = BuildSettingsVersion.V2;
		ExtraModuleNames.AddRange( new[] { "StreamChatSample", "DocsSamples" } );
	}
}
