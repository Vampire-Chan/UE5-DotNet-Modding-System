# DotNetScripting Bridge/Wrapper API Extension Guide

This guide explains how to add new functionality to the C++/C# bridge system for the DotNetScripting plugin.

## 🎯 Overview

The bridge follows a simple pattern:
1. **C++ Plugin** exports functions with `extern "C"` 
2. **C# Wrapper** imports them with `[DllImport]` (internal/hidden)
3. **C# API** provides user-friendly wrappers (public interface)

## 🏗️ Build Plan Strategy

### Recommended Build Order:
1. **UE Game + Plugin** (built together via UBT)
   ```bash
   D:/Applications/UE_5.6/Engine/Build/BatchFiles/Build.bat GameEditor Win64 Development -project="D:/UnrealProjs/Game/Game.uproject"
   ```
   - Builds both your game EXE and the DotNetScripting plugin DLL
   - Output: `Game.exe` + `UnrealEditor-DotNetScripting.dll`

2. **C# Bridge/Wrapper Library** (separate dotnet build)
   ```bash
   dotnet build ModdingTemplate/GameModding/GameModding.csproj
   ```
   - Builds the GameModding.dll with all DllImports and user APIs
   - Output: `GameModding.dll` in Binaries/Win64/DotNet/

3. **User Mods** (reference GameModding.dll)
   ```bash
   dotnet build ModdingTemplate/Examples/YourMod/YourMod.csproj
   ```

### Why This Order?
- **UBT handles dependencies** between game and plugin automatically
- **C# wrapper** needs the plugin DLL to exist for runtime
- **User mods** reference the wrapper library, not the plugin directly

## 📁 File Structure

```
Plugins/DotNetScripting/Source/DotNetScripting/
├── Public/GameExports.h          # C++ function declarations
├── Private/GameExports.cpp       # C++ function implementations
└── ...

ModdingTemplate/GameModding/
├── GameImports.cs                # C# DLL imports
├── GameAPI.cs                    # User-friendly C# API
├── TypeConversions.cs            # Type marshaling utilities
└── ...
```

## 🔧 Step-by-Step Process

### Step 1: Define Interop Types (if needed)

If your function uses UE types, create interop structs in `GameExports.h`:

```cpp
// Example: Adding a new UE type for interop
struct FTransform_Interop
{
    FVector3f_Interop Translation;
    FRotator_Interop Rotation;
    FVector3f_Interop Scale;
    
    FTransform_Interop() : Scale({1.0f, 1.0f, 1.0f}) {}
    FTransform_Interop(const FTransform& T) 
        : Translation(T.GetLocation()), Rotation(T.GetRotation().Rotator()), Scale(T.GetScale3D()) {}
    
    FTransform ToFTransform() const {
        return FTransform(Rotation.ToFRotator(), Translation.ToFVector(), Scale.ToFVector());
    }
};
```

### Step 2: Add C++ Export Declaration

In `GameExports.h`, add your function declaration inside the `extern "C"` block:

```cpp
extern "C" {
    // ═══════════════════════════════════════════════════════════════
    // YOUR NEW CATEGORY - Brief description
    // ═══════════════════════════════════════════════════════════════
    
    // Individual function docs
    DOTNETSCRIPTING_API void YourFunction_Native(void* handle, float param);
    DOTNETSCRIPTING_API bool YourQuery_Native(int id, SomeStruct_Interop* outResult);
}
```

### Step 3: Implement C++ Export Function

In `GameExports.cpp`, implement the function:

```cpp
extern "C" DOTNETSCRIPTING_API void YourFunction_Native(void* handle, float param)
{
    if (!handle)
    {
        UE_LOG(LogTemp, Warning, TEXT("[MODDING] YourFunction called with null handle"));
        return;
    }

    // TODO: Implement actual functionality
    UE_LOG(LogTemp, Log, TEXT("[MODDING] YourFunction called with param: %f"), param);
    
    // Example UE code:
    // ACoolActor* Actor = static_cast<ACoolActor*>(handle);
    // Actor->DoSomething(param);
}

extern "C" DOTNETSCRIPTING_API bool YourQuery_Native(int id, SomeStruct_Interop* outResult)
{
    if (!outResult)
    {
        UE_LOG(LogTemp, Warning, TEXT("[MODDING] YourQuery called with null output parameter"));
        return false;
    }

    // Initialize output
    *outResult = SomeStruct_Interop();

    // TODO: Implement query logic
    // if (SomeCondition(id)) {
    //     *outResult = SomeStruct_Interop(GetData(id));
    //     return true;
    // }
    
    return false;
}
```

