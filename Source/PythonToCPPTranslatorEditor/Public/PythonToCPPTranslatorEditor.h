#pragma once

#include "CoreMinimal.h"
#include "Modules/ModuleManager.h"
#include "ContentBrowserDelegates.h"
#include "Framework/MultiBox/MultiBoxBuilder.h"

class FPythonToCPPTranslatorEditorModule : public IModuleInterface
{
public:
    virtual void StartupModule() override;
    virtual void ShutdownModule() override;

private:
    void RegisterAssetContextMenu();
    void UnregisterAssetContextMenu();
    void ExecutePythonToCPP();
    void OnConvertUAssetToCpp(TArray<FAssetData> SelectedAssets);
    TSharedRef<FExtender> OnExtendContentBrowserAssetSelectionMenu(const TArray<FAssetData>& SelectedAssets);
    TSharedPtr<IAssetTypeActions> PythonAssetTypeActions;
};
