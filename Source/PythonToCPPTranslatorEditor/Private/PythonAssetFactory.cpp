#include "PythonAssetFactory.h"
#include "PythonScript.h"
#include "Misc/FileHelper.h"

UPythonAssetFactory::UPythonAssetFactory()
{
    SupportedClass = UPythonScript::StaticClass();
    bEditorImport = true;

    Formats.Add(TEXT("py;Python Script"));
}

UObject* UPythonAssetFactory::FactoryCreateFile(UClass* InClass, UObject* InParent, FName InName, EObjectFlags Flags,
                                                const FString& Filename, const TCHAR* Parms, FFeedbackContext* Warn,
                                                bool& bOutOperationCanceled)
{
    // Load the Python file content
    FString FileContent;
    if (!FFileHelper::LoadFileToString(FileContent, *Filename))
    {
        Warn->Logf(ELogVerbosity::Error, TEXT("Failed to load Python file: %s"), *Filename);
        bOutOperationCanceled = true;
        return nullptr;
    }

    // Create the PythonScript object
    UPythonScript* PythonScript = NewObject<UPythonScript>(InParent, InClass, InName, Flags);
    if (PythonScript)
    {
        PythonScript->ScriptContent = FileContent;
    }

    return PythonScript;
}
