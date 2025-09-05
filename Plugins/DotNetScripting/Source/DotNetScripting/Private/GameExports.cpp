#include "GameExports.h"
#include "Engine/World.h"
#include "Engine/Engine.h"
#include "Kismet/GameplayStatics.h"
#include "GameFramework/GameModeBase.h"
#include "Components/SkeletalMeshComponent.h"
#include "Engine/SkeletalMesh.h"
#include "Materials/MaterialInterface.h"
#include "GameFramework/Character.h"
#include "GameFramework/Pawn.h"

// Forward declarations - no hard dependencies yet
class UPedFactory;
class APed;
class UTaskManager;

// Helper function declarations
void LoadModularComponents(USkeletalMeshComponent* MainMesh, 
                          const char* headVariation,
                          const char* upperVariation, 
                          const char* lowerVariation,
                          const char* feetVariation,
                          const char* handVariation);

// ═══════════════════════════════════════════════════════════════
// HELPER FUNCTIONS (Dynamic Resolution)
// ═══════════════════════════════════════════════════════════════

UWorld* GetCurrentWorld()
{
    if (GEngine && GEngine->GetWorldContexts().Num() > 0)
    {
        return GEngine->GetWorldContexts()[0].World();
    }
    return nullptr;
}

// Dynamic resolution of game systems - will work when Game module is loaded
AGameModeBase* GetGameMode()
{
    UWorld* World = GetCurrentWorld();
    if (!World) return nullptr;
    
    // Use base class only - safe without hard dependency
    return World->GetAuthGameMode();
}

UPedFactory* GetPedFactory()
{
    // Simplified approach - just return nullptr for now
    // Will be implemented properly when Game builds successfully
    UE_LOG(LogTemp, Warning, TEXT("[MODDING] PedFactory access not yet implemented"));
    return nullptr;
}

// Helper function to load modular character components
void LoadModularComponents(USkeletalMeshComponent* MainMesh, 
                          const char* headVariation,
                          const char* upperVariation, 
                          const char* lowerVariation,
                          const char* feetVariation,
                          const char* handVariation)
{
    if (!MainMesh)
    {
        UE_LOG(LogTemp, Error, TEXT("[MODDING] No main mesh component provided"));
        return;
    }

    UE_LOG(LogTemp, Log, TEXT("[MODDING] Loading modular components for PlayerNiko..."));

    // Define base paths for PlayerNiko modular components
    FString BasePath = TEXT("/Game/Characters/PlayerNiko");
    
    // Load Head Component
    if (headVariation && strlen(headVariation) > 0)
    {
        FString HeadMeshPath = FString::Printf(TEXT("%s/head/%s.%s"), *BasePath, UTF8_TO_TCHAR(headVariation), UTF8_TO_TCHAR(headVariation));
        FString HeadTexturePath = FString::Printf(TEXT("%s/head/Texture/head_diff_%s_a_whi.head_diff_%s_a_whi"), *BasePath, UTF8_TO_TCHAR(headVariation), UTF8_TO_TCHAR(headVariation));
        
        UE_LOG(LogTemp, Log, TEXT("[MODDING] Loading head mesh: %s"), *HeadMeshPath);
        UE_LOG(LogTemp, Log, TEXT("[MODDING] Loading head texture: %s"), *HeadTexturePath);
    }
    else
    {
        // Default head - head_000
        FString HeadMeshPath = FString::Printf(TEXT("%s/head/head_000.head_000"), *BasePath);
        FString HeadTexturePath = FString::Printf(TEXT("%s/head/Texture/head_diff_000_a_whi.head_diff_000_a_whi"), *BasePath);
        
        UE_LOG(LogTemp, Log, TEXT("[MODDING] Loading default head mesh: %s"), *HeadMeshPath);
        UE_LOG(LogTemp, Log, TEXT("[MODDING] Loading default head texture: %s"), *HeadTexturePath);
    }

    // Load Upper Body Component
    if (upperVariation && strlen(upperVariation) > 0)
    {
        FString UpperMeshPath = FString::Printf(TEXT("%s/uppr/%s.%s"), *BasePath, UTF8_TO_TCHAR(upperVariation), UTF8_TO_TCHAR(upperVariation));
        FString UpperTexturePath = FString::Printf(TEXT("%s/uppr/Texture/uppr_diff_%s_a_uni.uppr_diff_%s_a_uni"), *BasePath, UTF8_TO_TCHAR(upperVariation), UTF8_TO_TCHAR(upperVariation));
        
        UE_LOG(LogTemp, Log, TEXT("[MODDING] Loading upper body mesh: %s"), *UpperMeshPath);
        UE_LOG(LogTemp, Log, TEXT("[MODDING] Loading upper body texture: %s"), *UpperTexturePath);
    }
    else
    {
        // Default upper - uppr_000
        FString UpperMeshPath = FString::Printf(TEXT("%s/uppr/uppr_000.uppr_000"), *BasePath);
        FString UpperTexturePath = FString::Printf(TEXT("%s/uppr/Texture/uppr_diff_000_a_uni.uppr_diff_000_a_uni"), *BasePath);
        
        UE_LOG(LogTemp, Log, TEXT("[MODDING] Loading default upper body mesh: %s"), *UpperMeshPath);
        UE_LOG(LogTemp, Log, TEXT("[MODDING] Loading default upper body texture: %s"), *UpperTexturePath);
    }

    // Load Lower Body Component
    if (lowerVariation && strlen(lowerVariation) > 0)
    {
        FString LowerMeshPath = FString::Printf(TEXT("%s/lowr/%s.%s"), *BasePath, UTF8_TO_TCHAR(lowerVariation), UTF8_TO_TCHAR(lowerVariation));
        UE_LOG(LogTemp, Log, TEXT("[MODDING] Loading lower body mesh: %s"), *LowerMeshPath);
    }
    else
    {
        UE_LOG(LogTemp, Log, TEXT("[MODDING] Loading default lower body components"));
    }

    // Load Feet Component
    if (feetVariation && strlen(feetVariation) > 0)
    {
        FString FeetMeshPath = FString::Printf(TEXT("%s/feet/%s.%s"), *BasePath, UTF8_TO_TCHAR(feetVariation), UTF8_TO_TCHAR(feetVariation));
        UE_LOG(LogTemp, Log, TEXT("[MODDING] Loading feet mesh: %s"), *FeetMeshPath);
    }
    else
    {
        UE_LOG(LogTemp, Log, TEXT("[MODDING] Loading default feet components"));
    }

    // Load Hand Component
    if (handVariation && strlen(handVariation) > 0)
    {
        FString HandMeshPath = FString::Printf(TEXT("%s/hand/%s.%s"), *BasePath, UTF8_TO_TCHAR(handVariation), UTF8_TO_TCHAR(handVariation));
        UE_LOG(LogTemp, Log, TEXT("[MODDING] Loading hand mesh: %s"), *HandMeshPath);
    }
    else
    {
        UE_LOG(LogTemp, Log, TEXT("[MODDING] Loading default hand components"));
    }

    UE_LOG(LogTemp, Log, TEXT("[MODDING] Modular component loading complete (placeholder implementation)"));
    UE_LOG(LogTemp, Warning, TEXT("[MODDING] Full modular assembly requires UE asset loading system integration"));
}

