# UE5 Reflection API Integration Guide

## Overview

The UE5 Reflection API bridge provides **real-time access** to Unreal Engine's powerful reflection system from C# mods. This enables dynamic interaction with UE5 objects, properties, functions, and classes without requiring recompilation.

## 🚀 Key Features

- **Real-time Object Manipulation**: Create, modify, and destroy UE5 objects from C#
- **Dynamic Property Access**: Get/set any UE5 object property by name
- **Function Invocation**: Call UE5 functions with parameters and return values
- **Class Introspection**: Discover classes, their properties, and functions at runtime
- **Actor System Integration**: Spawn, move, and manage actors in the world
- **Component System**: Add, remove, and configure actor components
- **Blueprint Compatibility**: Works with both C++ and Blueprint classes
- **Type Safety**: Marshaling between UE5 and C# types with validation

## 🏗️ Architecture

```
┌─────────────────┐    ┌───────────────────┐    ┌──────────────────┐
│   C# Mod Code   │───▶│  UE5Reflection.cs │───▶│ ReflectionAPI.h  │
│                 │    │  (High-level API) │    │ (C++ Bridge)     │
└─────────────────┘    └───────────────────┘    └──────────────────┘
                                                          │
                                                          ▼
                                                ┌──────────────────┐
                                                │ UE5 Reflection   │
                                                │ System (Native)  │
                                                └──────────────────┘
```

## 📚 Core Components

### 1. **ReflectionAPI.h/cpp** - C++ Bridge Layer
- Native UE5 reflection system interface
- Memory-safe object manipulation
- Type conversion utilities
- Performance optimization

### 2. **UE5Reflection.cs** - C# Wrapper Layer
- High-level C# API with LINQ support
- Automatic memory management
- Type-safe property access
- Exception handling

### 3. **Wrapper Classes** - Object-Oriented Interface
- `UE5Object` - Base wrapper for all UE5 objects
- `UE5Actor` - Actor-specific functionality
- `UE5Component` - Component management
- `UE5World` - World-level operations

## 🔧 Basic Usage

### Initialization

```csharp
public void Initialize()
{
    // Initialize the reflection system
    if (!UE5Reflection.Initialize())
    {
        throw new Exception("Failed to initialize UE5 Reflection");
    }
    
    // Get system statistics
    var stats = UE5Reflection.GetStats();
    Console.WriteLine($"Loaded {stats.Classes} classes, {stats.Properties} properties");
}
```

### Class Discovery

```csharp
// Find a specific class
var pedClass = UE5Reflection.FindClass("APed");
if (pedClass.HasValue)
{
    Console.WriteLine($"Found APed: {pedClass.Value.NumProperties} properties");
}

// Get all available classes
var allClasses = UE5Reflection.GetAllClasses();
foreach (var cls in allClasses.Where(c => c.IsActor))
{
    Console.WriteLine($"Actor class: {cls.Name}");
}
```

### Object Creation and Manipulation

```csharp
// Get current world
var world = UE5Reflection.GetWorld();

// Spawn an actor
var newActor = world.SpawnActor("APed", new Vector3(100, 0, 50));

// Access properties
var health = newActor.GetProperty<float>("Health");
newActor.SetProperty("Health", 100.0f);

// Move the actor
newActor.Location = new Vector3(200, 100, 50);
newActor.Rotation = new Vector3(0, 45, 0);

// Call functions
newActor.CallFunction("StartWalking");
```

### Component Management

```csharp
// Add components
var meshComponent = actor.AddComponent("StaticMeshComponent");
var dataComponent = actor.AddComponent("UPedDataComponent");

// Access existing components
var rootComponent = actor.GetComponent("RootComponent");

// Remove components
actor.RemoveComponent(meshComponent);
```

## 🎯 Advanced Examples

### Dynamic Ped Creation System

