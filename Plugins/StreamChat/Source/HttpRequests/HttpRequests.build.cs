using UnrealBuildTool;

public class HttpRequests : ModuleRules
{
    public HttpRequests(ReadOnlyTargetRules Target) : base(Target)
    {
        PCHUsage = PCHUsageMode.UseExplicitOrSharedPCHs;
        CppStandard = CppStandardVersion.Cpp17;

		PublicDependencyModuleNames.AddRange(new[]
		{
			"Core",
			"StreamJson",
		});

        PrivateDependencyModuleNames.AddRange(new[] {
            "CoreUObject",
			"HTTP",
			"Json",
			"JsonUtilities"
        });
    }
}
