#pragma once

#include "CoreMinimal.h"
#include "Engine/Engine.h"
#include "Engine/World.h"
#include "GameFramework/Actor.h"

/**
 * Static C++ functions that expose Unreal Engine functionality to .NET mods
 * This is the "wrapper" system you mentioned - pure C++ functions that C# scripts can call
 */
class DOTNETSCRIPTING_API FUnrealEngineAPI
{
public:
    // === LOGGING FUNCTIONS ===
    static void LogInfo(const char* Category, const char* Message);
    static void LogWarning(const char* Category, const char* Message);
    static void LogError(const char* Category, const char* Message);

    // === WORLD/ACTOR FUNCTIONS ===
    static void* SpawnActorByName(const char* ActorClassName, float X, float Y, float Z, float Roll, float Pitch, float Yaw);
    static void DestroyActor(void* ActorPtr);
    static bool IsActorValid(void* ActorPtr);
    
    // === TRANSFORM FUNCTIONS ===
    static void SetActorLocation(void* ActorPtr, float X, float Y, float Z);
    static void GetActorLocation(void* ActorPtr, float& OutX, float& OutY, float& OutZ);
    static void SetActorRotation(void* ActorPtr, float Roll, float Pitch, float Yaw);
    static void GetActorRotation(void* ActorPtr, float& OutRoll, float& OutPitch, float& OutYaw);

    // === COMPONENT FUNCTIONS ===
    static void* GetActorComponent(void* ActorPtr, const char* ComponentClassName);
    static void* AddComponentToActor(void* ActorPtr, const char* ComponentClassName);

    // === GAME STATE FUNCTIONS ===
    static void* GetGameMode();
    static void* GetGameState();
    static void* GetPlayerController(int32 PlayerIndex = 0);
    static void* GetPlayerPawn(int32 PlayerIndex = 0);

    // === UTILITY FUNCTIONS ===
    static float GetDeltaTime();
    static float GetGameTime();
    static void QuitGame();
    static void SetGamePaused(bool bPaused);

    // === FILE I/O FUNCTIONS ===
    static bool SaveStringToFile(const char* FilePath, const char* Content);
    static bool LoadStringFromFile(const char* FilePath, char* OutBuffer, int32 BufferSize);

    // === MATH HELPER FUNCTIONS ===
    static float GetDistanceBetweenPoints(float X1, float Y1, float Z1, float X2, float Y2, float Z2);
    static void GetRandomPointInSphere(float CenterX, float CenterY, float CenterZ, float Radius, float& OutX, float& OutY, float& OutZ);

    // === PED FACTORY API ===
    static void* GetPedFactory();
    static void* SpawnPed(const char* CharacterName, const char* VariationName, float X, float Y, float Z, float Roll, float Pitch, float Yaw, bool bAIEnabled, bool bPlayerControlled);
    static bool PossessPed(void* PedPtr, void* PlayerControllerPtr);
    static bool UnpossessPed(void* PlayerControllerPtr);
    static void SetPedAIEnabled(void* PedPtr, bool bEnabled);
    static void* FindPedByName(const char* PedName);

    // === TASK SYSTEM API ===
    static int32 CreateOneShotTask(const char* TaskName, int32 Priority);
    static int32 CreateComplexTask(const char* TaskName, int32 Priority, const char** SubTaskNames, int32 SubTaskCount);
    static int32 CreateWildComplexTask(const char* TaskName, int32 Priority, bool bAdaptive);
    
    static bool AssignTaskToPed(void* PedPtr, int32 TaskHandle);
    static bool RemoveTaskFromPed(void* PedPtr, int32 TaskHandle);
    static void ClearAllTasksFromPed(void* PedPtr);
    static bool InterruptCurrentTask(void* PedPtr);
    
    static int32 GetTaskState(int32 TaskHandle);
    static bool IsTaskRunning(int32 TaskHandle);
    static bool IsTaskCompleted(int32 TaskHandle);
    static const char* GetTaskName(int32 TaskHandle);
    static int32 GetTaskPriority(int32 TaskHandle);
    
    static int32 GetActiveTaskCount(void* PedPtr);
    static int32 GetCurrentTask(void* PedPtr);
    static void GetAllActiveTasks(void* PedPtr, int32* OutTaskHandles, int32& OutCount);
    static void* GetTaskManager(void* PedPtr);

    // === PED CORE API ===
    static const char* GetPedCharacterName(void* PedPtr);
    static bool IsPedPlayerControlled(void* PedPtr);
    static bool IsPedAIEnabled(void* PedPtr);
    static void SetPedPlayerControlled(void* PedPtr, bool bPlayerControlled);
    static void GetAllPedsInWorld(void** OutPedPtrs, int32& OutCount);
    static float GetDistanceBetweenPeds(void* Ped1Ptr, void* Ped2Ptr);

private:
    // Helper to get current world context
    static UWorld* GetCurrentWorld();
    
    // Helper to find actor class by name
    static UClass* FindActorClass(const FString& ClassName);
    
    // Helper to convert void* back to AActor*
    static AActor* GetActorFromPtr(void* ActorPtr);
};
