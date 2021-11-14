using UnrealBuildTool;

public class StreamChatUi : ModuleRules
{
    public StreamChatUi(ReadOnlyTargetRules Target) : base(Target)
    {
        PCHUsage = PCHUsageMode.UseExplicitOrSharedPCHs;
        CppStandard = CppStandardVersion.Cpp17;

        PrivateDependencyModuleNames.AddRange(new[] {
            "Slate",
            "InputCore",
            "ApplicationCore",
            "Http"
        });

        PublicDependencyModuleNames.AddRange(new[] {
            "Core",
            "CoreUObject",
            "Engine",
            "UMG",
            "SlateCore",
            "StreamChat"
        });
    }
}
