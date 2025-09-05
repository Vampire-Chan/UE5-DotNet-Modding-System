#include "UnrealExporter.h"
#include "Engine/World.h"
#include "Engine/Engine.h"
#include "GameFramework/GameModeBase.h"
#include "GameFramework/PlayerController.h"
#include "GameFramework/Pawn.h"
#include "Kismet/GameplayStatics.h"

// Simple type conversion helpers with memory management
namespace TypeConversion
{
    // === TYPE CONVERSION: UE ↔ C# ===
    
    FVector ToFVector(float x, float y, float z)
    {
        return FVector(x, y, z);
    }
    
    void FromFVector(const FVector& vec, float* x, float* y, float* z)
    {
        *x = vec.X; *y = vec.Y; *z = vec.Z;
    }
    
    FRotator ToFRotator(float pitch, float yaw, float roll)
    {
        return FRotator(pitch, yaw, roll);
    }
    
    void FromFRotator(const FRotator& rot, float* pitch, float* yaw, float* roll)
    {
        *pitch = rot.Pitch; *yaw = rot.Yaw; *roll = rot.Roll;
    }
    
    FString ToFString(const char* str)
    {
        return str ? FString(UTF8_TO_TCHAR(str)) : FString();
    }
    
    // Returns pointer to static buffer - safe for P/Invoke return
    const char* FromFString(const FString& str)
    {
        static TArray<char> ConversionBuffer;
        ConversionBuffer.Empty();
        
        FTCHARToUTF8 Converter(*str);
        int32 Length = Converter.Length();
        ConversionBuffer.AddUninitialized(Length + 1);
        FMemory::Memcpy(ConversionBuffer.GetData(), Converter.Get(), Length);
        ConversionBuffer[Length] = '\0';
        
        return ConversionBuffer.GetData();
    }
    
    // === MEMORY MANAGEMENT: C# ↔ UE GC Integration ===
    
    bool IsValidUEObject(void* Handle)
    {
        // UE manages object lifetime - C# just checks validity
        AActor* Actor = static_cast<AActor*>(Handle);
        return Actor && IsValid(Actor);
    }
    
    // Helper: Get current world for spawning
    UWorld* GetCurrentWorld()
    {
        if (GEngine && GEngine->GetWorldContexts().Num() > 0)
        {
            return GEngine->GetWorldContexts()[0].World();
        }
        return nullptr;
    }
    
    // Helper: Find actor class
    UClass* FindActorClass(const FString& ClassName)
    {
        return FindObject<UClass>(nullptr, *ClassName);
    }
}

// === LOGGING EXPORTS ===
extern "C" DOTNETSCRIPTING_API void UE_LogInfo(const char* Category, const char* Message)
{
    FString CategoryStr = TypeConversion::ToFString(Category);
    FString MessageStr = TypeConversion::ToFString(Message);
    UE_LOG(LogTemp, Log, TEXT("[%s] %s"), *CategoryStr, *MessageStr);
}

extern "C" DOTNETSCRIPTING_API void UE_LogWarning(const char* Category, const char* Message)
{
    FString CategoryStr = TypeConversion::ToFString(Category);
    FString MessageStr = TypeConversion::ToFString(Message);
    UE_LOG(LogTemp, Warning, TEXT("[%s] %s"), *CategoryStr, *MessageStr);
}

extern "C" DOTNETSCRIPTING_API void UE_LogError(const char* Category, const char* Message)
{
    FString CategoryStr = TypeConversion::ToFString(Category);
    FString MessageStr = TypeConversion::ToFString(Message);
    UE_LOG(LogTemp, Error, TEXT("[%s] %s"), *CategoryStr, *MessageStr);
}

