#include "GameLogger.h"
#include "Engine/Engine.h"

FString FGameLogger::LogFilePath = TEXT("");
bool FGameLogger::bInitialized = false;

void FGameLogger::Initialize()
{
    if (bInitialized) return;
    
    // Create log file in project's Saved/Logs directory
    FString ProjectDir = FPaths::ProjectDir();
    FString LogsDir = FPaths::Combine(ProjectDir, TEXT("Saved"), TEXT("Logs"));
    
    // Ensure logs directory exists
    IPlatformFile& PlatformFile = FPlatformFileManager::Get().GetPlatformFile();
    if (!PlatformFile.DirectoryExists(*LogsDir))
    {
        PlatformFile.CreateDirectoryTree(*LogsDir);
    }
    
    // Create timestamped log file
    FString Timestamp = FDateTime::Now().ToString(TEXT("%Y%m%d_%H%M%S"));
    LogFilePath = FPaths::Combine(LogsDir, FString::Printf(TEXT("GameDebug_%s.log"), *Timestamp));
    
    bInitialized = true;
    
    // Write header
    FString Header = FString::Printf(TEXT("=== GAME DEBUG LOG STARTED ===\nTime: %s\nProject: %s\n\n"), 
                                   *FDateTime::Now().ToString(), 
                                   *FPaths::GetProjectFilePath());
    WriteToFile(Header);
    
    UE_LOG(LogTemp, Log, TEXT("GameLogger: Custom log file created at %s"), *LogFilePath);
}

void FGameLogger::Log(const FString& Message, const FString& Category)
{
    if (!bInitialized) Initialize();
    
    FString FormattedMessage = FString::Printf(TEXT("[%s] [%s] %s"), 
                                             *GetTimestamp(), 
                                             *Category, 
                                             *Message);
    WriteToFile(FormattedMessage);
    
    // Also log to UE5 console for immediate visibility
    UE_LOG(LogTemp, Log, TEXT("%s: %s"), *Category, *Message);
}

void FGameLogger::LogError(const FString& Message, const FString& Category)
{
    if (!bInitialized) Initialize();
    
    FString FormattedMessage = FString::Printf(TEXT("[%s] [ERROR] [%s] %s"), 
                                             *GetTimestamp(), 
                                             *Category, 
                                             *Message);
    WriteToFile(FormattedMessage);
    
    // Also log to UE5 console
    UE_LOG(LogTemp, Error, TEXT("%s: %s"), *Category, *Message);
}

void FGameLogger::LogWarning(const FString& Message, const FString& Category)
{
    if (!bInitialized) Initialize();
    
    FString FormattedMessage = FString::Printf(TEXT("[%s] [WARNING] [%s] %s"), 
                                             *GetTimestamp(), 
                                             *Category, 
                                             *Message);
    WriteToFile(FormattedMessage);
    
    // Also log to UE5 console
    UE_LOG(LogTemp, Warning, TEXT("%s: %s"), *Category, *Message);
}

void FGameLogger::WriteToFile(const FString& FormattedMessage)
{
    if (LogFilePath.IsEmpty()) return;
    
    FString MessageWithNewline = FormattedMessage + TEXT("\n");
    FFileHelper::SaveStringToFile(MessageWithNewline, *LogFilePath, 
                                 FFileHelper::EEncodingOptions::AutoDetect, 
                                 &IFileManager::Get(), 
                                 FILEWRITE_Append);
}

FString FGameLogger::GetTimestamp()
{
    return FDateTime::Now().ToString(TEXT("%H:%M:%S.%f"));
}

void FGameLogger::Shutdown()
{
    if (!bInitialized) return;
    
    FString Footer = FString::Printf(TEXT("\n=== GAME DEBUG LOG ENDED ===\nTime: %s\n"), 
                                   *FDateTime::Now().ToString());
    WriteToFile(Footer);
    
    bInitialized = false;
}
