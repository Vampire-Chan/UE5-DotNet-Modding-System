#include "UnrealEngineAPI.h"
#include "Engine/World.h"
#include "Engine/GameInstance.h"
#include "GameFramework/GameModeBase.h"
#include "GameFramework/GameStateBase.h"
#include "GameFramework/PlayerController.h"
#include "GameFramework/Pawn.h"
#include "Components/ActorComponent.h"
#include "HAL/FileManager.h"
#include "Misc/FileHelper.h"
#include "Kismet/GameplayStatics.h"
#include "Kismet/KismetSystemLibrary.h"

// Forward declarations for game-specific classes
// We'll include these when the system builds
class UPedFactory;
class APed;
class UTaskManager;
class UBaseTask;
class UOneShotTask;
class UComplexTask;
class UWildComplexTask;
class UTaskFactory;

// === TYPE CONVERSION FUNCTIONS ===

namespace UnrealEngineTypeConversion
{
    // String Conversions
    FString ConvertToFString(const char* cstr)
    {
        if (!cstr) return FString();
        return FString(UTF8_TO_TCHAR(cstr));
    }

    // Note: This returns a static buffer - not thread safe but fine for our use case
    const char* ConvertFromFString(const FString& fstr)
    {
        static TArray<char> ConversionBuffer;
        ConversionBuffer.Empty();
        
        FTCHARToUTF8 Converter(*fstr);
        int32 Length = Converter.Length();
        ConversionBuffer.AddUninitialized(Length + 1);
        FMemory::Memcpy(ConversionBuffer.GetData(), Converter.Get(), Length);
        ConversionBuffer[Length] = '\0';
        
        return ConversionBuffer.GetData();
    }

    // Vector Conversions
    FVector ConvertToFVector(float x, float y, float z)
    {
        return FVector(x, y, z);
    }

    void ConvertFromFVector(const FVector& vec, float& x, float& y, float& z)
    {
        x = vec.X;
        y = vec.Y;
        z = vec.Z;
    }

    // Rotator Conversions (Note: UE uses Pitch, Yaw, Roll order in constructor!)
    FRotator ConvertToFRotator(float roll, float pitch, float yaw)
    {
        return FRotator(pitch, yaw, roll); // UE constructor order: Pitch, Yaw, Roll
    }

    void ConvertFromFRotator(const FRotator& rot, float& roll, float& pitch, float& yaw)
    {
        roll = rot.Roll;
        pitch = rot.Pitch;
        yaw = rot.Yaw;
    }

    // Transform Conversions
    FTransform ConvertToFTransform(float x, float y, float z, float roll, float pitch, float yaw, float scaleX = 1.0f, float scaleY = 1.0f, float scaleZ = 1.0f)
    {
        FVector Location = ConvertToFVector(x, y, z);
        FRotator Rotation = ConvertToFRotator(roll, pitch, yaw);
        FVector Scale = ConvertToFVector(scaleX, scaleY, scaleZ);
        return FTransform(Rotation, Location, Scale);
    }

    // Array Conversions Helper (for future use)
    template<typename T>
    TArray<T> ConvertToTArray(const T* items, int32 count)
    {
        TArray<T> Result;
        Result.Reserve(count);
        for (int32 i = 0; i < count; ++i)
        {
            Result.Add(items[i]);
        }
        return Result;
    }
}

// === LOGGING FUNCTIONS ===
void FUnrealEngineAPI::LogInfo(const char* Category, const char* Message)
{
    FString CategoryStr = UnrealEngineTypeConversion::ConvertToFString(Category);
    FString MessageStr = UnrealEngineTypeConversion::ConvertToFString(Message);
    UE_LOG(LogTemp, Log, TEXT("[%s] %s"), *CategoryStr, *MessageStr);
}

void FUnrealEngineAPI::LogWarning(const char* Category, const char* Message)
{
    FString CategoryStr = UnrealEngineTypeConversion::ConvertToFString(Category);
    FString MessageStr = UnrealEngineTypeConversion::ConvertToFString(Message);
    UE_LOG(LogTemp, Warning, TEXT("[%s] %s"), *CategoryStr, *MessageStr);
}

void FUnrealEngineAPI::LogError(const char* Category, const char* Message)
{
    FString CategoryStr = UnrealEngineTypeConversion::ConvertToFString(Category);
    FString MessageStr = UnrealEngineTypeConversion::ConvertToFString(Message);
    UE_LOG(LogTemp, Error, TEXT("[%s] %s"), *CategoryStr, *MessageStr);
}

