#pragma once

#include "CoreMinimal.h"
#include "AssetTypeActions_Base.h"

class FPythonAssetTypeActions : public FAssetTypeActions_Base
{
public:
    virtual FText GetName() const override;
    virtual FColor GetTypeColor() const override;
    virtual UClass* GetSupportedClass() const override;
    virtual uint32 GetCategories() override;
};
