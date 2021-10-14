using UnrealBuildTool;

public class StreamChatDto : ModuleRules
{
    public StreamChatDto(ReadOnlyTargetRules Target) : base(Target)
    {
        PCHUsage = PCHUsageMode.UseExplicitOrSharedPCHs;
        CppStandard = CppStandardVersion.Cpp17;

        PrivateDependencyModuleNames.AddRange(new[] {
            "CoreUObject",
        });

        PublicDependencyModuleNames.AddRange(new[] {
            "Core",
            "JsonUtilities",
        });
    }
}