// === WORLD/ACTOR FUNCTIONS ===
void* FUnrealEngineAPI::SpawnActorByName(const char* ActorClassName, float X, float Y, float Z, float Roll, float Pitch, float Yaw)
{
    UWorld* World = GetCurrentWorld();
    if (!World)
    {
        LogError("UnrealEngineAPI", "No valid world context for spawning actor");
        return nullptr;
    }

    // Use proper type conversion
    FString ClassNameStr = UnrealEngineTypeConversion::ConvertToFString(ActorClassName);
    UClass* ActorClass = FindActorClass(ClassNameStr);
    
    if (!ActorClass)
    {
        FString ErrorMsg = FString::Printf(TEXT("Actor class not found: %s"), *ClassNameStr);
        LogError("UnrealEngineAPI", UnrealEngineTypeConversion::ConvertFromFString(ErrorMsg));
        return nullptr;
    }

    // Use proper type conversion for location and rotation
    FVector Location = UnrealEngineTypeConversion::ConvertToFVector(X, Y, Z);
    FRotator Rotation = UnrealEngineTypeConversion::ConvertToFRotator(Roll, Pitch, Yaw);
    
    AActor* SpawnedActor = World->SpawnActor<AActor>(ActorClass, Location, Rotation);
    
    if (SpawnedActor)
    {
        FString SuccessMsg = FString::Printf(TEXT("Successfully spawned actor: %s"), *ClassNameStr);
        LogInfo("UnrealEngineAPI", UnrealEngineTypeConversion::ConvertFromFString(SuccessMsg));
        return static_cast<void*>(SpawnedActor);
    }
    
    return nullptr;
}

void FUnrealEngineAPI::DestroyActor(void* ActorPtr)
{
    AActor* Actor = GetActorFromPtr(ActorPtr);
    if (Actor && IsValid(Actor))
    {
        Actor->Destroy();
        LogInfo("UnrealEngineAPI", "Actor destroyed");
    }
}

bool FUnrealEngineAPI::IsActorValid(void* ActorPtr)
{
    AActor* Actor = GetActorFromPtr(ActorPtr);
    return Actor && IsValid(Actor);
}

// === TRANSFORM FUNCTIONS ===
void FUnrealEngineAPI::SetActorLocation(void* ActorPtr, float X, float Y, float Z)
{
    AActor* Actor = GetActorFromPtr(ActorPtr);
    if (Actor && IsValid(Actor))
    {
        Actor->SetActorLocation(FVector(X, Y, Z));
    }
}

void FUnrealEngineAPI::GetActorLocation(void* ActorPtr, float& OutX, float& OutY, float& OutZ)
{
    AActor* Actor = GetActorFromPtr(ActorPtr);
    if (Actor && IsValid(Actor))
    {
        FVector Location = Actor->GetActorLocation();
        OutX = Location.X;
        OutY = Location.Y;
        OutZ = Location.Z;
    }
    else
    {
        OutX = OutY = OutZ = 0.0f;
    }
}

void FUnrealEngineAPI::SetActorRotation(void* ActorPtr, float Roll, float Pitch, float Yaw)
{
    AActor* Actor = GetActorFromPtr(ActorPtr);
    if (Actor && IsValid(Actor))
    {
        Actor->SetActorRotation(FRotator(Pitch, Yaw, Roll));
    }
}

void FUnrealEngineAPI::GetActorRotation(void* ActorPtr, float& OutRoll, float& OutPitch, float& OutYaw)
{
    AActor* Actor = GetActorFromPtr(ActorPtr);
    if (Actor && IsValid(Actor))
    {
        FRotator Rotation = Actor->GetActorRotation();
        OutRoll = Rotation.Roll;
        OutPitch = Rotation.Pitch;
        OutYaw = Rotation.Yaw;
    }
    else
    {
        OutRoll = OutPitch = OutYaw = 0.0f;
    }
}

// === COMPONENT FUNCTIONS ===
void* FUnrealEngineAPI::GetActorComponent(void* ActorPtr, const char* ComponentClassName)
{
    AActor* Actor = GetActorFromPtr(ActorPtr);
    if (!Actor || !IsValid(Actor))
    {
        return nullptr;
    }

    FString ClassNameStr = UTF8_TO_TCHAR(ComponentClassName);
    
    // Try to find the component class
    UClass* ComponentClass = FindObject<UClass>(ANY_PACKAGE, *ClassNameStr);
    if (!ComponentClass)
    {
        LogError("UnrealEngineAPI", TCHAR_TO_UTF8(*FString::Printf(TEXT("Component class not found: %s"), *ClassNameStr)));
        return nullptr;
    }

    UActorComponent* Component = Actor->GetComponentByClass(ComponentClass);
    return static_cast<void*>(Component);
}