// === WORLD/ACTOR MANAGEMENT EXPORTS ===
extern "C" DOTNETSCRIPTING_API void* UE_SpawnActor(const char* ActorClassName, float X, float Y, float Z, float Pitch, float Yaw, float Roll)
{
    UWorld* World = TypeConversion::GetCurrentWorld();
    if (!World)
    {
        UE_LogError("UnrealExporter", "No valid world context for spawning actor");
        return nullptr;
    }

    FString ClassName = TypeConversion::ToFString(ActorClassName);
    UClass* ActorClass = TypeConversion::FindActorClass(ClassName);
    
    if (!ActorClass)
    {
        UE_LogError("UnrealExporter", TypeConversion::FromFString(FString::Printf(TEXT("Actor class not found: %s"), *ClassName)));
        return nullptr;
    }

    FVector Location = TypeConversion::ToFVector(X, Y, Z);
    FRotator Rotation = TypeConversion::ToFRotator(Pitch, Yaw, Roll);
    
    AActor* SpawnedActor = World->SpawnActor<AActor>(ActorClass, Location, Rotation);
    
    if (SpawnedActor)
    {
        UE_LogInfo("UnrealExporter", TypeConversion::FromFString(FString::Printf(TEXT("Successfully spawned actor: %s"), *ClassName)));
        return static_cast<void*>(SpawnedActor);
    }
    
    UE_LogError("UnrealExporter", TypeConversion::FromFString(FString::Printf(TEXT("Failed to spawn actor: %s"), *ClassName)));
    return nullptr;
}

extern "C" DOTNETSCRIPTING_API bool UE_DestroyActor(void* Actor)
{
    if (!TypeConversion::IsValidUEObject(Actor))
    {
        return false;
    }
    
    AActor* ActorPtr = static_cast<AActor*>(Actor);
    ActorPtr->Destroy();
    return true;
}

extern "C" DOTNETSCRIPTING_API bool UE_IsActorValid(void* Actor)
{
    return TypeConversion::IsValidUEObject(Actor);
}

// === ACTOR PROPERTIES EXPORTS ===
extern "C" DOTNETSCRIPTING_API void UE_GetActorLocation(void* Actor, float* OutX, float* OutY, float* OutZ)
{
    if (!TypeConversion::IsValidUEObject(Actor))
    {
        *OutX = *OutY = *OutZ = 0.0f;
        return;
    }
    
    AActor* ActorPtr = static_cast<AActor*>(Actor);
    FVector Location = ActorPtr->GetActorLocation();
    TypeConversion::FromFVector(Location, OutX, OutY, OutZ);
}

extern "C" DOTNETSCRIPTING_API void UE_SetActorLocation(void* Actor, float X, float Y, float Z)
{
    if (!TypeConversion::IsValidUEObject(Actor))
    {
        return;
    }
    
    AActor* ActorPtr = static_cast<AActor*>(Actor);
    FVector NewLocation = TypeConversion::ToFVector(X, Y, Z);
    ActorPtr->SetActorLocation(NewLocation);
}

extern "C" DOTNETSCRIPTING_API void UE_GetActorRotation(void* Actor, float* OutPitch, float* OutYaw, float* OutRoll)
{
    if (!TypeConversion::IsValidUEObject(Actor))
    {
        *OutPitch = *OutYaw = *OutRoll = 0.0f;
        return;
    }
    
    AActor* ActorPtr = static_cast<AActor*>(Actor);
    FRotator Rotation = ActorPtr->GetActorRotation();
    TypeConversion::FromFRotator(Rotation, OutPitch, OutYaw, OutRoll);
}

extern "C" DOTNETSCRIPTING_API void UE_SetActorRotation(void* Actor, float Pitch, float Yaw, float Roll)
{
    if (!TypeConversion::IsValidUEObject(Actor))
    {
        return;
    }
    
    AActor* ActorPtr = static_cast<AActor*>(Actor);
    FRotator NewRotation = TypeConversion::ToFRotator(Pitch, Yaw, Roll);
    ActorPtr->SetActorRotation(NewRotation);
}