```csharp
public class DynamicPedSpawner
{
    private UE5World world;
    private List<UE5Actor> spawnedPeds = new();
    
    public void SpawnPedWithCustomization(Vector3 location, PedConfig config)
    {
        // Spawn base ped
        var ped = world.SpawnActor("APed", location);
        
        // Configure data component
        var dataComponent = ped.GetComponent("UPedDataComponent");
        if (dataComponent != null)
        {
            dataComponent.SetProperty("Health", config.Health);
            dataComponent.SetProperty("Speed", config.Speed);
            dataComponent.SetProperty("PedType", (int)config.Type);
        }
        
        // Add equipment component
        if (config.HasWeapon)
        {
            var weaponComponent = ped.AddComponent("UWeaponComponent");
            weaponComponent.SetProperty("WeaponType", config.WeaponType);
        }
        
        // Set appearance
        var meshComponent = ped.GetComponent("USkeletalMeshComponent");
        if (meshComponent != null)
        {
            // Set mesh and materials dynamically
            meshComponent.CallFunction("SetSkeletalMesh", /* parameters */);
        }
        
        spawnedPeds.Add(ped);
    }
}
```

### Real-time Property Monitoring

```csharp
public class PedHealthMonitor
{
    public void Update(float deltaTime)
    {
        var allPeds = world.GetActors("APed");
        
        foreach (var ped in allPeds)
        {
            var health = ped.GetProperty<float>("Health");
            
            if (health <= 0)
            {
                // Ped died, trigger death sequence
                ped.CallFunction("PlayDeathAnimation");
                
                // Add death effects
                var effectComponent = ped.AddComponent("UParticleSystemComponent");
                effectComponent.SetProperty("Template", GetDeathEffect());
            }
            else if (health < 30)
            {
                // Low health, change behavior
                ped.CallFunction("SetBehaviorState", /* wounded state */);
            }
        }
    }
}
```

### Blueprint Integration

```csharp
public void WorkWithBlueprints()
{
    // Find blueprint classes
    var bpClasses = UE5Reflection.GetAllClasses()
        .Where(c => c.IsBlueprintable && c.Name.Contains("BP_"))
        .ToList();
    
    foreach (var bpClass in bpClasses)
    {
        Console.WriteLine($"Blueprint: {bpClass.Name}");
        
        // Get blueprint functions
        var functions = UE5Reflection.GetClassFunctions(bpClass.Name)
            .Where(f => f.IsBlueprintCallable)
            .ToList();
        
        foreach (var func in functions)
        {
            Console.WriteLine($"  Callable: {func.Name}()");
        }
    }
}
```

## 🔒 Safety and Best Practices

### Memory Management

```csharp
// Always use 'using' statements for automatic cleanup
using var actor = world.SpawnActor("APed", location);
// Actor is automatically disposed when leaving scope

// Or manually manage lifetime
var persistentActor = world.SpawnActor("APed", location);
// ... use actor ...
persistentActor.Dispose(); // Clean up when done
```

### Error Handling

```csharp
public T SafeGetProperty<T>(UE5Object obj, string propertyName, T defaultValue) where T : struct
{
    try
    {
        if (!obj.IsValid)
        {
            Console.WriteLine($"Object is invalid when accessing {propertyName}");
            return defaultValue;
        }
        
        return obj.GetProperty<T>(propertyName);
    }
    catch (Exception ex)
    {
        Console.WriteLine($"Error getting property {propertyName}: {ex.Message}");
        return defaultValue;
    }
}
```

### Performance Optimization

```csharp
public class OptimizedPedManager
{
    private Dictionary<string, ReflectionClass> classCache = new();
    private Dictionary<string, List<ReflectionProperty>> propertyCache = new();
    
    public void CacheReflectionData()
    {
        // Cache frequently used classes
        var pedClass = UE5Reflection.FindClass("APed");
        if (pedClass.HasValue)
        {
            classCache["APed"] = pedClass.Value;
            propertyCache["APed"] = UE5Reflection.GetClassProperties("APed");
        }
    }
    
    public void FastPropertyAccess(UE5Actor ped)
    {
        // Use cached reflection data for faster access
        if (propertyCache.TryGetValue("APed", out var properties))
        {
            var healthProp = properties.FirstOrDefault(p => p.Name == "Health");
            if (healthProp.PropertyPtr != IntPtr.Zero)
            {
                // Direct property access using cached data
                var health = ped.GetProperty<float>("Health");
            }
        }
    }
}
```

## 🎮 Integration with Your Ped System

### Exporting Your APed Class

