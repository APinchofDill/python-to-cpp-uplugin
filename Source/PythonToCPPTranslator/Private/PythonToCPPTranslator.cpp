#include "PythonToCPPTranslator.h"

#define LOCTEXT_NAMESPACE "FPythonToCPPTranslatorModule"

void FPythonToCPPTranslatorModule::StartupModule()
{
    UE_LOG(LogTemp, Warning, TEXT("PythonToCPPTranslator runtime module loaded."));
}

void FPythonToCPPTranslatorModule::ShutdownModule()
{
    UE_LOG(LogTemp, Warning, TEXT("PythonToCPPTranslator runtime module unloaded."));
}

#undef LOCTEXT_NAMESPACE

IMPLEMENT_MODULE(FPythonToCPPTranslatorModule, PythonToCPPTranslator)
