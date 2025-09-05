# 🎮 Ped System Export Guide - Based on Your Game Implementation

Based on your actual Ped implementation in `Source/Game/Peds/`, here's how you can export your complete Ped system through the bridge.

## 🏗️ Your Current Ped Architecture

```cpp
APed : public ACharacter
├── UBaseEntity (Component) - Core entity system
│   ├── EntityID, EntityName, EntityType
│   ├── Health, MaxHealth, CurrentState
│   ├── WorldPosition, WorldRotation, WorldScale
│   └── bIsActive, bIsSpawned, bIsAlive
├── UPedInputComponent - Input handling & movement
├── UPedCharacterComponent - Visual appearance & meshes  
├── UPedDataComponent - Behavioral data & attributes
│   ├── FPedAttributes (Health, Armor, Speed, Stamina, Accuracy)
│   └── EPedType, relationship data
└── UPedAnimationController - Animation control
```

## 📋 Exportable Ped Data Structure

### 1. Create Comprehensive Interop Types

```cpp
// In GameExports.h

// Base Entity data (from your UBaseEntity)
struct FEntityData_Interop
{
    int32 EntityID;
    char EntityName[64];        // From UBaseEntity::EntityName
    int32 EntityType;           // EEntityType as int
    char DisplayName[64];       // From UBaseEntity::DisplayName
    
    // Spatial data
    FVector3f_Interop WorldPosition;
    FRotator_Interop WorldRotation;
    FVector3f_Interop WorldScale;
    
    // State data
    float MaxHealth;
    float CurrentHealth;
    int32 CurrentState;         // EEntityState as int
    bool bIsActive;
    bool bIsSpawned;
    bool bIsAlive;
    bool bIsInvulnerable;
    
    FEntityData_Interop()
        : EntityID(0), EntityType(0), MaxHealth(100.0f), CurrentHealth(100.0f),
          CurrentState(0), bIsActive(true), bIsSpawned(false), bIsAlive(true), bIsInvulnerable(false),
          WorldScale({1.0f, 1.0f, 1.0f})
    {
        memset(EntityName, 0, sizeof(EntityName));
        memset(DisplayName, 0, sizeof(DisplayName));
    }
};

// Ped attributes (from your FPedAttributes)
struct FPedAttributes_Interop
{
    char PedName[64];
    int32 PedType;              // EPedType as int
    
    // Health & Armor
    float Health;
    float MaxHealth;
    float Armor;
    float MaxArmor;
    
    // Movement
    float MovementSpeed;
    float RunSpeed;
    float SprintSpeed;
    
    // Stamina & Skills
    float Stamina;
    float MaxStamina;
    float Accuracy;
    
    // State flags
    bool bIsPlayerControlled;
    
    FPedAttributes_Interop()
        : PedType(0), Health(100.0f), MaxHealth(100.0f), Armor(0.0f), MaxArmor(100.0f),
          MovementSpeed(1.0f), RunSpeed(2.0f), SprintSpeed(3.0f),
          Stamina(100.0f), MaxStamina(100.0f), Accuracy(0.5f), bIsPlayerControlled(false)
    {
        memset(PedName, 0, sizeof(PedName));
    }
};

// Complete Ped data combining entity + ped-specific
struct FPedData_Interop
{
    FEntityData_Interop EntityData;        // Base entity data
    FPedAttributes_Interop PedAttributes;  // Ped-specific attributes
    char CharacterName[64];                 // From APed::CharacterName
    bool bIsPedInitialized;                 // From APed::bIsPedInitialized
    
    FPedData_Interop()
        : bIsPedInitialized(false)
    {
        memset(CharacterName, 0, sizeof(CharacterName));
    }
};

// Ped spawn configuration (from your FPedSpawnConfiguration)
struct FPedSpawnConfig_Interop
{
    char PedName[64];           // XML name like "PlayerNiko"
    char Variation[32];         // Character variation
    char UniqueActorName[64];   // Unique instance name
    FVector3f_Interop Position;
    FRotator_Interop Rotation;
    bool bPlayerControlled;
    
    FPedSpawnConfig_Interop()
        : bPlayerControlled(false)
    {
        memset(PedName, 0, sizeof(PedName));
        memset(Variation, 0, sizeof(Variation));
        memset(UniqueActorName, 0, sizeof(UniqueActorName));
    }
};
```