```cpp
// In your APed.h, add reflection metadata:
UCLASS(BlueprintType, Blueprintable)
class GAME_API APed : public ACharacter
{
    GENERATED_BODY()

public:
    // Make properties accessible to reflection system
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Ped Data")
    float Health = 100.0f;
    
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Ped Data")
    float Speed = 300.0f;
    
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Ped Data")
    EPedType PedType = EPedType::Civilian;
    
    // Make functions callable from C#
    UFUNCTION(BlueprintCallable, Category = "Ped Actions")
    void StartWalking();
    
    UFUNCTION(BlueprintCallable, Category = "Ped Actions")
    void SetBehaviorState(EPedBehaviorState NewState);
};
```

### Using Your APed in C#

```csharp
public class PedController
{
    public void ControlPed(UE5Actor ped)
    {
        // Access your custom properties
        var health = ped.GetProperty<float>("Health");
        var speed = ped.GetProperty<float>("Speed");
        var pedType = ped.GetProperty<int>("PedType"); // Enum as int
        
        // Call your custom functions
        ped.CallFunction("StartWalking");
        ped.CallFunction("SetBehaviorState", /* parameters */);
        
        // Access your UBaseEntity component
        var baseEntity = ped.GetComponent("UBaseEntity");
        if (baseEntity != null)
        {
            var entityId = baseEntity.GetProperty<int>("EntityId");
            var isActive = baseEntity.GetProperty<bool>("bIsActive");
        }
        
        // Access your UPedDataComponent
        var pedData = ped.GetComponent("UPedDataComponent");
        if (pedData != null)
        {
            var pedConfig = pedData.GetProperty</* your struct type */>("PedConfig");
        }
    }
}
```

## 🔧 Build Integration

Add the Reflection API to your project:

1. **Update DotNetScripting.Build.cs**:
```csharp
PublicDependencyModuleNames.AddRange(new string[]
{
    "Core", "CoreUObject", "Engine", "UnrealEd",
    "BlueprintGraph", "KismetCompiler", "GameplayTags"
});
```

2. **Include in your mod projects**:
```xml
<ProjectReference Include="..\..\GameModding\GameModding.csproj" />
```

3. **Initialize in your mod**:
```csharp
public void Initialize()
{
    UE5Reflection.Initialize();
    // Your mod code here
}

public void Shutdown()
{
    UE5Reflection.Shutdown();
}
```

## 🚨 Troubleshooting

### Common Issues

1. **"Reflection system not initialized"**
   - Call `UE5Reflection.Initialize()` before using any reflection functions

2. **"Property not found"**
   - Ensure property has `UPROPERTY()` macro in C++
   - Check exact property name spelling and case

3. **"Function call failed"**
   - Verify function has `UFUNCTION()` macro
   - Check parameter types and count

4. **"Object is invalid"**
   - Object may have been garbage collected
   - Check `obj.IsValid` before accessing

### Debugging Tips

```csharp
// Print all available classes
var classes = UE5Reflection.GetAllClasses();
foreach (var cls in classes.Where(c => c.Name.Contains("Ped")))
{
    Console.WriteLine($"Class: {cls.Name}");
}

// Print all properties of an object
myObject.PrintProperties();

// Verify object validity
if (!myObject.IsValid)
{
    Console.WriteLine("Object is no longer valid!");
}
```

## 📈 Performance Considerations

- **Cache reflection data** for frequently accessed classes
- **Batch operations** when possible
- **Use direct property access** for performance-critical code
- **Dispose objects** promptly to avoid memory leaks
- **Limit reflection calls** in tight loops

## 🎯 Next Steps

1. **Implement Custom Property Types**: Add support for your specific data structures
2. **Add Delegate Support**: Enable C# event handling for UE5 delegates
3. **Blueprint Node Integration**: Create Blueprint nodes that call your C# functions
4. **Performance Profiling**: Measure and optimize reflection call overhead
5. **Hot Reload Support**: Enable dynamic mod reloading without editor restart

The UE5 Reflection API bridge opens up unlimited possibilities for dynamic, real-time interaction between your C# mods and the Unreal Engine. Use it to create sophisticated modding systems that can adapt and extend your game at runtime! 🚀
