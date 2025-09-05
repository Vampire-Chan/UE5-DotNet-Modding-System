#include "UnrealEngineExports.h"
#include "UnrealEngineAPI.h"

// C-style wrapper functions that call the C++ API
// These are exported for .NET P/Invoke calls

extern "C" {
    DOTNETSCRIPTING_API void LogInfoNative(const char* Category, const char* Message)
    {
        FUnrealEngineAPI::LogInfo(Category, Message);
    }

    DOTNETSCRIPTING_API void LogWarningNative(const char* Category, const char* Message)
    {
        FUnrealEngineAPI::LogWarning(Category, Message);
    }

    DOTNETSCRIPTING_API void LogErrorNative(const char* Category, const char* Message)
    {
        FUnrealEngineAPI::LogError(Category, Message);
    }

    DOTNETSCRIPTING_API void* SpawnActorByName(const char* ActorClassName, float X, float Y, float Z, float Roll, float Pitch, float Yaw)
    {
        return FUnrealEngineAPI::SpawnActorByName(ActorClassName, X, Y, Z, Roll, Pitch, Yaw);
    }

    DOTNETSCRIPTING_API void DestroyActor(void* ActorPtr)
    {
        FUnrealEngineAPI::DestroyActor(ActorPtr);
    }

    DOTNETSCRIPTING_API bool IsActorValid(void* ActorPtr)
    {
        return FUnrealEngineAPI::IsActorValid(ActorPtr);
    }

    DOTNETSCRIPTING_API void SetActorLocation(void* ActorPtr, float X, float Y, float Z)
    {
        FUnrealEngineAPI::SetActorLocation(ActorPtr, X, Y, Z);
    }

    DOTNETSCRIPTING_API void GetActorLocation(void* ActorPtr, float* OutX, float* OutY, float* OutZ)
    {
        if (OutX && OutY && OutZ)
        {
            FUnrealEngineAPI::GetActorLocation(ActorPtr, *OutX, *OutY, *OutZ);
        }
    }

    DOTNETSCRIPTING_API void SetActorRotation(void* ActorPtr, float Roll, float Pitch, float Yaw)
    {
        FUnrealEngineAPI::SetActorRotation(ActorPtr, Roll, Pitch, Yaw);
    }

    DOTNETSCRIPTING_API void GetActorRotation(void* ActorPtr, float* OutRoll, float* OutPitch, float* OutYaw)
    {
        if (OutRoll && OutPitch && OutYaw)
        {
            FUnrealEngineAPI::GetActorRotation(ActorPtr, *OutRoll, *OutPitch, *OutYaw);
        }
    }

    DOTNETSCRIPTING_API void* GetGameMode()
    {
        return FUnrealEngineAPI::GetGameMode();
    }

    DOTNETSCRIPTING_API void* GetGameState()
    {
        return FUnrealEngineAPI::GetGameState();
    }

    DOTNETSCRIPTING_API void* GetPlayerController(int32 PlayerIndex)
    {
        return FUnrealEngineAPI::GetPlayerController(PlayerIndex);
    }

    DOTNETSCRIPTING_API void* GetPlayerPawn(int32 PlayerIndex)
    {
        return FUnrealEngineAPI::GetPlayerPawn(PlayerIndex);
    }

    DOTNETSCRIPTING_API float GetDeltaTime()
    {
        return FUnrealEngineAPI::GetDeltaTime();
    }

    DOTNETSCRIPTING_API float GetGameTime()
    {
        return FUnrealEngineAPI::GetGameTime();
    }

    DOTNETSCRIPTING_API void SetGamePaused(bool bPaused)
    {
        FUnrealEngineAPI::SetGamePaused(bPaused);
    }

    DOTNETSCRIPTING_API float GetDistanceBetweenPoints(float X1, float Y1, float Z1, float X2, float Y2, float Z2)
    {
        return FUnrealEngineAPI::GetDistanceBetweenPoints(X1, Y1, Z1, X2, Y2, Z2);
    }

    // === PED FACTORY EXPORTS ===
    DOTNETSCRIPTING_API void* GetPedFactory()
    {
        return FUnrealEngineAPI::GetPedFactory();
    }

    DOTNETSCRIPTING_API void* SpawnPed(const char* CharacterName, const char* VariationName, float X, float Y, float Z, float Roll, float Pitch, float Yaw, bool bAIEnabled, bool bPlayerControlled)
    {
        return FUnrealEngineAPI::SpawnPed(CharacterName, VariationName, X, Y, Z, Roll, Pitch, Yaw, bAIEnabled, bPlayerControlled);
    }

    DOTNETSCRIPTING_API bool PossessPed(void* PedPtr, void* PlayerControllerPtr)
    {
        return FUnrealEngineAPI::PossessPed(PedPtr, PlayerControllerPtr);
    }

    DOTNETSCRIPTING_API bool UnpossessPed(void* PlayerControllerPtr)
    {
        return FUnrealEngineAPI::UnpossessPed(PlayerControllerPtr);
    }

    DOTNETSCRIPTING_API void SetPedAIEnabled(void* PedPtr, bool bEnabled)
    {
        FUnrealEngineAPI::SetPedAIEnabled(PedPtr, bEnabled);
    }

    DOTNETSCRIPTING_API void* FindPedByName(const char* PedName)
    {
        return FUnrealEngineAPI::FindPedByName(PedName);
    }

    // === TASK SYSTEM EXPORTS ===
    DOTNETSCRIPTING_API int CreateOneShotTask(const char* TaskName, int Priority)
    {
        return FUnrealEngineAPI::CreateOneShotTask(TaskName, Priority);
    }

    DOTNETSCRIPTING_API int CreateComplexTask(const char* TaskName, int Priority, const char** SubTaskNames, int SubTaskCount)
    {
        return FUnrealEngineAPI::CreateComplexTask(TaskName, Priority, SubTaskNames, SubTaskCount);
    }

    DOTNETSCRIPTING_API int CreateWildComplexTask(const char* TaskName, int Priority, bool bAdaptive)
    {
        return FUnrealEngineAPI::CreateWildComplexTask(TaskName, Priority, bAdaptive);
    }

    DOTNETSCRIPTING_API bool AssignTaskToPed(void* PedPtr, int TaskHandle)
    {
        return FUnrealEngineAPI::AssignTaskToPed(PedPtr, TaskHandle);
    }

    DOTNETSCRIPTING_API bool RemoveTaskFromPed(void* PedPtr, int TaskHandle)
    {
        return FUnrealEngineAPI::RemoveTaskFromPed(PedPtr, TaskHandle);
    }

    DOTNETSCRIPTING_API void ClearAllTasksFromPed(void* PedPtr)
    {
        FUnrealEngineAPI::ClearAllTasksFromPed(PedPtr);
    }

    DOTNETSCRIPTING_API bool InterruptCurrentTask(void* PedPtr)
    {
        return FUnrealEngineAPI::InterruptCurrentTask(PedPtr);
    }

    DOTNETSCRIPTING_API int GetTaskState(int TaskHandle)
    {
        return FUnrealEngineAPI::GetTaskState(TaskHandle);
    }

    DOTNETSCRIPTING_API bool IsTaskRunning(int TaskHandle)
    {
        return FUnrealEngineAPI::IsTaskRunning(TaskHandle);
    }

    DOTNETSCRIPTING_API bool IsTaskCompleted(int TaskHandle)
    {
        return FUnrealEngineAPI::IsTaskCompleted(TaskHandle);
    }

    DOTNETSCRIPTING_API const char* GetTaskName(int TaskHandle)
    {
        return FUnrealEngineAPI::GetTaskName(TaskHandle);
    }

    DOTNETSCRIPTING_API int GetTaskPriority(int TaskHandle)
    {
        return FUnrealEngineAPI::GetTaskPriority(TaskHandle);
    }

    DOTNETSCRIPTING_API int GetActiveTaskCount(void* PedPtr)
    {
        return FUnrealEngineAPI::GetActiveTaskCount(PedPtr);
    }

    DOTNETSCRIPTING_API int GetCurrentTask(void* PedPtr)
    {
        return FUnrealEngineAPI::GetCurrentTask(PedPtr);
    }

    DOTNETSCRIPTING_API void GetAllActiveTasks(void* PedPtr, int* OutTaskHandles, int* OutCount)
    {
        if (OutTaskHandles && OutCount)
        {
            FUnrealEngineAPI::GetAllActiveTasks(PedPtr, OutTaskHandles, *OutCount);
        }
    }

    DOTNETSCRIPTING_API void* GetTaskManager(void* PedPtr)
    {
        return FUnrealEngineAPI::GetTaskManager(PedPtr);
    }

    // === PED CORE EXPORTS ===
    DOTNETSCRIPTING_API const char* GetPedCharacterName(void* PedPtr)
    {
        return FUnrealEngineAPI::GetPedCharacterName(PedPtr);
    }

    DOTNETSCRIPTING_API bool IsPedPlayerControlled(void* PedPtr)
    {
        return FUnrealEngineAPI::IsPedPlayerControlled(PedPtr);
    }

    DOTNETSCRIPTING_API bool IsPedAIEnabled(void* PedPtr)
    {
        return FUnrealEngineAPI::IsPedAIEnabled(PedPtr);
    }

    DOTNETSCRIPTING_API void SetPedPlayerControlled(void* PedPtr, bool bPlayerControlled)
    {
        FUnrealEngineAPI::SetPedPlayerControlled(PedPtr, bPlayerControlled);
    }

    DOTNETSCRIPTING_API void GetAllPedsInWorld(void** OutPedPtrs, int* OutCount)
    {
        if (OutPedPtrs && OutCount)
        {
            FUnrealEngineAPI::GetAllPedsInWorld(OutPedPtrs, *OutCount);
        }
    }

    DOTNETSCRIPTING_API float GetDistanceBetweenPeds(void* Ped1Ptr, void* Ped2Ptr)
    {
        return FUnrealEngineAPI::GetDistanceBetweenPeds(Ped1Ptr, Ped2Ptr);
    }
}