void* FUnrealEngineAPI::AddComponentToActor(void* ActorPtr, const char* ComponentClassName)
{
    AActor* Actor = GetActorFromPtr(ActorPtr);
    if (!Actor || !IsValid(Actor))
    {
        return nullptr;
    }

    FString ClassNameStr = UTF8_TO_TCHAR(ComponentClassName);
    UClass* ComponentClass = FindObject<UClass>(ANY_PACKAGE, *ClassNameStr);
    
    if (!ComponentClass || !ComponentClass->IsChildOf<UActorComponent>())
    {
        LogError("UnrealEngineAPI", TCHAR_TO_UTF8(*FString::Printf(TEXT("Invalid component class: %s"), *ClassNameStr)));
        return nullptr;
    }

    UActorComponent* NewComponent = NewObject<UActorComponent>(Actor, ComponentClass);
    if (NewComponent)
    {
        Actor->AddInstanceComponent(NewComponent);
        NewComponent->RegisterComponent();
        LogInfo("UnrealEngineAPI", TCHAR_TO_UTF8(*FString::Printf(TEXT("Added component: %s"), *ClassNameStr)));
        return static_cast<void*>(NewComponent);
    }

    return nullptr;
}

// === GAME STATE FUNCTIONS ===
void* FUnrealEngineAPI::GetGameMode()
{
    UWorld* World = GetCurrentWorld();
    if (World)
    {
        return static_cast<void*>(World->GetAuthGameMode());
    }
    return nullptr;
}

void* FUnrealEngineAPI::GetGameState()
{
    UWorld* World = GetCurrentWorld();
    if (World)
    {
        return static_cast<void*>(World->GetGameState());
    }
    return nullptr;
}

void* FUnrealEngineAPI::GetPlayerController(int32 PlayerIndex)
{
    UWorld* World = GetCurrentWorld();
    if (World)
    {
        return static_cast<void*>(UGameplayStatics::GetPlayerController(World, PlayerIndex));
    }
    return nullptr;
}

void* FUnrealEngineAPI::GetPlayerPawn(int32 PlayerIndex)
{
    UWorld* World = GetCurrentWorld();
    if (World)
    {
        return static_cast<void*>(UGameplayStatics::GetPlayerPawn(World, PlayerIndex));
    }
    return nullptr;
}

// === UTILITY FUNCTIONS ===
float FUnrealEngineAPI::GetDeltaTime()
{
    UWorld* World = GetCurrentWorld();
    if (World)
    {
        return World->GetDeltaSeconds();
    }
    return 0.0f;
}

float FUnrealEngineAPI::GetGameTime()
{
    UWorld* World = GetCurrentWorld();
    if (World)
    {
        return World->GetTimeSeconds();
    }
    return 0.0f;
}

void FUnrealEngineAPI::QuitGame()
{
    UWorld* World = GetCurrentWorld();
    if (World)
    {
        UKismetSystemLibrary::QuitGame(World, nullptr, EQuitPreference::Quit, false);
    }
}

void FUnrealEngineAPI::SetGamePaused(bool bPaused)
{
    UWorld* World = GetCurrentWorld();
    if (World)
    {
        UGameplayStatics::SetGamePaused(World, bPaused);
    }
}

// === FILE I/O FUNCTIONS ===
bool FUnrealEngineAPI::SaveStringToFile(const char* FilePath, const char* Content)
{
    FString FilePathStr = UTF8_TO_TCHAR(FilePath);
    FString ContentStr = UTF8_TO_TCHAR(Content);
    
    bool bSuccess = FFileHelper::SaveStringToFile(ContentStr, *FilePathStr);
    
    if (bSuccess)
    {
        LogInfo("UnrealEngineAPI", TCHAR_TO_UTF8(*FString::Printf(TEXT("File saved: %s"), *FilePathStr)));
    }
    else
    {
        LogError("UnrealEngineAPI", TCHAR_TO_UTF8(*FString::Printf(TEXT("Failed to save file: %s"), *FilePathStr)));
    }
    
    return bSuccess;
}

