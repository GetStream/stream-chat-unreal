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
                // TODO These are all needed due to the reliance on templates for events
                "Json",
                "JsonUtilities",
                "StreamJson",
                "StreamChatApi",
                "StreamChatWebSocket",
                "StreamChatDto",
            }
        );
    }
}
