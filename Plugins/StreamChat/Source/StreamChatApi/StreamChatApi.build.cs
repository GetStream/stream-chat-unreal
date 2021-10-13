using UnrealBuildTool;

public class StreamChatApi : ModuleRules
{
    public StreamChatApi(ReadOnlyTargetRules Target) : base(Target)
    {
        PCHUsage = PCHUsageMode.UseExplicitOrSharedPCHs;
        CppStandard = CppStandardVersion.Cpp17;

        PrivateDependencyModuleNames.AddRange(new[]
        {
            "Core",
            "CoreUObject",
            "HttpRequests",
            "StreamJson",
        });
    }
}
