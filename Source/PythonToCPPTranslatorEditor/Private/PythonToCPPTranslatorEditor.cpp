#include "PythonToCPPTranslatorEditor.h"
#include "ContentBrowserModule.h"
#include "Misc/Paths.h"
#include "HAL/PlatformProcess.h"
#include "PythonAssetTypeActions.h"
#include "AssetToolsModule.h"
#include "IAssetTools.h"
#include "Modules/ModuleManager.h"

#define LOCTEXT_NAMESPACE "FPythonToCPPTranslatorEditorModule"

void FPythonToCPPTranslatorEditorModule::StartupModule()
{
    FContentBrowserModule& ContentBrowserModule = FModuleManager::LoadModuleChecked<FContentBrowserModule>("ContentBrowser");
    
    // Load the Asset Tools module
    FAssetToolsModule& AssetToolsModule = FModuleManager::LoadModuleChecked<FAssetToolsModule>("AssetTools");

    // Register Python asset type actions
    PythonAssetTypeActions = MakeShareable(new FPythonAssetTypeActions());
    AssetToolsModule.Get().RegisterAssetTypeActions(PythonAssetTypeActions.ToSharedRef());
    
    // load context menu extender
    RegisterAssetContextMenu();

    UE_LOG(LogTemp, Warning, TEXT("PythonToCPPTranslator Editor module loaded."));
}

void FPythonToCPPTranslatorEditorModule::ShutdownModule()
{
    // Perform cleanup if needed
    // Unregister Python asset type actions
    if (FModuleManager::Get().IsModuleLoaded("AssetTools"))
    {
        FAssetToolsModule& AssetToolsModule = FModuleManager::GetModuleChecked<FAssetToolsModule>("AssetTools");
        AssetToolsModule.Get().UnregisterAssetTypeActions(PythonAssetTypeActions.ToSharedRef());
    }
    
    // Unload context menu extender
    UnregisterAssetContextMenu();

    PythonAssetTypeActions.Reset();
}

void FPythonToCPPTranslatorEditorModule::RegisterAssetContextMenu()
{
    FContentBrowserModule& ContentBrowserModule = FModuleManager::LoadModuleChecked<FContentBrowserModule>(TEXT("ContentBrowser"));
    TArray<FContentBrowserMenuExtender_SelectedAssets>& MenuExtenders = ContentBrowserModule.GetAllAssetViewContextMenuExtenders();
    
    MenuExtenders.Add(FContentBrowserMenuExtender_SelectedAssets::CreateRaw(
        this, &FPythonToCPPTranslatorEditorModule::OnExtendContentBrowserAssetSelectionMenu));
}

void FPythonToCPPTranslatorEditorModule::UnregisterAssetContextMenu()
{
    if (FModuleManager::Get().IsModuleLoaded("ContentBrowser"))
    {
        FContentBrowserModule& ContentBrowserModule = FModuleManager::GetModuleChecked<FContentBrowserModule>("ContentBrowser");
        TArray<FContentBrowserMenuExtender_SelectedAssets>& MenuExtenders = ContentBrowserModule.GetAllAssetViewContextMenuExtenders();
        MenuExtenders.RemoveAll([this](const FContentBrowserMenuExtender_SelectedAssets& Extender)
        {
            return Extender.GetHandle().IsValid();
        });
    }
}

TSharedRef<FExtender> FPythonToCPPTranslatorEditorModule::OnExtendContentBrowserAssetSelectionMenu(const TArray<FAssetData>& SelectedAssets)
{
    TSharedRef<FExtender> Extender(new FExtender());

    if (SelectedAssets.Num() > 0)
    {
        TArray<FAssetData> Assets = SelectedAssets;
        
        Extender->AddMenuExtension(
            "AssetContextAdvancedActions",
            EExtensionHook::After,
            nullptr,
            FMenuExtensionDelegate::CreateLambda([this, Assets](FMenuBuilder& MenuBuilder)
            {
                const FUIAction MenuAction(FExecuteAction::CreateLambda([this, Assets]()
                {
                    this->OnConvertUAssetToCpp(Assets);
                }));

                MenuBuilder.AddMenuEntry(
                    LOCTEXT("ConvertUAssetToCpp_Label", "Sample - Content Menu Item"),
                    LOCTEXT("ConvertUAssetToCpp_Tooltip", "Converts the selected UAsset to a C++ class"),
                    FSlateIcon(),
                    MenuAction
                );
            })
        );
    }

    return Extender;
}

void FPythonToCPPTranslatorEditorModule::OnConvertUAssetToCpp(TArray<FAssetData> SelectedAssets)
{
    // Example implementation - you would expand this based on your needs
    for (const FAssetData& Asset : SelectedAssets)
    {
        FString AssetName = Asset.AssetName.ToString();
        FString AssetClass = Asset.GetClass()->GetName();
        
        // Create the C++ class name
        FString ClassName = FString::Printf(TEXT("%s_Cpp"), *AssetName);
        
        // Here you would implement the actual conversion logic
        // This could include:
        // 1. Creating a new C++ class
        // 2. Copying relevant properties from the asset
        // 3. Setting up inheritance
        // 4. Generating necessary code
        
        FMessageDialog::Open(EAppMsgType::Ok, 
            FText::Format(LOCTEXT("ConvertSuccess", "Converting {0} of type {1} to C++"), 
                FText::FromString(AssetName), 
                FText::FromString(AssetClass)));
    }
}

#undef LOCTEXT_NAMESPACE

IMPLEMENT_MODULE(FPythonToCPPTranslatorEditorModule, PythonToCPPTranslatorEditor)