### 2. C++ Export Functions

```cpp
// In GameExports.h
extern "C" {
    // ═══════════════════════════════════════════════════════════════
    // PED SYSTEM - Based on your APed + UBaseEntity architecture
    // ═══════════════════════════════════════════════════════════════
    
    // Ped lifecycle (using your PedFactory)
    DOTNETSCRIPTING_API void* Ped_Spawn_Native(const FPedSpawnConfig_Interop* spawnConfig);
    DOTNETSCRIPTING_API bool Ped_Destroy_Native(void* pedHandle);
    DOTNETSCRIPTING_API bool Ped_IsValid_Native(void* pedHandle);
    DOTNETSCRIPTING_API bool Ped_IsInitialized_Native(void* pedHandle);
    
    // Complete Ped data access
    DOTNETSCRIPTING_API void Ped_GetData_Native(void* pedHandle, FPedData_Interop* outData);
    DOTNETSCRIPTING_API void Ped_SetData_Native(void* pedHandle, const FPedData_Interop* data);
    
    // Entity base properties (from UBaseEntity)
    DOTNETSCRIPTING_API void Ped_GetPosition_Native(void* pedHandle, FVector3f_Interop* outPosition);
    DOTNETSCRIPTING_API void Ped_SetPosition_Native(void* pedHandle, FVector3f_Interop position);
    DOTNETSCRIPTING_API void Ped_GetRotation_Native(void* pedHandle, FRotator_Interop* outRotation);
    DOTNETSCRIPTING_API void Ped_SetRotation_Native(void* pedHandle, FRotator_Interop rotation);
    
    // Health system (from UBaseEntity)
    DOTNETSCRIPTING_API float Ped_GetHealth_Native(void* pedHandle);
    DOTNETSCRIPTING_API void Ped_SetHealth_Native(void* pedHandle, float health);
    DOTNETSCRIPTING_API float Ped_GetMaxHealth_Native(void* pedHandle);
    DOTNETSCRIPTING_API void Ped_SetMaxHealth_Native(void* pedHandle, float maxHealth);
    DOTNETSCRIPTING_API float Ped_GetArmor_Native(void* pedHandle);
    DOTNETSCRIPTING_API void Ped_SetArmor_Native(void* pedHandle, float armor);
    
    // State management (from UBaseEntity)
    DOTNETSCRIPTING_API bool Ped_IsActive_Native(void* pedHandle);
    DOTNETSCRIPTING_API void Ped_SetActive_Native(void* pedHandle, bool bActive);
    DOTNETSCRIPTING_API bool Ped_IsAlive_Native(void* pedHandle);
    DOTNETSCRIPTING_API void Ped_SetAlive_Native(void* pedHandle, bool bAlive);
    DOTNETSCRIPTING_API int32 Ped_GetEntityState_Native(void* pedHandle);
    DOTNETSCRIPTING_API void Ped_SetEntityState_Native(void* pedHandle, int32 state);
    
    // Player control (from APed)
    DOTNETSCRIPTING_API bool Ped_IsPlayerControlled_Native(void* pedHandle);
    DOTNETSCRIPTING_API void Ped_SetPlayerControlled_Native(void* pedHandle, bool bControlled);
    DOTNETSCRIPTING_API bool Ped_IsPlayer_Native(void* pedHandle);
    DOTNETSCRIPTING_API bool Ped_IsNPC_Native(void* pedHandle);
    
    // Movement attributes (from FPedAttributes)
    DOTNETSCRIPTING_API float Ped_GetMovementSpeed_Native(void* pedHandle);
    DOTNETSCRIPTING_API void Ped_SetMovementSpeed_Native(void* pedHandle, float speed);
    DOTNETSCRIPTING_API float Ped_GetRunSpeed_Native(void* pedHandle);
    DOTNETSCRIPTING_API void Ped_SetRunSpeed_Native(void* pedHandle, float speed);
    DOTNETSCRIPTING_API float Ped_GetSprintSpeed_Native(void* pedHandle, float speed);
    
    // Stamina system (from FPedAttributes)
    DOTNETSCRIPTING_API float Ped_GetStamina_Native(void* pedHandle);
    DOTNETSCRIPTING_API void Ped_SetStamina_Native(void* pedHandle, float stamina);
    DOTNETSCRIPTING_API float Ped_GetMaxStamina_Native(void* pedHandle);
    DOTNETSCRIPTING_API void Ped_SetMaxStamina_Native(void* pedHandle, float maxStamina);
    
    // Character identity (from APed + UBaseEntity)
    DOTNETSCRIPTING_API void Ped_GetCharacterName_Native(void* pedHandle, char* outName, int32 maxLength);
    DOTNETSCRIPTING_API void Ped_SetCharacterName_Native(void* pedHandle, const char* name);
    DOTNETSCRIPTING_API int32 Ped_GetEntityID_Native(void* pedHandle);
    DOTNETSCRIPTING_API void Ped_GetEntityName_Native(void* pedHandle, char* outName, int32 maxLength);
}
```

