#pragma once

#include "CoreMinimal.h"

// =====================================================
// UNREAL ENGINE TYPE DEFINITIONS FOR C# INTEROP
// Matches the C# TypeConversions structures exactly
// =====================================================

// UE FVector3f for C# interop (32-bit floats)
struct FVector3f_Interop
{
    float X;
    float Y;
    float Z;
    
    FVector3f_Interop() : X(0), Y(0), Z(0) {}
    FVector3f_Interop(float x, float y, float z) : X(x), Y(y), Z(z) {}
    
    // Convert from UE FVector
    FVector3f_Interop(const FVector& V) : X(V.X), Y(V.Y), Z(V.Z) {}
    
    // Convert to UE FVector
    FVector ToFVector() const { return FVector(X, Y, Z); }
};

// UE FRotator for C# interop (double precision to match C#)
struct FRotator_Interop
{
    double Pitch;
    double Yaw; 
    double Roll;
    
    FRotator_Interop() : Pitch(0), Yaw(0), Roll(0) {}
    FRotator_Interop(double pitch, double yaw, double roll) : Pitch(pitch), Yaw(yaw), Roll(roll) {}
    
    // Convert from UE FRotator
    FRotator_Interop(const FRotator& R) : Pitch(R.Pitch), Yaw(R.Yaw), Roll(R.Roll) {}
    
    // Convert to UE FRotator
    FRotator ToFRotator() const { return FRotator(Pitch, Yaw, Roll); }
};

// UE FLinearColor for C# interop
struct FLinearColor_Interop
{
    float R, G, B, A;
    
    FLinearColor_Interop() : R(0), G(0), B(0), A(1) {}
    FLinearColor_Interop(float r, float g, float b, float a = 1.0f) : R(r), G(g), B(b), A(a) {}
    
    // Convert from UE FLinearColor
    FLinearColor_Interop(const FLinearColor& C) : R(C.R), G(C.G), B(C.B), A(C.A) {}
    
    // Convert to UE FLinearColor
    FLinearColor ToFLinearColor() const { return FLinearColor(R, G, B, A); }
};

// =====================================================
// GAME ENGINE EXPORTS FOR C# INTEROP
// Type-safe functions with proper UE conversions
// =====================================================

extern "C" {
    
    // ═══════════════════════════════════════════════════════════════
    // LOGGING FUNCTIONS - String marshaling
    // ═══════════════════════════════════════════════════════════════
    
    DOTNETSCRIPTING_API void Game_Log_Native(const char* message);
    DOTNETSCRIPTING_API void Game_LogWarning_Native(const char* message);
    DOTNETSCRIPTING_API void Game_LogError_Native(const char* message);

    // ═══════════════════════════════════════════════════════════════
    // WORLD/PLAYER FUNCTIONS - UE type safe
    // ═══════════════════════════════════════════════════════════════
    
    DOTNETSCRIPTING_API void World_GetPlayerPosition_Native(FVector3f_Interop* position);
    DOTNETSCRIPTING_API void World_SetPlayerPosition_Native(FVector3f_Interop position);
    DOTNETSCRIPTING_API int World_GetPedCount();

    // ═══════════════════════════════════════════════════════════════
    // PED FACTORY FUNCTIONS - Type safe spawning
    // ═══════════════════════════════════════════════════════════════
    
    DOTNETSCRIPTING_API void* PedFactory_Spawn_Native(const char* characterName, const char* variation, 
                                                     FVector3f_Interop position, FRotator_Interop rotation);
    
    // Modular Character Spawning - PlayerNiko specific
    DOTNETSCRIPTING_API void* PedFactory_SpawnModularCharacter_Native(const char* characterPath, 
                                                                     const char* headVariation,
                                                                     const char* upperVariation, 
                                                                     const char* lowerVariation,
                                                                     const char* feetVariation,
                                                                     const char* handVariation,
                                                                     FVector3f_Interop position, 
                                                                     FRotator_Interop rotation);
    
    DOTNETSCRIPTING_API bool PedFactory_Remove(void* pedHandle);
    DOTNETSCRIPTING_API bool PedFactory_IsValid(void* pedHandle);
    DOTNETSCRIPTING_API bool PedFactory_Possess(void* pedHandle);
    DOTNETSCRIPTING_API bool PedFactory_Unpossess();
    DOTNETSCRIPTING_API void* PedFactory_GetPlayerPed();

    // ═══════════════════════════════════════════════════════════════
    // PED CONTROL FUNCTIONS - UE type safe
    // ═══════════════════════════════════════════════════════════════
    
    DOTNETSCRIPTING_API void Ped_GetPosition_Native(void* pedHandle, FVector3f_Interop* position);
    DOTNETSCRIPTING_API void Ped_SetPosition_Native(void* pedHandle, FVector3f_Interop position);
    DOTNETSCRIPTING_API void Ped_GetRotation_Native(void* pedHandle, FRotator_Interop* rotation);
    DOTNETSCRIPTING_API void Ped_SetRotation_Native(void* pedHandle, FRotator_Interop rotation);

    // ═══════════════════════════════════════════════════════════════
    // UTILITY FUNCTIONS - Type safe math
    // ═══════════════════════════════════════════════════════════════
    
    DOTNETSCRIPTING_API float Math_Distance_Native(FVector3f_Interop pos1, FVector3f_Interop pos2);
    DOTNETSCRIPTING_API float Math_Distance2D_Native(FVector3f_Interop pos1, FVector3f_Interop pos2);
    DOTNETSCRIPTING_API void Math_RandomPosition_Native(FVector3f_Interop center, float radius, FVector3f_Interop* outPosition);

    // ═══════════════════════════════════════════════════════════════
    // ADVANCED TYPE CONVERSIONS - For future expansion
    // ═══════════════════════════════════════════════════════════════
    
    // Array handling
    DOTNETSCRIPTING_API int Array_GetPedsInRange(FVector3f_Interop center, float radius, void** outPeds, int maxCount);
    DOTNETSCRIPTING_API void Array_FreePedArray(void** pedArray, int count);
    
    // String operations
    DOTNETSCRIPTING_API void String_GetPedName(void* pedHandle, char* outName, int maxLength);
    DOTNETSCRIPTING_API bool String_SetPedName(void* pedHandle, const char* newName);
    
    // Color operations  
    DOTNETSCRIPTING_API void Ped_SetColor(void* pedHandle, FLinearColor_Interop color);
    DOTNETSCRIPTING_API void Ped_GetColor(void* pedHandle, FLinearColor_Interop* outColor);
}
