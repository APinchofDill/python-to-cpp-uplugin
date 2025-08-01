#include "PythonAssetFactory.h"
#include "PythonScript.h"
#include "Misc/FileHelper.h"

UPythonAssetFactory::UPythonAssetFactory()
{
    bCreateNew = false;
    bEditorImport = true;
    SupportedClass = UPythonScript::StaticClass();
    Formats.Add(TEXT("py;Python Script"));
}

UObject* UPythonAssetFactory::FactoryCreateFile(UClass* InClass, UObject* InParent, FName InName,
    EObjectFlags Flags, const FString& Filename, const TCHAR* Parms,
    FFeedbackContext* Warn, bool& bOutOperationCanceled)
{
    FString FileContents;
    if (!FFileHelper::LoadFileToString(FileContents, *Filename))
    {
        UE_LOG(LogTemp, Error, TEXT("Failed to read file: %s"), *Filename);
        return nullptr;
    }

    UPythonScript* NewScript = NewObject<UPythonScript>(InParent, InClass, InName, Flags);
    NewScript->ScriptContent = FileContents;
    return NewScript;
}
