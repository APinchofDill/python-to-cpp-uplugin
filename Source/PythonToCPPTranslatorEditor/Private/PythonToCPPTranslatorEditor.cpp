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
                    LOCTEXT("ConvertUAssetToCpp_Label", "Convert selected file to c++"),
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
    for (const FAssetData& Asset : SelectedAssets)
    {
        FString AssetName = Asset.AssetName.ToString();
        FString AssetClass = Asset.GetClass()->GetName();
        
        FString FilePath;
        if (!Asset.GetTagValue("FilePath", FilePath))
        {
            // Fallback for physical files
            FString PackagePath = Asset.PackageName.ToString(); 
            FString FullPath = FPackageName::LongPackageNameToFilename(PackagePath, FPackageName::GetAssetPackageExtension());
            FullPath = FPaths::ChangeExtension(FullPath, TEXT("py"));
            FilePath = FullPath;
        }

        FPaths::NormalizeFilename(FilePath);
        FilePath = FPaths::ConvertRelativePathToFull(FilePath);

        if (!FPaths::FileExists(FilePath))
        {
            FMessageDialog::Open(EAppMsgType::Ok, FText::FromString(FString::Printf(TEXT("Python file not found: %s"), *FilePath)));
            continue;
        }

        FString PythonContent;
        if (!FFileHelper::LoadFileToString(PythonContent, *FilePath))
        {
            FMessageDialog::Open(EAppMsgType::Ok, LOCTEXT("PythonLoadFail", "Failed to load the Python file."));
            continue;
        }

        // === Parse Python ===
        FString ClassName = AssetName;
        TArray<FString> Lines;
        PythonContent.ParseIntoArrayLines(Lines);

        FString TickBody;
        FString BeginPlayBody;
        TArray<FString> Properties;

        for (const FString& Line : Lines)
        {
            if (Line.StartsWith("#@actor"))
            {
                ClassName = Line.Replace(TEXT("#@actor "), TEXT("")).TrimStartAndEnd();
            }
            else if (Line.StartsWith("#@property"))
            {
                FString PropertyLine = Line.Replace(TEXT("#@property "), TEXT("")).TrimStartAndEnd();
                Properties.Add(PropertyLine);
            }
            else if (Line.StartsWith("#@function"))
            {
                if (Line.Contains("BeginPlay"))
                {
                    // Next line is assumed to be print or logic
                    int32 Index = Lines.Find(Line);
                    if (Lines.IsValidIndex(Index + 1))
                    {
                        BeginPlayBody = Lines[Index + 1].TrimStartAndEnd();
                    }
                }
            }
            else if (Line.StartsWith("#@tick"))
            {
                int32 Index = Lines.Find(Line);
                if (Lines.IsValidIndex(Index + 1))
                {
                    TickBody = Lines[Index + 1].TrimStartAndEnd();
                }
            }
        }

        // === Generate Header ===
        FString HeaderText = FString::Printf(TEXT(
            "#pragma once\n\n"
            "#include \"CoreMinimal.h\"\n"
            "#include \"GameFramework/Actor.h\"\n"
            "#include \"%s.generated.h\"\n\n"
            "UCLASS()\n"
            "class YOURPROJECT_API A%s : public AActor\n"
            "{\n"
            "    GENERATED_BODY()\n\n"
            "public:\n"
            "    A%s();\n\n"
            "protected:\n"
            "    virtual void BeginPlay() override;\n\n"
            "public:\n"
            "    virtual void Tick(float DeltaTime) override;\n\n"
            "private:\n"
        ), *ClassName, *ClassName, *ClassName);

        for (const FString& Prop : Properties)
        {
            HeaderText += FString::Printf(TEXT("    UPROPERTY(EditAnywhere)\n    %s;\n\n"), *Prop);
        }

        HeaderText += TEXT("    FVector Location;\n};\n");

        // === Generate Source ===
        FString SourceText = FString::Printf(TEXT(
            "#include \"%s.h\"\n\n"
            "A%s::A%s()\n"
            "{\n"
            "    PrimaryActorTick.bCanEverTick = true;\n"
            "}\n\n"
            "void A%s::BeginPlay()\n"
            "{\n"
            "    Super::BeginPlay();\n"
            "    Location = GetActorLocation();\n"
            "    UE_LOG(LogTemp, Log, TEXT(\"%s\"));\n"
            "}\n\n"
            "void A%s::Tick(float DeltaTime)\n"
            "{\n"
            "    Super::Tick(DeltaTime);\n"
            "    %s;\n"
            "}\n"
        ),
            *ClassName, *ClassName, *ClassName,
            *ClassName, *BeginPlayBody,
            *ClassName, *TickBody
        );

        // === Save Files ===
        FString SaveDir = FPaths::ProjectDir() / TEXT("Source/") / FApp::GetProjectName() / TEXT("GeneratedActors/");
        IFileManager::Get().MakeDirectory(*SaveDir, true);

        FString HeaderPath = SaveDir / (ClassName + TEXT(".h"));
        FString SourcePath = SaveDir / (ClassName + TEXT(".cpp"));

        bool bHeaderSaved = FFileHelper::SaveStringToFile(HeaderText, *HeaderPath);
        bool bSourceSaved = FFileHelper::SaveStringToFile(SourceText, *SourcePath);

        if (bHeaderSaved && bSourceSaved)
        {
            FMessageDialog::Open(EAppMsgType::Ok,
                FText::Format(LOCTEXT("ConvertSuccess", "Successfully generated C++ files for: {0}"), FText::FromString(ClassName)));
        }
        else
        {
            FMessageDialog::Open(EAppMsgType::Ok, LOCTEXT("FileSaveFail", "Failed to save one or both generated files."));
        }
    }
}


#undef LOCTEXT_NAMESPACE

IMPLEMENT_MODULE(FPythonToCPPTranslatorEditorModule, PythonToCPPTranslatorEditor)