### Step 4: Add C# DLL Import

In `GameImports.cs`, add the `[DllImport]` declaration:

```csharp
public static class GameImports
{
    private const string DllName = "UnrealEditor-DotNetScripting.dll";

    // ═══════════════════════════════════════════════════════════════
    // YOUR NEW CATEGORY
    // ═══════════════════════════════════════════════════════════════

    [DllImport(DllName, CallingConvention = CallingConvention.Cdecl)]
    public static extern void YourFunction_Native(IntPtr handle, float param);

    [DllImport(DllName, CallingConvention = CallingConvention.Cdecl)]
    [return: MarshalAs(UnmanagedType.I1)]  // for bool return types
    public static extern bool YourQuery_Native(int id, out SomeStruct_Interop outResult);
}
```

### Step 5: Add Type Conversion (if needed)

In `TypeConversions.cs`, add any new struct conversions:

```csharp
[StructLayout(LayoutKind.Sequential)]
public struct SomeStruct_Interop
{
    public float X, Y, Z;
    public int SomeId;
    
    public SomeStruct_Interop(float x, float y, float z, int id)
    {
        X = x; Y = y; Z = z; SomeId = id;
    }
}

// Extension methods for easy conversion
public static class SomeStructExtensions
{
    public static SomeStruct_Interop ToInterop(this YourCSharpStruct data)
    {
        return new SomeStruct_Interop(data.X, data.Y, data.Z, data.Id);
    }

    public static YourCSharpStruct FromInterop(this SomeStruct_Interop interop)
    {
        return new YourCSharpStruct 
        { 
            X = interop.X, Y = interop.Y, Z = interop.Z, Id = interop.SomeId 
        };
    }
}
```

### Step 6: Create User-Friendly C# API

In `GameAPI.cs`, add convenient wrapper methods:

```csharp
public static class GameAPI
{
    /// <summary>
    /// Does something cool with the specified handle and parameter.
    /// </summary>
    /// <param name="handle">The handle to operate on</param>
    /// <param name="param">The parameter value</param>
    public static void DoSomethingCool(IntPtr handle, float param)
    {
        GameImports.YourFunction_Native(handle, param);
    }

    /// <summary>
    /// Queries data by ID and returns the result.
    /// </summary>
    /// <param name="id">The ID to query</param>
    /// <returns>The query result, or null if not found</returns>
    public static YourCSharpStruct? QueryData(int id)
    {
        if (GameImports.YourQuery_Native(id, out SomeStruct_Interop result))
        {
            return result.FromInterop();
        }
        return null;
    }
}
```

## ⚠️ Important Rules & Best Practices

### C++ Export Rules
1. **Always use `extern "C"`** - Required for C# interop
2. **Never return structs directly** - Use output parameters instead:
   ```cpp
   // ❌ BAD - C linkage can't return structs
   extern "C" DOTNETSCRIPTING_API FVector_Interop GetPosition();
   
   // ✅ GOOD - Use output parameter
   extern "C" DOTNETSCRIPTING_API void GetPosition(FVector_Interop* outPosition);
   ```
3. **Always validate input parameters** - Check for null pointers
4. **Use descriptive logging** - Include `[MODDING]` prefix in logs
5. **Add TODO comments** - For stub implementations

### C# Import Rules
1. **Use correct calling convention** - Always `CallingConvention.Cdecl`
2. **Marshal return types properly**:
   ```csharp
   [return: MarshalAs(UnmanagedType.I1)]  // for bool
   public static extern bool SomeFunction();
   ```
3. **Use `out` for output parameters**:
   ```csharp
   public static extern void GetData(out SomeStruct_Interop outData);
   ```
4. **Use `IntPtr` for void pointers**:
   ```csharp
   public static extern void ProcessHandle(IntPtr handle);
   ```

### Type Safety Rules
1. **Create interop structs for UE types** - Don't use UE types directly in exports
2. **Use explicit layout for structs**:
   ```csharp
   [StructLayout(LayoutKind.Sequential)]
   public struct MyStruct_Interop { ... }
   ```
3. **Provide conversion methods** - Between interop and user-friendly types
4. **Match C++ struct layout exactly** - Order and types must be identical