// ═══════════════════════════════════════════════════════════════
// EXPORTED FUNCTIONS (Safe Implementation)
// ═══════════════════════════════════════════════════════════════

// LOGGING SYSTEM (always works)
extern "C" DOTNETSCRIPTING_API void Game_Log(const char* message)
{
    if (message)
    {
        UE_LOG(LogTemp, Log, TEXT("[MODDING] %s"), UTF8_TO_TCHAR(message));
    }
}

extern "C" DOTNETSCRIPTING_API void Game_LogWarning(const char* message)
{
    if (message)
    {
        UE_LOG(LogTemp, Warning, TEXT("[MODDING] %s"), UTF8_TO_TCHAR(message));
    }
}

extern "C" DOTNETSCRIPTING_API void Game_LogError(const char* message)
{
    if (message)
    {
        UE_LOG(LogTemp, Error, TEXT("[MODDING] %s"), UTF8_TO_TCHAR(message));
    }
}

// WORLD QUERIES (Type-safe versions)
extern "C" DOTNETSCRIPTING_API void World_GetPlayerPosition_Native(FVector3f_Interop* outPosition)
{
    if (!outPosition)
    {
        UE_LOG(LogTemp, Warning, TEXT("[MODDING] World_GetPlayerPosition called with null output parameter"));
        return;
    }
    
    *outPosition = {0.0f, 0.0f, 0.0f};
    
    UWorld* World = GetCurrentWorld();
    if (!World) return;

    APawn* PlayerPawn = UGameplayStatics::GetPlayerPawn(World, 0);
    if (!PlayerPawn) return;

    FVector Position = PlayerPawn->GetActorLocation();
    outPosition->X = Position.X;
    outPosition->Y = Position.Y;
    outPosition->Z = Position.Z;
}

