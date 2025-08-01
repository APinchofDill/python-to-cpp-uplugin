using UnrealBuildTool;

public class PythonToCPPTranslatorEditor : ModuleRules
{
    public PythonToCPPTranslatorEditor(ReadOnlyTargetRules Target) : base(Target)
    {
        PCHUsage = PCHUsageMode.UseExplicitOrSharedPCHs;

        PublicDependencyModuleNames.AddRange(new string[]
        {
                "Core",
                "CoreUObject",
                "Engine",
                "InputCore"
        });

        PrivateDependencyModuleNames.AddRange(new string[]
        {
            "UnrealEd",
            "ContentBrowser",
            "EditorStyle",
            "Projects",
            "AssetRegistry",
            "Slate",
            "SlateCore",
            "EditorSubsystem",
            "AssetTools",
            "UMG",
            "EditorStyle"
        });
    }
}