### 3. C++ Implementation Example

```cpp
// In GameExports.cpp

extern "C" DOTNETSCRIPTING_API void* Ped_Spawn_Native(const FPedSpawnConfig_Interop* spawnConfig)
{
    if (!spawnConfig)
    {
        UE_LOG(LogTemp, Warning, TEXT("[MODDING] Ped_Spawn called with null config"));
        return nullptr;
    }

    UWorld* World = GetCurrentWorld();
    if (!World)
    {
        UE_LOG(LogTemp, Warning, TEXT("[MODDING] No valid world for Ped spawn"));
        return nullptr;
    }

    // Get your PedFactory (adapt to your actual factory access method)
    UPedFactory* PedFactory = GetPedFactory();
    if (!PedFactory)
    {
        UE_LOG(LogTemp, Warning, TEXT("[MODDING] PedFactory not available"));
        return nullptr;
    }

    // Convert interop data to your actual types
    FString PedName = FString(UTF8_TO_TCHAR(spawnConfig->PedName));
    FString Variation = FString(UTF8_TO_TCHAR(spawnConfig->Variation));
    FString UniqueActorName = FString(UTF8_TO_TCHAR(spawnConfig->UniqueActorName));
    FVector SpawnLocation = spawnConfig->Position.ToFVector();
    FRotator SpawnRotation = spawnConfig->Rotation.ToFRotator();

    // Create spawn configuration (adapt to your FPedSpawnConfiguration structure)
    FPedSpawnConfiguration SpawnConfiguration;
    // Fill in the configuration based on your actual struct...

    try
    {
        // Use your actual PedFactory spawn method
        APed* NewPed = PedFactory->SpawnPed(SpawnConfiguration, UniqueActorName);
        
        if (NewPed)
        {
            // Set player control if needed
            if (spawnConfig->bPlayerControlled)
            {
                NewPed->SetPlayerControlled(true);
            }

            UE_LOG(LogTemp, Log, TEXT("[MODDING] Successfully spawned Ped: %s"), *PedName);
            return NewPed;
        }
        else
        {
            UE_LOG(LogTemp, Warning, TEXT("[MODDING] Failed to spawn Ped: %s"), *PedName);
            return nullptr;
        }
    }
    catch (...)
    {
        UE_LOG(LogTemp, Error, TEXT("[MODDING] Exception during Ped spawn: %s"), *PedName);
        return nullptr;
    }
}

extern "C" DOTNETSCRIPTING_API void Ped_GetData_Native(void* pedHandle, FPedData_Interop* outData)
{
    if (!outData)
    {
        UE_LOG(LogTemp, Warning, TEXT("[MODDING] Ped_GetData called with null output"));
        return;
    }

    *outData = FPedData_Interop(); // Initialize

    if (!pedHandle)
    {
        UE_LOG(LogTemp, Warning, TEXT("[MODDING] Ped_GetData called with null handle"));
        return;
    }

    APed* Ped = static_cast<APed*>(pedHandle);
    if (!Ped || !IsValid(Ped))
    {
        UE_LOG(LogTemp, Warning, TEXT("[MODDING] Invalid Ped handle"));
        return;
    }

    // Get BaseEntity component data
    UBaseEntity* BaseEntity = Ped->GetBaseEntityComponent();
    if (BaseEntity)
    {
        // Fill entity data
        outData->EntityData.EntityID = BaseEntity->EntityID;
        FCStringAnsi::Strncpy(outData->EntityData.EntityName, TCHAR_TO_UTF8(*BaseEntity->EntityName), 63);
        FCStringAnsi::Strncpy(outData->EntityData.DisplayName, TCHAR_TO_UTF8(*BaseEntity->DisplayName), 63);
        outData->EntityData.EntityType = static_cast<int32>(BaseEntity->EntityType);
        
        // Spatial data
        outData->EntityData.WorldPosition = FVector3f_Interop(BaseEntity->WorldPosition);
        outData->EntityData.WorldRotation = FRotator_Interop(BaseEntity->WorldRotation);
        outData->EntityData.WorldScale = FVector3f_Interop(BaseEntity->WorldScale);
        
        // Health and state
        outData->EntityData.MaxHealth = BaseEntity->MaxHealth;
        outData->EntityData.CurrentHealth = BaseEntity->CurrentHealth;
        outData->EntityData.CurrentState = static_cast<int32>(BaseEntity->CurrentState);
        outData->EntityData.bIsActive = BaseEntity->bIsActive;
        outData->EntityData.bIsSpawned = BaseEntity->bIsSpawned;
        outData->EntityData.bIsAlive = BaseEntity->bIsAlive;
        outData->EntityData.bIsInvulnerable = BaseEntity->bIsInvulnerable;
    }

    // Get PedData component
    UPedDataComponent* PedDataComp = Ped->GetPedDataComponent();
    if (PedDataComp)
    {
        // Fill ped attributes (adapt to your actual PedDataComponent structure)
        const FPedAttributes& Attributes = PedDataComp->GetCurrentAttributes();
        
        FCStringAnsi::Strncpy(outData->PedAttributes.PedName, TCHAR_TO_UTF8(*Attributes.PedName), 63);
        outData->PedAttributes.PedType = static_cast<int32>(Attributes.PedType);
        outData->PedAttributes.Health = Attributes.Health;
        outData->PedAttributes.MaxHealth = Attributes.MaxHealth;
        outData->PedAttributes.Armor = Attributes.Armor;
        outData->PedAttributes.MaxArmor = Attributes.MaxArmor;
        outData->PedAttributes.MovementSpeed = Attributes.MovementSpeed;
        outData->PedAttributes.RunSpeed = Attributes.RunSpeed;
        outData->PedAttributes.SprintSpeed = Attributes.SprintSpeed;
        outData->PedAttributes.Stamina = Attributes.Stamina;
        outData->PedAttributes.MaxStamina = Attributes.MaxStamina;
        outData->PedAttributes.Accuracy = Attributes.Accuracy;
    }

    // Fill APed-specific data
    FCStringAnsi::Strncpy(outData->CharacterName, TCHAR_TO_UTF8(*Ped->GetCharacterName()), 63);
    outData->bIsPedInitialized = Ped->bIsPedInitialized;
    outData->PedAttributes.bIsPlayerControlled = Ped->GetIsPlayerControlled();

    UE_LOG(LogTemp, VeryVerbose, TEXT("[MODDING] Retrieved Ped data for: %s"), *Ped->GetCharacterName());
}

extern "C" DOTNETSCRIPTING_API float Ped_GetHealth_Native(void* pedHandle)
{
    if (!pedHandle) return 0.0f;
    
    APed* Ped = static_cast<APed*>(pedHandle);
    if (!Ped || !IsValid(Ped)) return 0.0f;
    
    UBaseEntity* BaseEntity = Ped->GetBaseEntityComponent();
    return BaseEntity ? BaseEntity->CurrentHealth : 0.0f;
}

extern "C" DOTNETSCRIPTING_API void Ped_SetPlayerControlled_Native(void* pedHandle, bool bControlled)
{
    if (!pedHandle) return;
    
    APed* Ped = static_cast<APed*>(pedHandle);
    if (!Ped || !IsValid(Ped)) return;
    
    Ped->SetPlayerControlled(bControlled);
    UE_LOG(LogTemp, Log, TEXT("[MODDING] Set player control for %s: %s"), 
           *Ped->GetCharacterName(), bControlled ? TEXT("true") : TEXT("false"));
}
```