extern "C" DOTNETSCRIPTING_API void World_SetPlayerPosition_Native(FVector3f_Interop position)
{
    UWorld* World = GetCurrentWorld();
    if (!World) return;

    APawn* PlayerPawn = UGameplayStatics::GetPlayerPawn(World, 0);
    if (PlayerPawn)
    {
        PlayerPawn->SetActorLocation(FVector(position.X, position.Y, position.Z));
    }
}

extern "C" DOTNETSCRIPTING_API void World_GetPlayerRotation_Native(FRotator_Interop* outRotation)
{
    if (!outRotation)
    {
        UE_LOG(LogTemp, Warning, TEXT("[MODDING] World_GetPlayerRotation called with null output parameter"));
        return;
    }
    
    *outRotation = {0.0f, 0.0f, 0.0f};
    
    UWorld* World = GetCurrentWorld();
    if (!World) return;

    APawn* PlayerPawn = UGameplayStatics::GetPlayerPawn(World, 0);
    if (!PlayerPawn) return;

    FRotator Rotation = PlayerPawn->GetActorRotation();
    outRotation->Pitch = Rotation.Pitch;
    outRotation->Yaw = Rotation.Yaw;
    outRotation->Roll = Rotation.Roll;
}

extern "C" DOTNETSCRIPTING_API void World_SetPlayerRotation_Native(FRotator_Interop rotation)
{
    UWorld* World = GetCurrentWorld();
    if (!World) return;

    APawn* PlayerPawn = UGameplayStatics::GetPlayerPawn(World, 0);
    if (PlayerPawn)
    {
        PlayerPawn->SetActorRotation(FRotator(rotation.Pitch, rotation.Yaw, rotation.Roll));
    }
}

// Legacy functions for backward compatibility
extern "C" DOTNETSCRIPTING_API void World_GetPlayerPosition(float* x, float* y, float* z)
{
    FVector3f_Interop pos;
    World_GetPlayerPosition_Native(&pos);
    if (x) *x = pos.X;
    if (y) *y = pos.Y;
    if (z) *z = pos.Z;
}

extern "C" DOTNETSCRIPTING_API void World_SetPlayerPosition(float x, float y, float z)
{
    FVector3f_Interop pos = {x, y, z};
    World_SetPlayerPosition_Native(pos);
}

// MATH UTILITIES (no dependencies)
extern "C" DOTNETSCRIPTING_API float Math_Distance(float x1, float y1, float z1, float x2, float y2, float z2)
{
    FVector A(x1, y1, z1);
    FVector B(x2, y2, z2);
    return FVector::Dist(A, B);
}

extern "C" DOTNETSCRIPTING_API float Math_Distance2D(float x1, float y1, float x2, float y2)
{
    FVector2D A(x1, y1);
    FVector2D B(x2, y2);
    return FVector2D::Distance(A, B);
}

// PED SYSTEM (Type-safe versions)
extern "C" DOTNETSCRIPTING_API void* PedFactory_Spawn_Native(const char* characterName, const char* variation, FVector3f_Interop position, FRotator_Interop rotation)
{
    UPedFactory* Factory = GetPedFactory();
    if (!Factory)
    {
        UE_LOG(LogTemp, Warning, TEXT("[MODDING] PedFactory not available yet"));
        return nullptr;
    }

    FVector SpawnLocation(position.X, position.Y, position.Z);
    FRotator SpawnRotation(rotation.Pitch, rotation.Yaw, rotation.Roll);

    // TODO: Implement when PedFactory structure is confirmed
    UE_LOG(LogTemp, Log, TEXT("[MODDING] PedFactory_Spawn called: %s at (%f,%f,%f)"), 
           UTF8_TO_TCHAR(characterName), position.X, position.Y, position.Z);
    return nullptr;
}