## 📝 Example: Adding Vehicle System

Here's a complete example of adding a new vehicle API based on your Entity-Vehicle inheritance architecture:

### 🏗️ C++ Side: Vehicle inherits from Entity

```cpp
// Your existing C++ classes (in your game code)
class ABaseEntity : public AActor 
{
public:
    int32 ID;
    int32 ModelID; 
    FString Name;
    FTransform Transform;
    float Health;
    // ... other entity properties
};

class AVehicle : public ABaseEntity
{
public:
    float EngineHealth;
    float BodyHealth; 
    float MaxSpeed;
    int32 WheelCount;
    bool bIsEngineRunning;
    // ... vehicle-specific properties
};
```

### 1. GameExports.h - Define Interop Types

```cpp
// Base Entity interop (shared by Ped, Vehicle, etc.)
struct FEntityData_Interop
{
    int32 ID;
    int32 ModelID;
    char Name[64];           // Fixed-size for C interop
    FVector3f_Interop Position;
    FRotator_Interop Rotation;
    FVector3f_Interop Scale;
    float Health;
    
    FEntityData_Interop() 
        : ID(0), ModelID(0), Health(100.0f), Scale({1.0f, 1.0f, 1.0f}) 
    {
        memset(Name, 0, sizeof(Name));
    }
};

// Vehicle-specific interop (extends entity data)
struct FVehicleData_Interop
{
    FEntityData_Interop EntityData;  // Base entity data
    
    // Vehicle-specific data
    float EngineHealth;
    float BodyHealth;
    float MaxSpeed;
    int32 WheelCount;
    bool bIsEngineRunning;
    
    FVehicleData_Interop() 
        : EngineHealth(100.0f), BodyHealth(100.0f), MaxSpeed(200.0f), 
          WheelCount(4), bIsEngineRunning(false) {}
};

extern "C" {
    // ═══════════════════════════════════════════════════════════════
    // VEHICLE SYSTEM - Entity-based vehicle management
    // ═══════════════════════════════════════════════════════════════
    
    // Vehicle lifecycle
    DOTNETSCRIPTING_API void* Vehicle_Spawn_Native(int32 modelID, const char* name, FVector3f_Interop position, FRotator_Interop rotation);
    DOTNETSCRIPTING_API bool Vehicle_Destroy_Native(void* vehicleHandle);
    DOTNETSCRIPTING_API bool Vehicle_IsValid_Native(void* vehicleHandle);
    
    // Vehicle data access (combines entity + vehicle data)
    DOTNETSCRIPTING_API void Vehicle_GetData_Native(void* vehicleHandle, FVehicleData_Interop* outData);
    DOTNETSCRIPTING_API void Vehicle_SetData_Native(void* vehicleHandle, const FVehicleData_Interop* data);
    
    // Vehicle-specific operations
    DOTNETSCRIPTING_API void Vehicle_StartEngine_Native(void* vehicleHandle);
    DOTNETSCRIPTING_API void Vehicle_StopEngine_Native(void* vehicleHandle);
    DOTNETSCRIPTING_API void Vehicle_SetSpeed_Native(void* vehicleHandle, float speed);
    
    // Entity base operations (inherited)
    DOTNETSCRIPTING_API void Vehicle_SetPosition_Native(void* vehicleHandle, FVector3f_Interop position);
    DOTNETSCRIPTING_API void Vehicle_GetPosition_Native(void* vehicleHandle, FVector3f_Interop* outPosition);
    DOTNETSCRIPTING_API void Vehicle_SetHealth_Native(void* vehicleHandle, float health);
    DOTNETSCRIPTING_API float Vehicle_GetHealth_Native(void* vehicleHandle);
}
```

### 2. GameExports.cpp - Implementation