bool FUnrealEngineAPI::LoadStringFromFile(const char* FilePath, char* OutBuffer, int32 BufferSize)
{
    FString FilePathStr = UTF8_TO_TCHAR(FilePath);
    FString LoadedContent;
    
    bool bSuccess = FFileHelper::LoadFileToString(LoadedContent, *FilePathStr);
    
    if (bSuccess && OutBuffer && BufferSize > 0)
    {
        FTCHARToUTF8 Converter(*LoadedContent);
        int32 SourceLength = Converter.Length();
        int32 CopyLength = FMath::Min(SourceLength, BufferSize - 1);
        
        FMemory::Memcpy(OutBuffer, Converter.Get(), CopyLength);
        OutBuffer[CopyLength] = '\0';
        
        LogInfo("UnrealEngineAPI", TCHAR_TO_UTF8(*FString::Printf(TEXT("File loaded: %s"), *FilePathStr)));
    }
    else if (!bSuccess)
    {
        LogError("UnrealEngineAPI", TCHAR_TO_UTF8(*FString::Printf(TEXT("Failed to load file: %s"), *FilePathStr)));
    }
    
    return bSuccess;
}

// === MATH HELPER FUNCTIONS ===
float FUnrealEngineAPI::GetDistanceBetweenPoints(float X1, float Y1, float Z1, float X2, float Y2, float Z2)
{
    FVector Point1(X1, Y1, Z1);
    FVector Point2(X2, Y2, Z2);
    return FVector::Dist(Point1, Point2);
}

void FUnrealEngineAPI::GetRandomPointInSphere(float CenterX, float CenterY, float CenterZ, float Radius, float& OutX, float& OutY, float& OutZ)
{
    FVector Center(CenterX, CenterY, CenterZ);
    FVector RandomPoint = Center + FMath::VRand() * Radius;
    
    OutX = RandomPoint.X;
    OutY = RandomPoint.Y;
    OutZ = RandomPoint.Z;
}

// === PRIVATE HELPER FUNCTIONS ===
UWorld* FUnrealEngineAPI::GetCurrentWorld()
{
    if (GEngine)
    {
        for (const FWorldContext& Context : GEngine->GetWorldContexts())
        {
            if (Context.World() && Context.World()->IsGameWorld())
            {
                return Context.World();
            }
        }
    }
    return nullptr;
}

UClass* FUnrealEngineAPI::FindActorClass(const FString& ClassName)
{
    // Try to find the class by name
    UClass* FoundClass = FindObject<UClass>(ANY_PACKAGE, *ClassName);
    
    if (!FoundClass)
    {
        // Try with common prefixes
        TArray<FString> Prefixes = { TEXT("BP_"), TEXT("A"), TEXT("U") };
        for (const FString& Prefix : Prefixes)
        {
            FString PrefixedName = Prefix + ClassName;
            FoundClass = FindObject<UClass>(ANY_PACKAGE, *PrefixedName);
            if (FoundClass)
            {
                break;
            }
        }
    }
    
    // Make sure it's actually an Actor class
    if (FoundClass && !FoundClass->IsChildOf<AActor>())
    {
        return nullptr;
    }
    
    return FoundClass;
}

AActor* FUnrealEngineAPI::GetActorFromPtr(void* ActorPtr)
{
    return static_cast<AActor*>(ActorPtr);
}

// === PED FACTORY API IMPLEMENTATIONS ===

void* FUnrealEngineAPI::GetPedFactory()
{
    LogWarning("PedFactory", "GetPedFactory not yet implemented - needs game module link");
    return nullptr;
}

void* FUnrealEngineAPI::SpawnPed(const char* CharacterName, const char* VariationName, float X, float Y, float Z, float Roll, float Pitch, float Yaw, bool bAIEnabled, bool bPlayerControlled)
{
    LogWarning("PedFactory", "SpawnPed not yet implemented - needs game module link");
    return nullptr;
}

bool FUnrealEngineAPI::PossessPed(void* PedPtr, void* PlayerControllerPtr)
{
    LogWarning("PedFactory", "PossessPed not yet implemented - needs game module link");
    return false;
}

bool FUnrealEngineAPI::UnpossessPed(void* PlayerControllerPtr)
{
    LogWarning("PedFactory", "UnpossessPed not yet implemented - needs game module link");
    return false;
}

void FUnrealEngineAPI::SetPedAIEnabled(void* PedPtr, bool bEnabled)
{
    LogWarning("PedFactory", "SetPedAIEnabled not yet implemented - needs game module link");
}

void* FUnrealEngineAPI::FindPedByName(const char* PedName)
{
    LogWarning("PedFactory", "FindPedByName not yet implemented - needs game module link");
    return nullptr;
}

// === TASK SYSTEM API IMPLEMENTATIONS ===

// Global task handle counter and storage
static int32 GTaskHandleCounter = 1;

int32 FUnrealEngineAPI::CreateOneShotTask(const char* TaskName, int32 Priority)
{
    LogWarning("TaskSystem", "CreateOneShotTask not yet implemented - needs game module link");
    return 0;
}