extern "C" DOTNETSCRIPTING_API void* PedFactory_SpawnModularCharacter_Native(const char* characterPath, 
                                                                           const char* headVariation,
                                                                           const char* upperVariation, 
                                                                           const char* lowerVariation,
                                                                           const char* feetVariation,
                                                                           const char* handVariation,
                                                                           FVector3f_Interop position, 
                                                                           FRotator_Interop rotation)
{
    UWorld* World = GetCurrentWorld();
    if (!World)
    {
        UE_LOG(LogTemp, Error, TEXT("[MODDING] No valid world for spawning modular character"));
        return nullptr;
    }

    FVector SpawnLocation(position.X, position.Y, position.Z);
    FRotator SpawnRotation(rotation.Pitch, rotation.Yaw, rotation.Roll);

    UE_LOG(LogTemp, Log, TEXT("[MODDING] Attempting to spawn modular character: %s"), UTF8_TO_TCHAR(characterPath));
    UE_LOG(LogTemp, Log, TEXT("[MODDING] - Head: %s"), UTF8_TO_TCHAR(headVariation ? headVariation : "default"));
    UE_LOG(LogTemp, Log, TEXT("[MODDING] - Upper: %s"), UTF8_TO_TCHAR(upperVariation ? upperVariation : "default"));
    UE_LOG(LogTemp, Log, TEXT("[MODDING] - Lower: %s"), UTF8_TO_TCHAR(lowerVariation ? lowerVariation : "default"));
    UE_LOG(LogTemp, Log, TEXT("[MODDING] - Feet: %s"), UTF8_TO_TCHAR(feetVariation ? feetVariation : "default"));
    UE_LOG(LogTemp, Log, TEXT("[MODDING] - Hands: %s"), UTF8_TO_TCHAR(handVariation ? handVariation : "default"));
    UE_LOG(LogTemp, Log, TEXT("[MODDING] - Position: (%f, %f, %f)"), position.X, position.Y, position.Z);

    // Try to spawn a basic Character as a foundation
    FActorSpawnParameters SpawnParams;
    SpawnParams.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;
    
    // For now, spawn a basic ACharacter and log the attempt
    // This will be expanded when we have proper modular character system
    ACharacter* SpawnedCharacter = World->SpawnActor<ACharacter>(ACharacter::StaticClass(), SpawnLocation, SpawnRotation, SpawnParams);
    
    if (SpawnedCharacter)
    {
        UE_LOG(LogTemp, Log, TEXT("[MODDING] Successfully spawned base character, now setting up modular components..."));
        
        // Get the skeletal mesh component
        USkeletalMeshComponent* MeshComp = SpawnedCharacter->GetMesh();
        if (MeshComp)
        {
            // Load the main skeleton for PlayerNiko
            FString SkeletonPath = FString::Printf(TEXT("/Game/Characters/PlayerNiko/SKEL_PlayerNiko.SKEL_PlayerNiko"));
            USkeletalMesh* MainSkeleton = LoadObject<USkeletalMesh>(nullptr, *SkeletonPath);
            
            if (MainSkeleton)
            {
                UE_LOG(LogTemp, Log, TEXT("[MODDING] Loaded main skeleton: %s"), *SkeletonPath);
                MeshComp->SetSkeletalMesh(MainSkeleton);
                
                // TODO: Load and attach modular components based on variations
                // This would involve:
                // 1. Loading individual body part meshes (head_000.uasset, uppr_000.uasset, etc.)
                // 2. Loading appropriate textures (head_diff_000_a_whi.uasset, etc.)
                // 3. Creating materials and applying them
                // 4. Attaching components to the main skeleton
                
                LoadModularComponents(MeshComp, headVariation, upperVariation, lowerVariation, feetVariation, handVariation);
            }
            else
            {
                UE_LOG(LogTemp, Warning, TEXT("[MODDING] Failed to load main skeleton: %s"), *SkeletonPath);
            }
        }
        
        return SpawnedCharacter;
    }
    else
    {
        UE_LOG(LogTemp, Error, TEXT("[MODDING] Failed to spawn base character"));
        return nullptr;
    }
}

extern "C" DOTNETSCRIPTING_API void Ped_GetPosition_Native(void* ped, FVector3f_Interop* outPosition)
{
    if (!outPosition)
    {
        UE_LOG(LogTemp, Warning, TEXT("[MODDING] Ped_GetPosition called with null output parameter"));
        return;
    }
    
    *outPosition = {0.0f, 0.0f, 0.0f};
    
    if (!ped)
    {
        UE_LOG(LogTemp, Warning, TEXT("[MODDING] Ped_GetPosition called with null ped"));
        return;
    }

    // TODO: Implement when Ped structure is confirmed
    UE_LOG(LogTemp, Log, TEXT("[MODDING] Ped_GetPosition called"));
}

extern "C" DOTNETSCRIPTING_API void Ped_SetPosition_Native(void* ped, FVector3f_Interop position)
{
    if (!ped)
    {
        UE_LOG(LogTemp, Warning, TEXT("[MODDING] Ped_SetPosition called with null ped"));
        return;
    }

    // TODO: Implement when Ped structure is confirmed
    UE_LOG(LogTemp, Log, TEXT("[MODDING] Ped_SetPosition called: (%f,%f,%f)"), 
           position.X, position.Y, position.Z);
}

