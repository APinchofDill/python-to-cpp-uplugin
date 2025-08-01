#pragma once

#include "CoreMinimal.h"
#include "Factories/Factory.h"
#include "PythonAssetFactory.generated.h"

UCLASS()
class PYTHONTOCPPTRANSLATOREDITOR_API UPythonAssetFactory : public UFactory
{
    GENERATED_BODY()

public:
    UPythonAssetFactory();

    virtual UObject* FactoryCreateFile(UClass* InClass, UObject* InParent, FName InName, EObjectFlags Flags,
                                       const FString& Filename, const TCHAR* Parms, FFeedbackContext* Warn,
                                       bool& bOutOperationCanceled) override;
};