### 4. C# User-Friendly API

```csharp
/// <summary>
/// Represents a Ped in your game world, wrapping your APed + UBaseEntity system
/// </summary>
public class Ped : Entity  // Inherits from Entity base class
{
    // Ped-specific properties (from FPedAttributes)
    public string PedName { get; set; } = "";
    public PedType PedType { get; set; }
    public float Armor { get; set; }
    public float MaxArmor { get; set; }
    public float MovementSpeed { get; set; } = 1.0f;
    public float RunSpeed { get; set; } = 2.0f;
    public float SprintSpeed { get; set; } = 3.0f;
    public float Stamina { get; set; } = 100.0f;
    public float MaxStamina { get; set; } = 100.0f;
    public float Accuracy { get; set; } = 0.5f;
    
    // APed properties
    public string CharacterName { get; set; } = "";
    public bool IsPlayerControlled { get; set; }
    public bool IsPedInitialized { get; private set; }
    
    // Convenience properties
    public bool IsPlayer => IsPlayerControlled;
    public bool IsNPC => !IsPlayerControlled;
    
    // Create from native data
    internal static Ped FromInterop(FPedData_Interop data, IntPtr handle)
    {
        return new Ped
        {
            // Entity base properties
            ID = data.EntityData.EntityID,
            ModelID = data.EntityData.EntityType,  // Using EntityType as ModelID
            Name = data.EntityData.EntityName,
            Position = data.EntityData.WorldPosition.FromInterop(),
            Rotation = data.EntityData.WorldRotation.FromInterop(),
            Scale = data.EntityData.WorldScale.FromInterop(),
            Health = data.EntityData.CurrentHealth,
            MaxHealth = data.EntityData.MaxHealth,
            IsActive = data.EntityData.bIsActive,
            IsSpawned = data.EntityData.bIsSpawned,
            IsAlive = data.EntityData.bIsAlive,
            Handle = handle,
            
            // Ped-specific properties
            PedName = data.PedAttributes.PedName,
            PedType = (PedType)data.PedAttributes.PedType,
            Armor = data.PedAttributes.Armor,
            MaxArmor = data.PedAttributes.MaxArmor,
            MovementSpeed = data.PedAttributes.MovementSpeed,
            RunSpeed = data.PedAttributes.RunSpeed,
            SprintSpeed = data.PedAttributes.SprintSpeed,
            Stamina = data.PedAttributes.Stamina,
            MaxStamina = data.PedAttributes.MaxStamina,
            Accuracy = data.PedAttributes.Accuracy,
            CharacterName = data.CharacterName,
            IsPlayerControlled = data.PedAttributes.bIsPlayerControlled,
            IsPedInitialized = data.bIsPedInitialized
        };
    }
}

/// <summary>
/// Configuration for spawning a new Ped
/// </summary>
public class PedSpawnConfig
{
    public string PedName { get; set; } = "";           // XML name like "PlayerNiko"
    public string Variation { get; set; } = "Default";  // Character variation
    public string UniqueActorName { get; set; } = "";   // Unique instance name
    public FVector3f Position { get; set; }
    public FRotator Rotation { get; set; }
    public bool PlayerControlled { get; set; } = false;
}

/// <summary>
/// Ped management API - maps directly to your APed + UBaseEntity system
/// </summary>
public static class PedAPI
{
    /// <summary>
    /// Spawns a Ped using your PedFactory system
    /// </summary>
    public static Ped? SpawnPed(PedSpawnConfig config)
    {
        var interopConfig = new FPedSpawnConfig_Interop
        {
            PedName = config.PedName,
            Variation = config.Variation,
            UniqueActorName = config.UniqueActorName,
            Position = config.Position.ToInterop(),
            Rotation = config.Rotation.ToInterop(),
            bPlayerControlled = config.PlayerControlled
        };
        
        IntPtr handle = GameImports.Ped_Spawn_Native(ref interopConfig);
        if (handle == IntPtr.Zero) return null;
        
        GameImports.Ped_GetData_Native(handle, out var data);
        return Ped.FromInterop(data, handle);
    }
    
    /// <summary>
    /// Spawns the PlayerNiko character (convenience method)
    /// </summary>
    public static Ped? SpawnPlayerNiko(FVector3f position, FRotator? rotation = null)
    {
        return SpawnPed(new PedSpawnConfig
        {
            PedName = "PlayerNiko",
            Variation = "Default",
            UniqueActorName = $"PlayerNiko_{DateTime.Now.Ticks}",
            Position = position,
            Rotation = rotation ?? new FRotator(),
            PlayerControlled = true
        });
    }
    
    /// <summary>
    /// Gets complete Ped data from the native side
    /// </summary>
    public static void RefreshPedData(Ped ped)
    {
        if (ped?.Handle == IntPtr.Zero) return;
        
        GameImports.Ped_GetData_Native(ped.Handle, out var data);
        var updated = Ped.FromInterop(data, ped.Handle);
        
        // Update all properties...
        // (Copy all properties from updated to ped)
    }
    
    /// <summary>
    /// Sets player control for the Ped
    /// </summary>
    public static void SetPlayerControlled(Ped ped, bool controlled)
    {
        if (ped?.Handle == IntPtr.Zero) return;
        GameImports.Ped_SetPlayerControlled_Native(ped.Handle, controlled);
        ped.IsPlayerControlled = controlled;
    }
    
    /// <summary>
    /// Sets Ped movement speed
    /// </summary>
    public static void SetMovementSpeed(Ped ped, float speed)
    {
        if (ped?.Handle == IntPtr.Zero) return;
        GameImports.Ped_SetMovementSpeed_Native(ped.Handle, speed);
        ped.MovementSpeed = speed;
    }
    
    // ... more API methods for all the other functions
}
```

