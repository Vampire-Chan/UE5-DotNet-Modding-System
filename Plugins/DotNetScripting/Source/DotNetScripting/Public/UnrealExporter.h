#pragma once

#include "CoreMinimal.h"
#include "Engine/World.h"
#include "GameFramework/Actor.h"

// Simple C exports for .NET interop with proper type conversion
// Handles UE types ↔ C# types automatically

extern "C" {
    // === LOGGING ===
    DOTNETSCRIPTING_API void UE_LogInfo(const char* Category, const char* Message);
    DOTNETSCRIPTING_API void UE_LogWarning(const char* Category, const char* Message);
    DOTNETSCRIPTING_API void UE_LogError(const char* Category, const char* Message);
    
    // === WORLD/ACTOR MANAGEMENT ===
    DOTNETSCRIPTING_API void* UE_SpawnActor(const char* ActorClassName, float X, float Y, float Z, float Pitch, float Yaw, float Roll);
    DOTNETSCRIPTING_API bool UE_DestroyActor(void* Actor);
    DOTNETSCRIPTING_API bool UE_IsActorValid(void* Actor);
    
    // === ACTOR PROPERTIES ===
    DOTNETSCRIPTING_API void UE_GetActorLocation(void* Actor, float* OutX, float* OutY, float* OutZ);
    DOTNETSCRIPTING_API void UE_SetActorLocation(void* Actor, float X, float Y, float Z);
    DOTNETSCRIPTING_API void UE_GetActorRotation(void* Actor, float* OutPitch, float* OutYaw, float* OutRoll);
    DOTNETSCRIPTING_API void UE_SetActorRotation(void* Actor, float Pitch, float Yaw, float Roll);
    
    // === GAME STATE ===
    DOTNETSCRIPTING_API void* UE_GetPlayerPawn();
    DOTNETSCRIPTING_API void UE_GetPlayerLocation(float* OutX, float* OutY, float* OutZ);
    DOTNETSCRIPTING_API void UE_SetPlayerLocation(float X, float Y, float Z);
    
    // === YOUR GAME SYSTEMS INTEGRATION ===
    // These will connect to your actual PedFactory, TaskManager, etc.
    DOTNETSCRIPTING_API void* UE_SpawnPedFromFactory(const char* CharacterName, const char* VariationName, float X, float Y, float Z, float Pitch, float Yaw, float Roll);
    DOTNETSCRIPTING_API bool UE_GivePedTaskFromManager(void* Ped, const char* TaskName, float X, float Y, float Z);
    DOTNETSCRIPTING_API int UE_GetPedTaskStateFromManager(void* Ped);
    
    // === MEMORY MANAGEMENT ===
    // C# GC integration - UE manages objects, C# just holds handles
    DOTNETSCRIPTING_API bool UE_IsHandleValid(void* Handle);
    DOTNETSCRIPTING_API void UE_ReleaseHandle(void* Handle); // Optional cleanup hint
}

// Type conversion helpers (internal)
namespace TypeConversion
{
    // UE ↔ C# type conversion
    FVector ToFVector(float x, float y, float z);
    void FromFVector(const FVector& vec, float* x, float* y, float* z);
    
    FRotator ToFRotator(float pitch, float yaw, float roll);
    void FromFRotator(const FRotator& rot, float* pitch, float* yaw, float* roll);
    
    FString ToFString(const char* str);
    const char* FromFString(const FString& str); // Returns static buffer
    
    // Memory management
    bool IsValidUEObject(void* Handle);
}