```cpp
extern "C" DOTNETSCRIPTING_API void* Vehicle_Spawn_Native(int32 modelID, const char* name, FVector3f_Interop position, FRotator_Interop rotation)
{
    if (!name)
    {
        UE_LOG(LogTemp, Warning, TEXT("[MODDING] Vehicle_Spawn called with null name"));
        return nullptr;
    }

    UE_LOG(LogTemp, Log, TEXT("[MODDING] Vehicle_Spawn: ModelID=%d, Name=%s at (%f,%f,%f)"), 
           modelID, UTF8_TO_TCHAR(name), position.X, position.Y, position.Z);
    
    // TODO: Implement actual vehicle spawning
    // UWorld* World = GetCurrentWorld();
    // if (World) {
    //     FVector SpawnLocation = position.ToFVector();
    //     FRotator SpawnRotation = rotation.ToFRotator();
    //     AVehicle* NewVehicle = World->SpawnActor<AVehicle>(SpawnLocation, SpawnRotation);
    //     if (NewVehicle) {
    //         NewVehicle->ID = GenerateUniqueID();
    //         NewVehicle->ModelID = modelID;
    //         NewVehicle->Name = FString(UTF8_TO_TCHAR(name));
    //         return NewVehicle;
    //     }
    // }
    
    return nullptr;
}

extern "C" DOTNETSCRIPTING_API void Vehicle_GetData_Native(void* vehicleHandle, FVehicleData_Interop* outData)
{
    if (!outData)
    {
        UE_LOG(LogTemp, Warning, TEXT("[MODDING] Vehicle_GetData called with null output"));
        return;
    }

    *outData = FVehicleData_Interop(); // Initialize

    if (!vehicleHandle)
    {
        UE_LOG(LogTemp, Warning, TEXT("[MODDING] Vehicle_GetData called with null handle"));
        return;
    }

    // TODO: Implement data retrieval
    // AVehicle* Vehicle = static_cast<AVehicle*>(vehicleHandle);
    // if (Vehicle) {
    //     // Fill entity data
    //     outData->EntityData.ID = Vehicle->ID;
    //     outData->EntityData.ModelID = Vehicle->ModelID;
    //     FCStringAnsi::Strncpy(outData->EntityData.Name, TCHAR_TO_UTF8(*Vehicle->Name), 63);
    //     outData->EntityData.Position = FVector3f_Interop(Vehicle->GetActorLocation());
    //     outData->EntityData.Rotation = FRotator_Interop(Vehicle->GetActorRotation());
    //     outData->EntityData.Scale = FVector3f_Interop(Vehicle->GetActorScale3D());
    //     outData->EntityData.Health = Vehicle->Health;
    //     
    //     // Fill vehicle-specific data
    //     outData->EngineHealth = Vehicle->EngineHealth;
    //     outData->BodyHealth = Vehicle->BodyHealth;
    //     outData->MaxSpeed = Vehicle->MaxSpeed;
    //     outData->WheelCount = Vehicle->WheelCount;
    //     outData->bIsEngineRunning = Vehicle->bIsEngineRunning;
    // }

    UE_LOG(LogTemp, Log, TEXT("[MODDING] Vehicle_GetData called"));
}

extern "C" DOTNETSCRIPTING_API void Vehicle_StartEngine_Native(void* vehicleHandle)
{
    if (!vehicleHandle)
    {
        UE_LOG(LogTemp, Warning, TEXT("[MODDING] Vehicle_StartEngine called with null handle"));
        return;
    }

    // TODO: Implement engine starting
    // AVehicle* Vehicle = static_cast<AVehicle*>(vehicleHandle);
    // if (Vehicle) {
    //     Vehicle->bIsEngineRunning = true;
    //     Vehicle->OnEngineStart(); // Custom event
    // }

    UE_LOG(LogTemp, Log, TEXT("[MODDING] Vehicle_StartEngine called"));
}
```

### 3. GameImports.cs - DLL Imports (INTERNAL/HIDDEN)