## 🎯 Usage Example

```csharp
// Modder's code - clean and simple!
public class MyPedMod
{
    public void SpawnSomeCharacters()
    {
        // Spawn PlayerNiko
        var niko = PedAPI.SpawnPlayerNiko(new FVector3f(0, 0, 100));
        if (niko != null)
        {
            Console.WriteLine($"Spawned {niko.CharacterName} - Health: {niko.Health}");
            Console.WriteLine($"Entity ID: {niko.ID}, Type: {niko.PedType}");
            
            // Modify attributes
            PedAPI.SetMovementSpeed(niko, 2.5f);
            PedAPI.SetPlayerControlled(niko, true);
        }
        
        // Spawn an NPC
        var npc = PedAPI.SpawnPed(new PedSpawnConfig
        {
            PedName = "Cop",
            Variation = "Default",
            Position = new FVector3f(100, 100, 100),
            PlayerControlled = false
        });
        
        if (npc != null)
        {
            Console.WriteLine($"Spawned NPC: {npc.PedName}");
        }
    }
}
```

## ✅ Summary

**Yes, your Ped system can be fully exported!** The key is:

1. **Map your component architecture** (UBaseEntity, UPedDataComponent, etc.) to interop structs
2. **Export comprehensive data access** functions that combine all component data
3. **Provide both granular and bulk operations** for flexibility
4. **Hide complexity** behind clean C# APIs

Your current architecture is perfect for modding because:
- ✅ **Modular components** are easy to export individually
- ✅ **UBaseEntity provides common entity functionality** 
- ✅ **Clear separation** between data and behavior
- ✅ **Factory pattern** makes spawning straightforward to export

This approach gives modders full access to your rich Ped system while keeping the API clean and typed! 🎮
