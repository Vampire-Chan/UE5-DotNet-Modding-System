// Copyright Epic Games, Inc. All Rights Reserved.

#include "Game.h"
#include "Modules/ModuleManager.h"
// #include "Test/PedTestConsoleCommands.h" // Removed - file doesn't exist

class FGameModule : public FDefaultGameModuleImpl
{
public:
    virtual void StartupModule() override
    {
        FDefaultGameModuleImpl::StartupModule();
        
        // Register console commands
        // FPedTestConsoleCommands::RegisterCommands(); // Removed - class doesn't exist
        
        UE_LOG(LogTemp, Log, TEXT("Game Module: Started"));
    }

    virtual void ShutdownModule() override
    {
        // Unregister console commands
        // FPedTestConsoleCommands::UnregisterCommands(); // Removed - class doesn't exist
        
        FDefaultGameModuleImpl::ShutdownModule();
        
        UE_LOG(LogTemp, Log, TEXT("Game Module: Shutdown"));
    }
};

IMPLEMENT_PRIMARY_GAME_MODULE(FGameModule, Game, "Game");