```csharp
// This class is INTERNAL - users never see these DllImports directly
internal static class GameImports
{
    private const string DllName = "UnrealEditor-DotNetScripting.dll";

    // ═══════════════════════════════════════════════════════════════
    // VEHICLE SYSTEM - Internal DLL imports (hidden from users)
    // ═══════════════════════════════════════════════════════════════

    [DllImport(DllName, CallingConvention = CallingConvention.Cdecl)]
    internal static extern IntPtr Vehicle_Spawn_Native(int modelID, [MarshalAs(UnmanagedType.LPStr)] string name, 
                                                      FVector3f_Interop position, FRotator_Interop rotation);

    [DllImport(DllName, CallingConvention = CallingConvention.Cdecl)]
    [return: MarshalAs(UnmanagedType.I1)]
    internal static extern bool Vehicle_Destroy_Native(IntPtr vehicleHandle);

    [DllImport(DllName, CallingConvention = CallingConvention.Cdecl)]
    [return: MarshalAs(UnmanagedType.I1)]
    internal static extern bool Vehicle_IsValid_Native(IntPtr vehicleHandle);

    [DllImport(DllName, CallingConvention = CallingConvention.Cdecl)]
    internal static extern void Vehicle_GetData_Native(IntPtr vehicleHandle, out FVehicleData_Interop outData);

    [DllImport(DllName, CallingConvention = CallingConvention.Cdecl)]
    internal static extern void Vehicle_SetData_Native(IntPtr vehicleHandle, in FVehicleData_Interop data);

    [DllImport(DllName, CallingConvention = CallingConvention.Cdecl)]
    internal static extern void Vehicle_StartEngine_Native(IntPtr vehicleHandle);

    [DllImport(DllName, CallingConvention = CallingConvention.Cdecl)]
    internal static extern void Vehicle_StopEngine_Native(IntPtr vehicleHandle);

    [DllImport(DllName, CallingConvention = CallingConvention.Cdecl)]
    internal static extern void Vehicle_SetSpeed_Native(IntPtr vehicleHandle, float speed);

    [DllImport(DllName, CallingConvention = CallingConvention.Cdecl)]
    internal static extern void Vehicle_SetPosition_Native(IntPtr vehicleHandle, FVector3f_Interop position);

    [DllImport(DllName, CallingConvention = CallingConvention.Cdecl)]
    internal static extern void Vehicle_GetPosition_Native(IntPtr vehicleHandle, out FVector3f_Interop outPosition);

    [DllImport(DllName, CallingConvention = CallingConvention.Cdecl)]
    internal static extern void Vehicle_SetHealth_Native(IntPtr vehicleHandle, float health);

    [DllImport(DllName, CallingConvention = CallingConvention.Cdecl)]
    internal static extern float Vehicle_GetHealth_Native(IntPtr vehicleHandle);
}
```

### 4. TypeConversions.cs - Type Definitions & Conversions

```csharp
// ═══════════════════════════════════════════════════════════════
// ENTITY & VEHICLE INTEROP TYPES
// ═══════════════════════════════════════════════════════════════

[StructLayout(LayoutKind.Sequential, CharSet = CharSet.Ansi)]
public struct FEntityData_Interop
{
    public int ID;
    public int ModelID;
    [MarshalAs(UnmanagedType.ByValTStr, SizeConst = 64)]
    public string Name;
    public FVector3f_Interop Position;
    public FRotator_Interop Rotation;
    public FVector3f_Interop Scale;
    public float Health;
}

[StructLayout(LayoutKind.Sequential)]
public struct FVehicleData_Interop
{
    public FEntityData_Interop EntityData;
    public float EngineHealth;
    public float BodyHealth;
    public float MaxSpeed;
    public int WheelCount;
    [MarshalAs(UnmanagedType.I1)]
    public bool IsEngineRunning;
}

// ═══════════════════════════════════════════════════════════════
// USER-FRIENDLY C# CLASSES (what modders actually use)
// ═══════════════════════════════════════════════════════════════

/// <summary>
/// Base entity class - inherited by Ped, Vehicle, etc.
/// </summary>
public class Entity
{
    public int ID { get; set; }
    public int ModelID { get; set; }
    public string Name { get; set; } = "";
    public FVector3f Position { get; set; }
    public FRotator Rotation { get; set; }
    public FVector3f Scale { get; set; } = new(1.0f, 1.0f, 1.0f);
    public float Health { get; set; } = 100.0f;
    
    // Internal handle to the native object
    internal IntPtr Handle { get; set; }
    
    public bool IsValid => Handle != IntPtr.Zero;
}

/// <summary>
/// Vehicle class - extends Entity with vehicle-specific properties
/// </summary>
public class Vehicle : Entity
{
    public float EngineHealth { get; set; } = 100.0f;
    public float BodyHealth { get; set; } = 100.0f;
    public float MaxSpeed { get; set; } = 200.0f;
    public int WheelCount { get; set; } = 4;
    public bool IsEngineRunning { get; set; } = false;
    
    /// <summary>
    /// Creates a Vehicle from native data
    /// </summary>
    internal static Vehicle FromInterop(FVehicleData_Interop data, IntPtr handle)
    {
        return new Vehicle
        {
            // Entity properties
            ID = data.EntityData.ID,
            ModelID = data.EntityData.ModelID,
            Name = data.EntityData.Name ?? "",
            Position = data.EntityData.Position.FromInterop(),
            Rotation = data.EntityData.Rotation.FromInterop(),
            Scale = data.EntityData.Scale.FromInterop(),
            Health = data.EntityData.Health,
            Handle = handle,
            
            // Vehicle properties  
            EngineHealth = data.EngineHealth,
            BodyHealth = data.BodyHealth,
            MaxSpeed = data.MaxSpeed,
            WheelCount = data.WheelCount,
            IsEngineRunning = data.IsEngineRunning
        };
    }
    
    /// <summary>
    /// Converts this Vehicle to interop format
    /// </summary>
    internal FVehicleData_Interop ToInterop()
    {
        return new FVehicleData_Interop
        {
            EntityData = new FEntityData_Interop
            {
                ID = ID,
                ModelID = ModelID,
                Name = Name,
                Position = Position.ToInterop(),
                Rotation = Rotation.ToInterop(),
                Scale = Scale.ToInterop(),
                Health = Health
            },
            EngineHealth = EngineHealth,
            BodyHealth = BodyHealth,
            MaxSpeed = MaxSpeed,
            WheelCount = WheelCount,
            IsEngineRunning = IsEngineRunning
        };
    }
}
```

