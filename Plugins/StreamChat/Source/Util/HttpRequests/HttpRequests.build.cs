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
			"StreamJson",
        });

        PrivateDependencyModuleNames.AddRange(new[] {
			"Core",
            "CoreUObject",
        });
    }
}