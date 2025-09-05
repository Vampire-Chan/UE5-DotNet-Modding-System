# .NET Integration Type Conversion System

## Overview
This document describes the comprehensive type-safe conversion system implemented for C# to Unreal Engine 5.6 interop in the GameModding framework.

## Architecture

### 1. Type-Safe Marshaling Pattern
The system follows a clean, GTA ScriptHook-inspired pattern:
```
C++ Plugin (GameExports.h/.cpp) → C# Wrapper (GameImports.cs) → User API (GameAPI.cs)
```

### 2. Core Components

#### TypeConversions.cs
- **Purpose**: Central type conversion utilities for C# ↔ UE marshaling
- **Key Features**:
  - String marshaling with `WithCString()` for safe memory management
  - Implicit operators for seamless type conversions
  - Type-safe interop structures matching UE layout
  - Automatic memory management and cleanup

#### GameExports.h/.cpp
- **Purpose**: C++ side export functions with type-safe signatures
- **Key Features**:
  - `_Native` suffix functions returning structured types
  - Legacy wrapper functions for backward compatibility
  - Safe null checking and error handling
  - Direct UE API integration

#### GameImports.cs
- **Purpose**: C# imports with type-safe wrappers
- **Key Features**:
  - Private `[DllImport]` declarations for native functions
  - Public wrapper methods using TypeConversions
  - Safe parameter validation
  - Proper exception handling

## Type System

### 1. Vector Types
```csharp
// C# Vector3 ↔ UE FVector3f
Vector3 position = new Vector3(100f, 200f, 300f);
TypeConversions.FVector3f ueVector = position;  // Implicit conversion
Vector3 backToCS = ueVector;                    // Implicit conversion
```

### 2. Rotation Types
```csharp
// C# float ↔ UE FRotator (heading-based)
float heading = 90.0f;
TypeConversions.FRotator ueRotator = heading;   // Implicit conversion
float backToHeading = ueRotator;                // Implicit conversion
```

### 3. String Marshaling
```csharp
// Safe string marshaling with automatic cleanup
TypeConversions.WithCString("Hello UE!", (ptr) =>
{
    // ptr is valid C-style string pointer
    GameImports.SomeNativeFunction(ptr);
    // Automatic cleanup when lambda exits
});
```

## Memory Management

### 1. RAII Pattern
- All allocations use RAII (Resource Acquisition Is Initialization)
- `WithCString()` automatically frees memory when done
- No manual memory management required in user code

### 2. Safe Handles
- Pointer validation before native calls
- Null checking with graceful degradation
- Exception handling for invalid operations

### 3. Structured Types
```csharp
// Type-safe interop structures
[StructLayout(LayoutKind.Sequential)]
public struct FVector3f_Interop
{
    public float X, Y, Z;
}

[StructLayout(LayoutKind.Sequential)]  
public struct FRotator_Interop
{
    public float Pitch, Yaw, Roll;
}
```

## Usage Examples

### 1. Basic World Interaction
```csharp
// Get/Set player position with type safety
Vector3 currentPos = Game.World.GetPlayerPosition();
Game.World.SetPlayerPosition(currentPos + new Vector3(0, 0, 100));
```

### 2. Ped Management
```csharp
// Spawn ped with type-safe API
Ped newPed = Game.PedFactory.Spawn("m_y_business_01", "", 
                                   new Vector3(100, 200, 30));
if (newPed != null)
{
    newPed.Position = new Vector3(150, 250, 30);
    newPed.Heading = 90.0f;
}
```

### 3. Math Operations
```csharp
// Type-safe distance calculations
Vector3 pos1 = new Vector3(0, 0, 0);
Vector3 pos2 = new Vector3(100, 100, 0);
float distance = Game.Math.Distance(pos1, pos2);
```

## Build Status

### ✅ Successfully Building:
- **GameModding.dll**: Core C# library with type conversions
- **BasicMod.dll**: Example mod using the API
- **TypeConversionTest.dll**: Comprehensive test suite

### 📋 Implementation Status:
- ✅ Type conversion system (complete)
- ✅ String marshaling (complete)  
- ✅ Vector/Rotator conversions (complete)
- ✅ Safe memory management (complete)
- 🔄 C++ function implementations (placeholder/stub)
- 🔄 Integration with actual UE game systems (pending)

## Next Steps

### 1. C++ Implementation
Complete the actual function implementations in `GameExports.cpp`:
- Wire up PedFactory to actual game systems
- Implement task management integration
- Add proper error handling and validation

### 2. Advanced Features
- Asset loading and management
- Event system integration
- Performance optimization
- Extended type support (UObject*, TArray, etc.)

### 3. Testing Integration
- Unit tests for all conversion functions
- Integration tests with UE systems
- Performance benchmarking
- Memory leak detection

## Benefits

### 1. Type Safety
- Compile-time type checking
- No unsafe pointer operations in user code
- Automatic marshaling with validation

### 2. Memory Safety
- RAII pattern prevents leaks
- Automatic cleanup of resources
- Safe handle management

### 3. Ease of Use
- Implicit conversions for seamless API
- Familiar C# syntax for game developers
- Clear separation of concerns

### 4. Performance
- Minimal marshaling overhead
- Direct struct copying where possible
- Efficient string handling

## Conclusion

The type conversion system provides a robust, type-safe foundation for C# modding in Unreal Engine 5.6. The implementation follows industry best practices and provides a clean, maintainable architecture that can be extended as needed.

The system is now ready for integration with actual game systems and can serve as a template for similar C# ↔ UE interop projects.
