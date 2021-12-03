using System;
using System.IO;
using UnrealBuildTool;

public class Jwt : ModuleRules
{
    public Jwt(ReadOnlyTargetRules Target) : base(Target)
    {
        PCHUsage = PCHUsageMode.UseExplicitOrSharedPCHs;
        CppStandard = CppStandardVersion.Cpp17;
        bEnableExceptions = true;

		var ThirdPartyPath = Path.GetFullPath(Path.Combine(ModuleDirectory, "ThirdParty"));
        PrivateIncludePaths.Add(Path.Combine(ThirdPartyPath, "jwt-cpp-0.5.1", "include"));

        PublicDependencyModuleNames.AddRange(new[] {
            "CoreUObject",
            "Engine",
        });

        PrivateDependencyModuleNames.AddRange(new[]
        {
            "Core",
            "OpenSSL"
        });
    }
}
