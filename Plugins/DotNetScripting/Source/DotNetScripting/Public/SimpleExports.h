#pragma once

// Simple header for C exports
// These are the ONLY functions exposed to .NET

extern "C" 
{
    // Logging
    __declspec(dllexport) void UE_LogInfo(const char* message);
    __declspec(dllexport) void UE_LogWarning(const char* message);
    __declspec(dllexport) void UE_LogError(const char* message);
    
    // Ped System
    __declspec(dllexport) void* UE_SpawnPed(const char* pedName, float x, float y, float z);
    __declspec(dllexport) void UE_GivePedTask(void* pedHandle, const char* taskName, float targetX, float targetY, float targetZ);
    __declspec(dllexport) void UE_GetPedPosition(void* pedHandle, float* x, float* y, float* z);
    
    // Game State
    __declspec(dllexport) void UE_GetPlayerPosition(float* x, float* y, float* z);
    __declspec(dllexport) const char* UE_GetGameMode();
}