extern "C" DOTNETSCRIPTING_API void Ped_GetRotation_Native(void* ped, FRotator_Interop* outRotation)
{
    if (!outRotation)
    {
        UE_LOG(LogTemp, Warning, TEXT("[MODDING] Ped_GetRotation called with null output parameter"));
        return;
    }
    
    *outRotation = {0.0f, 0.0f, 0.0f};
    
    if (!ped)
    {
        UE_LOG(LogTemp, Warning, TEXT("[MODDING] Ped_GetRotation called with null ped"));
        return;
    }

    // TODO: Implement when Ped structure is confirmed
    UE_LOG(LogTemp, Log, TEXT("[MODDING] Ped_GetRotation called"));
}

extern "C" DOTNETSCRIPTING_API void Ped_SetRotation_Native(void* ped, FRotator_Interop rotation)
{
    if (!ped)
    {
        UE_LOG(LogTemp, Warning, TEXT("[MODDING] Ped_SetRotation called with null ped"));
        return;
    }

    // TODO: Implement when Ped structure is confirmed
    UE_LOG(LogTemp, Log, TEXT("[MODDING] Ped_SetRotation called: (%f,%f,%f)"), 
           rotation.Pitch, rotation.Yaw, rotation.Roll);
}

extern "C" DOTNETSCRIPTING_API void Ped_SetHealth_Native(void* ped, float health)
{
    if (!ped)
    {
        UE_LOG(LogTemp, Warning, TEXT("[MODDING] Ped_SetHealth called with null ped"));
        return;
    }

    // TODO: Implement when Ped structure is confirmed
    UE_LOG(LogTemp, Log, TEXT("[MODDING] Ped_SetHealth called: %f"), health);
}

extern "C" DOTNETSCRIPTING_API float Ped_GetHealth_Native(void* ped)
{
    if (!ped)
    {
        UE_LOG(LogTemp, Warning, TEXT("[MODDING] Ped_GetHealth called with null ped"));
        return 0.0f;
    }

    // TODO: Implement when Ped structure is confirmed
    UE_LOG(LogTemp, Log, TEXT("[MODDING] Ped_GetHealth called"));
    return 100.0f; // Default health for now
}

// Legacy function for backward compatibility
extern "C" DOTNETSCRIPTING_API void* PedFactory_Spawn(const char* characterName, const char* variation, float x, float y, float z, float yaw)
{
    FVector3f_Interop position = {x, y, z};
    FRotator_Interop rotation = {0.0, yaw, 0.0}; // Convert yaw to full rotation
    return PedFactory_Spawn_Native(characterName, variation, position, rotation);
}

extern "C" DOTNETSCRIPTING_API bool PedFactory_Remove(void* ped)
{
    if (!ped) return false;
    
    // Basic validity check
    UE_LOG(LogTemp, Log, TEXT("[MODDING] PedFactory_Remove called"));
    return false; // TODO: Implement
}

extern "C" DOTNETSCRIPTING_API bool PedFactory_IsValid(void* ped)
{
    // Basic pointer check
    return ped != nullptr;
}

// Legacy PED PROPERTIES (now using type-safe versions)
extern "C" DOTNETSCRIPTING_API void Ped_GetPosition(void* ped, float* x, float* y, float* z)
{
    FVector3f_Interop pos;
    Ped_GetPosition_Native(ped, &pos);
    if (x) *x = pos.X;
    if (y) *y = pos.Y;
    if (z) *z = pos.Z;
}

extern "C" DOTNETSCRIPTING_API void Ped_SetPosition(void* ped, float x, float y, float z)
{
    FVector3f_Interop position = {x, y, z};
    Ped_SetPosition_Native(ped, position);
}

extern "C" DOTNETSCRIPTING_API float Ped_GetHeading(void* ped)
{
    FRotator_Interop rotation;
    Ped_GetRotation_Native(ped, &rotation);
    return rotation.Yaw;
}

extern "C" DOTNETSCRIPTING_API void Ped_SetHeading(void* ped, float heading)
{
    FRotator_Interop rotation;
    Ped_GetRotation_Native(ped, &rotation);
    rotation.Yaw = heading;
    Ped_SetRotation_Native(ped, rotation);
}

// TASK SYSTEM (placeholder for now)
extern "C" DOTNETSCRIPTING_API bool TaskManager_GiveTask(void* ped, const char* taskType, float x, float y, float z)
{
    if (!ped || !taskType) return false;
    
    UE_LOG(LogTemp, Log, TEXT("[MODDING] TaskManager_GiveTask: %s"), UTF8_TO_TCHAR(taskType));
    return false; // TODO: Implement when TaskManager is available
}

extern "C" DOTNETSCRIPTING_API bool TaskManager_StopCurrentTask(void* ped)
{
    if (!ped) return false;
    
    UE_LOG(LogTemp, Log, TEXT("[MODDING] TaskManager_StopCurrentTask called"));
    return false; // TODO: Implement
}
