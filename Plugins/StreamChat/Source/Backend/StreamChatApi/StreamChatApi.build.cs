using UnrealBuildTool;

public class StreamChatApi : ModuleRules
{
    public StreamChatApi(ReadOnlyTargetRules Target) : base(Target)
    {
        PCHUsage = PCHUsageMode.UseExplicitOrSharedPCHs;
        CppStandard = CppStandardVersion.Cpp17;

        PrivateDependencyModuleNames.AddRange(new[]
        {
            "CoreUObject",
            "HttpRequests",
            "StreamJson",
            "TokenProvider"
        });

        PublicDependencyModuleNames.AddRange(new[]
        {
            "Core",
            "JsonUtilities",
            "StreamChatDto",
        });
    }
}