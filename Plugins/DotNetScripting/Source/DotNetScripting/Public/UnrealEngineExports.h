#pragma once

// C-style exports for .NET interop
// These functions are the "wrapper" that exposes UE functionality to C# mods

extern "C" {
    // === LOGGING EXPORTS ===
    DOTNETSCRIPTING_API void LogInfoNative(const char* Category, const char* Message);
    DOTNETSCRIPTING_API void LogWarningNative(const char* Category, const char* Message);
    DOTNETSCRIPTING_API void LogErrorNative(const char* Category, const char* Message);

    // === ACTOR EXPORTS ===
    DOTNETSCRIPTING_API void* SpawnActorByName(const char* ActorClassName, float X, float Y, float Z, float Roll, float Pitch, float Yaw);
    DOTNETSCRIPTING_API void DestroyActor(void* ActorPtr);
    DOTNETSCRIPTING_API bool IsActorValid(void* ActorPtr);

    // === TRANSFORM EXPORTS ===
    DOTNETSCRIPTING_API void SetActorLocation(void* ActorPtr, float X, float Y, float Z);
    DOTNETSCRIPTING_API void GetActorLocation(void* ActorPtr, float* OutX, float* OutY, float* OutZ);
    DOTNETSCRIPTING_API void SetActorRotation(void* ActorPtr, float Roll, float Pitch, float Yaw);
    DOTNETSCRIPTING_API void GetActorRotation(void* ActorPtr, float* OutRoll, float* OutPitch, float* OutYaw);

    // === GAME STATE EXPORTS ===
    DOTNETSCRIPTING_API void* GetGameMode();
    DOTNETSCRIPTING_API void* GetGameState();
    DOTNETSCRIPTING_API void* GetPlayerController(int32 PlayerIndex);
    DOTNETSCRIPTING_API void* GetPlayerPawn(int32 PlayerIndex);

    // === UTILITY EXPORTS ===
    DOTNETSCRIPTING_API float GetDeltaTime();
    DOTNETSCRIPTING_API float GetGameTime();
    DOTNETSCRIPTING_API void SetGamePaused(bool bPaused);
    DOTNETSCRIPTING_API float GetDistanceBetweenPoints(float X1, float Y1, float Z1, float X2, float Y2, float Z2);

    // === PED FACTORY EXPORTS ===
    DOTNETSCRIPTING_API void* GetPedFactory();
    DOTNETSCRIPTING_API void* SpawnPed(const char* CharacterName, const char* VariationName, float X, float Y, float Z, float Roll, float Pitch, float Yaw, bool bAIEnabled, bool bPlayerControlled);
    DOTNETSCRIPTING_API bool PossessPed(void* PedPtr, void* PlayerControllerPtr);
    DOTNETSCRIPTING_API bool UnpossessPed(void* PlayerControllerPtr);
    DOTNETSCRIPTING_API void SetPedAIEnabled(void* PedPtr, bool bEnabled);
    DOTNETSCRIPTING_API void* FindPedByName(const char* PedName);

    // === TASK SYSTEM EXPORTS ===
    DOTNETSCRIPTING_API int CreateOneShotTask(const char* TaskName, int Priority);
    DOTNETSCRIPTING_API int CreateComplexTask(const char* TaskName, int Priority, const char** SubTaskNames, int SubTaskCount);
    DOTNETSCRIPTING_API int CreateWildComplexTask(const char* TaskName, int Priority, bool bAdaptive);
    
    DOTNETSCRIPTING_API bool AssignTaskToPed(void* PedPtr, int TaskHandle);
    DOTNETSCRIPTING_API bool RemoveTaskFromPed(void* PedPtr, int TaskHandle);
    DOTNETSCRIPTING_API void ClearAllTasksFromPed(void* PedPtr);
    DOTNETSCRIPTING_API bool InterruptCurrentTask(void* PedPtr);
    
    DOTNETSCRIPTING_API int GetTaskState(int TaskHandle);
    DOTNETSCRIPTING_API bool IsTaskRunning(int TaskHandle);
    DOTNETSCRIPTING_API bool IsTaskCompleted(int TaskHandle);
    DOTNETSCRIPTING_API const char* GetTaskName(int TaskHandle);
    DOTNETSCRIPTING_API int GetTaskPriority(int TaskHandle);
    
    DOTNETSCRIPTING_API int GetActiveTaskCount(void* PedPtr);
    DOTNETSCRIPTING_API int GetCurrentTask(void* PedPtr);
    DOTNETSCRIPTING_API void GetAllActiveTasks(void* PedPtr, int* OutTaskHandles, int* OutCount);
    DOTNETSCRIPTING_API void* GetTaskManager(void* PedPtr);

    // === PED CORE EXPORTS ===
    DOTNETSCRIPTING_API const char* GetPedCharacterName(void* PedPtr);
    DOTNETSCRIPTING_API bool IsPedPlayerControlled(void* PedPtr);
    DOTNETSCRIPTING_API bool IsPedAIEnabled(void* PedPtr);
    DOTNETSCRIPTING_API void SetPedPlayerControlled(void* PedPtr, bool bPlayerControlled);
    DOTNETSCRIPTING_API void GetAllPedsInWorld(void** OutPedPtrs, int* OutCount);
    DOTNETSCRIPTING_API float GetDistanceBetweenPeds(void* Ped1Ptr, void* Ped2Ptr);
}
