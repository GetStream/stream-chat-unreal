using UnrealBuildTool;

public class StreamChatEditor : ModuleRules
{
    public StreamChatEditor(ReadOnlyTargetRules Target) : base(Target)
    {
        PCHUsage = PCHUsageMode.UseExplicitOrSharedPCHs;
		CppStandard = CppStandardVersion.Cpp17;

        PrivateDependencyModuleNames.AddRange(new[]
        {
            "Core",
            "CoreUObject",
            "Engine",
            "Settings",
            "StreamChat",
        });
    }
}
