#include "PythonAssetTypeActions.h"

FText FPythonAssetTypeActions::GetName() const
{
    return FText::FromString(TEXT("Python Script"));
}

FColor FPythonAssetTypeActions::GetTypeColor() const
{
    return FColor::Purple;
}

UClass* FPythonAssetTypeActions::GetSupportedClass() const
{
    return UObject::StaticClass();
}

uint32 FPythonAssetTypeActions::GetCategories()
{
    return EAssetTypeCategories::Misc;
}