int32 FUnrealEngineAPI::CreateComplexTask(const char* TaskName, int32 Priority, const char** SubTaskNames, int32 SubTaskCount)
{
    LogWarning("TaskSystem", "CreateComplexTask not yet implemented - needs game module link");
    return 0;
}

int32 FUnrealEngineAPI::CreateWildComplexTask(const char* TaskName, int32 Priority, bool bAdaptive)
{
    LogWarning("TaskSystem", "CreateWildComplexTask not yet implemented - needs game module link");
    return 0;
}

bool FUnrealEngineAPI::AssignTaskToPed(void* PedPtr, int32 TaskHandle)
{
    LogWarning("TaskSystem", "AssignTaskToPed not yet implemented - needs game module link");
    return false;
}

bool FUnrealEngineAPI::RemoveTaskFromPed(void* PedPtr, int32 TaskHandle)
{
    LogWarning("TaskSystem", "RemoveTaskFromPed not yet implemented - needs game module link");
    return false;
}

void FUnrealEngineAPI::ClearAllTasksFromPed(void* PedPtr)
{
    LogWarning("TaskSystem", "ClearAllTasksFromPed not yet implemented - needs game module link");
}

bool FUnrealEngineAPI::InterruptCurrentTask(void* PedPtr)
{
    LogWarning("TaskSystem", "InterruptCurrentTask not yet implemented - needs game module link");
    return false;
}

int32 FUnrealEngineAPI::GetTaskState(int32 TaskHandle)
{
    LogWarning("TaskSystem", "GetTaskState not yet implemented - needs game module link");
    return -1;
}

bool FUnrealEngineAPI::IsTaskRunning(int32 TaskHandle)
{
    return GetTaskState(TaskHandle) == 1;
}

bool FUnrealEngineAPI::IsTaskCompleted(int32 TaskHandle)
{
    return GetTaskState(TaskHandle) == 2;
}

const char* FUnrealEngineAPI::GetTaskName(int32 TaskHandle)
{
    LogWarning("TaskSystem", "GetTaskName not yet implemented - needs game module link");
    return "";
}

int32 FUnrealEngineAPI::GetTaskPriority(int32 TaskHandle)
{
    LogWarning("TaskSystem", "GetTaskPriority not yet implemented - needs game module link");
    return -1;
}

int32 FUnrealEngineAPI::GetActiveTaskCount(void* PedPtr)
{
    LogWarning("TaskSystem", "GetActiveTaskCount not yet implemented - needs game module link");
    return 0;
}

int32 FUnrealEngineAPI::GetCurrentTask(void* PedPtr)
{
    LogWarning("TaskSystem", "GetCurrentTask not yet implemented - needs game module link");
    return 0;
}

void FUnrealEngineAPI::GetAllActiveTasks(void* PedPtr, int32* OutTaskHandles, int32& OutCount)
{
    LogWarning("TaskSystem", "GetAllActiveTasks not yet implemented - needs game module link");
    OutCount = 0;
}

void* FUnrealEngineAPI::GetTaskManager(void* PedPtr)
{
    LogWarning("TaskSystem", "GetTaskManager not yet implemented - needs game module link");
    return nullptr;
}

// === PED CORE API IMPLEMENTATIONS ===

const char* FUnrealEngineAPI::GetPedCharacterName(void* PedPtr)
{
    LogWarning("PedCore", "GetPedCharacterName not yet implemented - needs game module link");
    return "";
}

bool FUnrealEngineAPI::IsPedPlayerControlled(void* PedPtr)
{
    LogWarning("PedCore", "IsPedPlayerControlled not yet implemented - needs game module link");
    return false;
}

bool FUnrealEngineAPI::IsPedAIEnabled(void* PedPtr)
{
    LogWarning("PedCore", "IsPedAIEnabled not yet implemented - needs game module link");
    return false;
}

void FUnrealEngineAPI::SetPedPlayerControlled(void* PedPtr, bool bPlayerControlled)
{
    LogWarning("PedCore", "SetPedPlayerControlled not yet implemented - needs game module link");
}

void FUnrealEngineAPI::GetAllPedsInWorld(void** OutPedPtrs, int32& OutCount)
{
    LogWarning("PedCore", "GetAllPedsInWorld not yet implemented - needs game module link");
    OutCount = 0;
}

float FUnrealEngineAPI::GetDistanceBetweenPeds(void* Ped1Ptr, void* Ped2Ptr)
{
    LogWarning("PedCore", "GetDistanceBetweenPeds not yet implemented - needs game module link");
    return -1.0f;
}