### 5. GameAPI.cs - User-Friendly Public API

```csharp
/// <summary>
/// Vehicle management API - This is what modders actually use!
/// DLL imports are completely hidden from users.
/// </summary>
public static class VehicleAPI
{
    /// <summary>
    /// Spawns a vehicle with the specified model and properties.
    /// </summary>
    /// <param name="modelID">Vehicle model ID</param>
    /// <param name="name">Display name for the vehicle</param>
    /// <param name="position">Spawn position</param>
    /// <param name="rotation">Spawn rotation (optional)</param>
    /// <returns>Vehicle instance or null if spawn failed</returns>
    public static Vehicle? SpawnVehicle(int modelID, string name, FVector3f position, FRotator? rotation = null)
    {
        var spawnRotation = rotation ?? new FRotator(0, 0, 0);
        
        IntPtr handle = GameImports.Vehicle_Spawn_Native(modelID, name, 
                                                        position.ToInterop(), 
                                                        spawnRotation.ToInterop());
        
        if (handle == IntPtr.Zero)
            return null;
            
        // Get the full vehicle data and create user object
        GameImports.Vehicle_GetData_Native(handle, out var data);
        return Vehicle.FromInterop(data, handle);
    }
    
    /// <summary>
    /// Gets a vehicle by its handle (for internal use)
    /// </summary>
    internal static Vehicle? GetVehicleByHandle(IntPtr handle)
    {
        if (handle == IntPtr.Zero || !GameImports.Vehicle_IsValid_Native(handle))
            return null;
            
        GameImports.Vehicle_GetData_Native(handle, out var data);
        return Vehicle.FromInterop(data, handle);
    }
    
    /// <summary>
    /// Destroys the specified vehicle.
    /// </summary>
    public static bool DestroyVehicle(Vehicle vehicle)
    {
        if (vehicle?.Handle == IntPtr.Zero) return false;
        return GameImports.Vehicle_Destroy_Native(vehicle.Handle);
    }
    
    /// <summary>
    /// Starts the vehicle's engine.
    /// </summary>
    public static void StartEngine(Vehicle vehicle)
    {
        if (vehicle?.Handle == IntPtr.Zero) return;
        GameImports.Vehicle_StartEngine_Native(vehicle.Handle);
        vehicle.IsEngineRunning = true; // Update local state
    }
    
    /// <summary>
    /// Stops the vehicle's engine.
    /// </summary>
    public static void StopEngine(Vehicle vehicle)
    {
        if (vehicle?.Handle == IntPtr.Zero) return;
        GameImports.Vehicle_StopEngine_Native(vehicle.Handle);
        vehicle.IsEngineRunning = false; // Update local state
    }
    
    /// <summary>
    /// Sets the vehicle's current speed.
    /// </summary>
    public static void SetSpeed(Vehicle vehicle, float speed)
    {
        if (vehicle?.Handle == IntPtr.Zero) return;
        GameImports.Vehicle_SetSpeed_Native(vehicle.Handle, speed);
    }
    
    /// <summary>
    /// Updates the vehicle's position in the world.
    /// </summary>
    public static void SetPosition(Vehicle vehicle, FVector3f position)
    {
        if (vehicle?.Handle == IntPtr.Zero) return;
        GameImports.Vehicle_SetPosition_Native(vehicle.Handle, position.ToInterop());
        vehicle.Position = position; // Update local state
    }
    
    /// <summary>
    /// Gets the vehicle's current position.
    /// </summary>
    public static FVector3f GetPosition(Vehicle vehicle)
    {
        if (vehicle?.Handle == IntPtr.Zero) return new FVector3f();
        
        GameImports.Vehicle_GetPosition_Native(vehicle.Handle, out var pos);
        var result = pos.FromInterop();
        vehicle.Position = result; // Update local state
        return result;
    }
    
    /// <summary>
    /// Sets the vehicle's health.
    /// </summary>
    public static void SetHealth(Vehicle vehicle, float health)
    {
        if (vehicle?.Handle == IntPtr.Zero) return;
        GameImports.Vehicle_SetHealth_Native(vehicle.Handle, health);
        vehicle.Health = health; // Update local state
    }
    
    /// <summary>
    /// Gets the vehicle's current health.
    /// </summary>
    public static float GetHealth(Vehicle vehicle)
    {
        if (vehicle?.Handle == IntPtr.Zero) return 0.0f;
        
        float health = GameImports.Vehicle_GetHealth_Native(vehicle.Handle);
        vehicle.Health = health; // Update local state
        return health;
    }
    
    /// <summary>
    /// Refreshes the vehicle's data from the native side.
    /// Call this to sync local state with the game engine.
    /// </summary>
    public static void RefreshData(Vehicle vehicle)
    {
        if (vehicle?.Handle == IntPtr.Zero) return;
        
        GameImports.Vehicle_GetData_Native(vehicle.Handle, out var data);
        var updated = Vehicle.FromInterop(data, vehicle.Handle);
        
        // Update all properties
        vehicle.ID = updated.ID;
        vehicle.ModelID = updated.ModelID;
        vehicle.Name = updated.Name;
        vehicle.Position = updated.Position;
        vehicle.Rotation = updated.Rotation;
        vehicle.Scale = updated.Scale;
        vehicle.Health = updated.Health;
        vehicle.EngineHealth = updated.EngineHealth;
        vehicle.BodyHealth = updated.BodyHealth;
        vehicle.MaxSpeed = updated.MaxSpeed;
        vehicle.WheelCount = updated.WheelCount;
        vehicle.IsEngineRunning = updated.IsEngineRunning;
    }
}
```

