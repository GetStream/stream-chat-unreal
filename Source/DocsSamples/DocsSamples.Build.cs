using UnrealBuildTool;

public class DocsSamples : ModuleRules
{
    public DocsSamples(ReadOnlyTargetRules Target) : base(Target)
    {
        PCHUsage = ModuleRules.PCHUsageMode.UseExplicitOrSharedPCHs;

        PublicDependencyModuleNames.AddRange(
            new string[]
            {
                "Core",
            }
        );

        PrivateDependencyModuleNames.AddRange(
            new string[]
            {
                "CoreUObject",
                "Engine",
                "Slate",
                "SlateCore",
                "StreamChat",
                "Json",
                "JsonUtilities",
                "StreamJson",
                "StreamChatApi",
                "StreamChatWebSocket",
            }
        );
    }
}