// === GAME STATE EXPORTS ===
extern "C" DOTNETSCRIPTING_API void* UE_GetPlayerPawn()
{
    UWorld* World = TypeConversion::GetCurrentWorld();
    if (!World)
    {
        return nullptr;
    }
    
    APlayerController* PlayerController = UGameplayStatics::GetPlayerController(World, 0);
    if (!PlayerController)
    {
        return nullptr;
    }
    
    APawn* PlayerPawn = PlayerController->GetPawn();
    return static_cast<void*>(PlayerPawn);
}

extern "C" DOTNETSCRIPTING_API void UE_GetPlayerLocation(float* OutX, float* OutY, float* OutZ)
{
    void* PlayerPawn = UE_GetPlayerPawn();
    UE_GetActorLocation(PlayerPawn, OutX, OutY, OutZ);
}

extern "C" DOTNETSCRIPTING_API void UE_SetPlayerLocation(float X, float Y, float Z)
{
    void* PlayerPawn = UE_GetPlayerPawn();
    UE_SetActorLocation(PlayerPawn, X, Y, Z);
}

// === YOUR GAME SYSTEMS INTEGRATION (Stubs - ready for connection) ===
extern "C" DOTNETSCRIPTING_API void* UE_SpawnPedFromFactory(const char* CharacterName, const char* VariationName, float X, float Y, float Z, float Pitch, float Yaw, float Roll)
{
    // TODO: Connect to your Source/Game/Peds/PedFactory.cpp
    UE_LogWarning("UnrealExporter", "UE_SpawnPedFromFactory ready for connection to your PedFactory system");
    
    // When ready, this will become:
    // UWorld* World = TypeConversion::GetCurrentWorld();
    // UPedFactory* Factory = World->GetSubsystem<UPedFactory>();
    // FPedSpawnConfiguration Config = { ... };
    // APed* SpawnedPed = Factory->SpawnPed(World, Config);
    // return SpawnedPed;
    
    // For now, use basic actor spawn as placeholder
    return UE_SpawnActor("Pawn", X, Y, Z, Pitch, Yaw, Roll);
}

extern "C" DOTNETSCRIPTING_API bool UE_GivePedTaskFromManager(void* Ped, const char* TaskName, float X, float Y, float Z)
{
    // TODO: Connect to your Source/Game/Tasks/TaskManager.cpp
    UE_LogWarning("UnrealExporter", "UE_GivePedTaskFromManager ready for connection to your TaskManager system");
    
    // When ready, this will become:
    // APed* PedActor = static_cast<APed*>(Ped);
    // UTaskManager* TaskManager = PedActor->GetTaskManager();
    // UBaseTask* Task = TaskFactory::CreateTask(TaskName, FVector(X,Y,Z));
    // TaskManager->GiveTask(Task);
    // return true;
    
    return false; // Placeholder
}

extern "C" DOTNETSCRIPTING_API int UE_GetPedTaskStateFromManager(void* Ped)
{
    // TODO: Connect to your Source/Game/Tasks/TaskManager.cpp
    UE_LogWarning("UnrealExporter", "UE_GetPedTaskStateFromManager ready for connection to your TaskManager system");
    
    // When ready, this will become:
    // APed* PedActor = static_cast<APed*>(Ped);
    // UTaskManager* TaskManager = PedActor->GetTaskManager();
    // UBaseTask* CurrentTask = TaskManager->GetCurrentTask();
    // return (int)CurrentTask->GetTaskState();
    
    return 0; // Unknown state
}

// === MEMORY MANAGEMENT EXPORTS ===
extern "C" DOTNETSCRIPTING_API bool UE_IsHandleValid(void* Handle)
{
    return TypeConversion::IsValidUEObject(Handle);
}

extern "C" DOTNETSCRIPTING_API void UE_ReleaseHandle(void* Handle)
{
    // Optional: C# can hint that it's done with this handle
    // UE still manages the actual object lifetime
    // This is just for optimization/cleanup hints
    
    // For now, do nothing - UE GC will handle everything
    // Later: Could implement reference counting if needed
}