## � API Hiding Strategy - How We Hide DLL Imports

### The Three-Layer Architecture:

```
┌─────────────────────────────────────────────────────────────┐
│                     USER'S MOD CODE                         │
│  🎮 What modders see and use                               │
│                                                             │
│  var car = VehicleAPI.SpawnVehicle(123, "Police Car",      │
│                     new FVector3f(0, 0, 0));               │
│  VehicleAPI.StartEngine(car);                              │
│  VehicleAPI.SetSpeed(car, 60.0f);                          │
│                                                             │
├─────────────────────────────────────────────────────────────┤
│                    GAMEMODDING.DLL                          │
│  📚 User-friendly wrapper API (PUBLIC)                     │
│                                                             │
│  public static class VehicleAPI                            │
│  {                                                          │
│      public static Vehicle? SpawnVehicle(...)              │
│      public static void StartEngine(Vehicle vehicle)       │
│  }                                                          │
│                                                             │
├─────────────────────────────────────────────────────────────┤
│                    GAMEMODDING.DLL                          │
│  🔒 DLL Import Layer (INTERNAL - HIDDEN)                   │
│                                                             │
│  internal static class GameImports                         │
│  {                                                          │
│      [DllImport("UnrealEditor-DotNetScripting.dll")]       │
│      internal static extern IntPtr Vehicle_Spawn_Native... │
│  }                                                          │
│                                                             │
├─────────────────────────────────────────────────────────────┤
│              UNREALENGINE-DOTNETSCRIPTING.DLL              │
│  ⚙️ C++ Native Implementation                              │
│                                                             │
│  extern "C" void* Vehicle_Spawn_Native(...)                │
│  {                                                          │
│      // Actual UE C++ code here                            │
│  }                                                          │
└─────────────────────────────────────────────────────────────┘
```

