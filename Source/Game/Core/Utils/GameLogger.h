#pragma once

#include "CoreMinimal.h"
#include "HAL/PlatformFilemanager.h"
#include "Misc/FileHelper.h"
#include "Misc/DateTime.h"

/**
 * Custom Game Logger that writes to a separate, easily readable file
 * Usage: GAME_LOG(TEXT("Your message here"));
 */
class GAME_API FGameLogger
{
public:
    static void Initialize();
    static void Log(const FString& Message, const FString& Category = TEXT("Game"));
    static void LogError(const FString& Message, const FString& Category = TEXT("Game"));
    static void LogWarning(const FString& Message, const FString& Category = TEXT("Game"));
    static void Shutdown();

private:
    static FString LogFilePath;
    static bool bInitialized;
    
    static void WriteToFile(const FString& FormattedMessage);
    static FString GetTimestamp();
};

// Convenient macros for easy logging
#define GAME_LOG(Message) FGameLogger::Log(Message)
#define GAME_LOG_CATEGORY(Message, Category) FGameLogger::Log(Message, Category)
#define GAME_LOG_ERROR(Message) FGameLogger::LogError(Message)
#define GAME_LOG_WARNING(Message) FGameLogger::LogWarning(Message)
