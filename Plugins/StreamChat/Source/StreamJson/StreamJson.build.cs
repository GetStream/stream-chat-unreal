using UnrealBuildTool;

public class StreamJson : ModuleRules
{
    public StreamJson(ReadOnlyTargetRules Target) : base(Target)
    {
        PCHUsage = PCHUsageMode.UseExplicitOrSharedPCHs;

        PublicDependencyModuleNames.AddRange(new string[] {
            "Core",
            "CoreUObject",
            "Json",
            "JsonUtilities"
        });
    }
}
