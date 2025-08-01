#pragma once

#include "CoreMinimal.h"
#include "UObject/Object.h"
#include "PythonScript.generated.h"

UCLASS(BlueprintType)
class PYTHONTOCPPTRANSLATOREDITOR_API UPythonScript : public UObject
{
    GENERATED_BODY()

public:
    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Python")
    FString ScriptContent;
};