### How the Hiding Works:

1. **`internal` keyword**: DLL imports are marked as `internal`, making them invisible outside the GameModding.dll assembly
   ```csharp
   internal static class GameImports  // 🔒 HIDDEN from users
   {
       [DllImport(...)]
       internal static extern IntPtr Vehicle_Spawn_Native(...);
   }
   ```

2. **`public` wrapper APIs**: Users only see clean, friendly APIs
   ```csharp
   public static class VehicleAPI  // ✅ VISIBLE to users
   {
       public static Vehicle? SpawnVehicle(...)  // ✅ Clean, typed interface
       {
           // Internal call to hidden DLL import
           IntPtr handle = GameImports.Vehicle_Spawn_Native(...);
           return Vehicle.FromInterop(data, handle);
       }
   }
   ```

3. **Type abstraction**: Users work with nice C# classes, not raw handles
   ```csharp
   // ❌ Users NEVER see this:
   IntPtr handle = SomeNativeFunction();
   
   // ✅ Users see this instead:
   Vehicle car = VehicleAPI.SpawnVehicle(123, "My Car", position);
   ```

### Benefits of This Approach:

- **🎯 Clean API**: Modders use `Vehicle car` instead of `IntPtr handle`
- **🔒 Implementation hiding**: DLL imports are completely internal
- **🛡️ Type safety**: No raw pointers, marshaling handled internally  
- **📚 IntelliSense**: Users get full documentation and autocomplete
- **🔄 Future-proof**: Can change internal implementation without breaking mods

### Example - What Users Actually Write:

```csharp
// User's mod code - clean and simple!
public class MyVehicleMod
{
    public void OnStart()
    {
        // Spawn a police car
        var policeCar = VehicleAPI.SpawnVehicle(
            modelID: 123, 
            name: "Police Interceptor",
            position: new FVector3f(100, 200, 0)
        );
        
        if (policeCar != null)
        {
            // Start the engine
            VehicleAPI.StartEngine(policeCar);
            
            // Set max speed
            VehicleAPI.SetSpeed(policeCar, 120.0f);
            
            // Access properties directly
            Console.WriteLine($"Vehicle {policeCar.Name} has {policeCar.WheelCount} wheels");
            Console.WriteLine($"Engine running: {policeCar.IsEngineRunning}");
            
            // Update health
            VehicleAPI.SetHealth(policeCar, 75.0f);
        }
    }
}
```

**Notice**: No `IntPtr`, no `[DllImport]`, no marshaling - just clean, typed C# code!

## 🚀 Build & Test Process

1. **Build C++ Plugin**: `D:/Applications/UE_5.6/Engine/Build/BatchFiles/Build.bat GameEditor Win64 Development -project="D:/UnrealProjs/Game/Game.uproject"`
2. **Build C# Library**: `dotnet build ModdingTemplate/GameModding/GameModding.csproj`
3. **Test with Example**: `dotnet build ModdingTemplate/Examples/YourExample/YourExample.csproj`

1. **Build C++ Plugin**: `D:/Applications/UE_5.6/Engine/Build/BatchFiles/Build.bat GameEditor Win64 Development -project="D:/UnrealProjs/Game/Game.uproject"`
2. **Build C# Library**: `dotnet build ModdingTemplate/GameModding/GameModding.csproj`
3. **Test with Example**: `dotnet build ModdingTemplate/Examples/YourExample/YourExample.csproj`

## 📋 Common Issues & Solutions

### Issue: "function does not take X arguments"
**Cause**: Function signature mismatch between header and implementation
**Solution**: Ensure `.h` and `.cpp` signatures match exactly

### Issue: "cannot convert from X to Y"
**Cause**: Type mismatch in function calls
**Solution**: Check interop struct usage and conversions

### Issue: "Unterminated character constant"
**Cause**: Missing quotes or escaped characters
**Solution**: Check string literals and character escaping

### Issue: DLL not found at runtime
**Cause**: DLL not in correct location
**Solution**: Ensure DLL builds to `Binaries/Win64/` directory

---

## 🎉 You're Ready!

With this guide, you can extend the modding API with any new functionality needed. Remember to:
- ✅ Follow the established patterns
- ✅ Add proper error checking
- ✅ Document your functions
- ✅ Test both C++ and C# sides
- ✅ Keep interop types simple and efficient

Happy modding! 🎮
