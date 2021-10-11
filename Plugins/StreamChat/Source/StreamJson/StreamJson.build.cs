using UnrealBuildTool;

public class StreamJson : ModuleRules
{
    public StreamJson(ReadOnlyTargetRules Target) : base(Target)
    {
        PCHUsage = PCHUsageMode.UseExplicitOrSharedPCHs;
		CppStandard = CppStandardVersion.Cpp17;

        PrivateDependencyModuleNames.AddRange(new[] {
            "Core",
            "CoreUObject",
            "Json",
            "JsonUtilities"
        });
    }
}
