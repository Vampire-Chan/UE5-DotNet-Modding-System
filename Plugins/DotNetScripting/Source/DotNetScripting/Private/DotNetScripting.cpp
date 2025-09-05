
#include "DotNetScripting.h"
#include "DotNetHostManager.h"

#define LOCTEXT_NAMESPACE "FDotNetScriptingModule"

void FDotNetScriptingModule::StartupModule()
{
    UE_LOG(LogTemp, Log, TEXT("DotNetScripting: Module startup"));
    
    // Register our subsystem
    // The UDotNetHostManager will be automatically created as a GameInstance subsystem
    
    bModuleInitialized = true;
    UE_LOG(LogTemp, Log, TEXT("DotNetScripting: Module initialized successfully"));
}

void FDotNetScriptingModule::ShutdownModule()
{
    UE_LOG(LogTemp, Log, TEXT("DotNetScripting: Module shutdown"));
    
    // Cleanup will be handled by the subsystem itself
    
    bModuleInitialized = false;
    UE_LOG(LogTemp, Log, TEXT("DotNetScripting: Module shutdown complete"));
}

#undef LOCTEXT_NAMESPACE
    
IMPLEMENT_MODULE(FDotNetScriptingModule, DotNetScripting)
