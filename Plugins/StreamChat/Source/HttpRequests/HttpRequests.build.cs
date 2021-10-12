using UnrealBuildTool;

public class HttpRequests : ModuleRules
{
    public HttpRequests(ReadOnlyTargetRules Target) : base(Target)
    {
        PCHUsage = PCHUsageMode.UseExplicitOrSharedPCHs;
        CppStandard = CppStandardVersion.Cpp17;

        PublicDependencyModuleNames.AddRange(new[]
        {
			"HTTP",
        });

        PrivateDependencyModuleNames.AddRange(new[] {
			"Core",
            "CoreUObject",
			"StreamJson",
        });
    }
}
