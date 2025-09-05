#pragma once

#include "CoreMinimal.h"
#include "Modules/ModuleManager.h"

/**
 * DotNet Scripting Module
 * Main module for .NET integration with Unreal Engine
 */
class FDotNetScriptingModule : public IModuleInterface
{
public:
    /** IModuleInterface implementation */
    virtual void StartupModule() override;
    virtual void ShutdownModule() override;

    /** Module instance access */
    static FDotNetScriptingModule& Get()
    {
        return FModuleManager::LoadModuleChecked<FDotNetScriptingModule>("DotNetScripting");
    }

    /** Check if module is loaded */
    static bool IsAvailable()
    {
        return FModuleManager::Get().IsModuleLoaded("DotNetScripting");
    }

private:
    bool bModuleInitialized = false;
};