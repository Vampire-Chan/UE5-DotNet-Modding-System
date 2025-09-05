# DotNetScripting Plugin

This plugin provides the core C++ bridge/wrapper functionality for .NET integration in Unreal Engine. It handles the low-level interop between the game engine and the .NET runtime.

## Plugin Structure

```
DotNetScripting/
├── DotNetScripting.uplugin         ← Plugin definition file
├── Source/DotNetScripting/          ← Core C++ bridge implementation
│   ├── DotNetScripting.Build.cs     ← Build configuration
│   ├── Public/                      ← Header files
│   │   ├── DotNetScripting.h        ← Main plugin header
│   │   ├── DotNetHostManager.h      ← .NET runtime host management
│   │   ├── GameExports.h            ← Game-specific export functions
│   │   ├── SimpleExports.h          ← Basic export functions
│   │   ├── UnrealEngineAPI.h        ← UE API wrappers
│   │   ├── UnrealEngineExports.h    ← UE export functions
│   │   └── UnrealExporter.h         ← UE export utilities
│   └── Private/                     ← Implementation files
│       ├── DotNetScripting.cpp      ← Main plugin implementation
│       ├── DotNetHostManager.cpp    ← .NET runtime management
│       ├── GameExports.cpp          ← Game-specific implementations
│       ├── SimpleExports.cpp        ← Basic function implementations
│       ├── UnrealEngineAPI.cpp      ← UE API implementations
│       ├── UnrealEngineExports.cpp  ← UE export implementations
│       ├── UnrealExporter.cpp       ← UE export utilities
│       └── DotNetScripting.runtimeconfig.json ← .NET runtime config
├── Binaries/                        ← Compiled plugin binaries (auto-generated)
└── Intermediate/                    ← Build intermediates (auto-generated)
```

## Core Functionality

### 1. .NET Runtime Hosting (`DotNetHostManager`)
- Initializes the .NET runtime within Unreal Engine
- Manages assembly loading and execution
- Handles runtime configuration and cleanup

### 2. Game Exports (`GameExports`)
- **Type-safe character spawning** with modular support
- **PlayerNiko integration** with component variations
- **World interaction** functions (player position, rotation, etc.)
- **Ped management** (spawning, controlling, removing characters)
- **Math utilities** (distance calculations, random positions)

### 3. Engine Integration (`UnrealEngineAPI`)
- Low-level UE system access
- Asset loading and management
- Event system integration
- Memory management utilities

### 4. Simple Exports (`SimpleExports`)
- Basic logging functions
- Utility functions for testing
- Development and debugging helpers

## Key Features

### Type-Safe Interop
All exported functions use type-safe structures for C# ↔ C++ communication:
```cpp
struct FVector3f_Interop { float X, Y, Z; };
struct FRotator_Interop { float Pitch, Yaw, Roll; };
struct FLinearColor_Interop { float R, G, B, A; };
```

### Memory Management
- RAII patterns for automatic cleanup
- Safe string marshaling between C# and C++
- Proper handle management for game objects

### Modular Character Support
Built-in support for modular characters like PlayerNiko:
```cpp
extern "C" DOTNETSCRIPTING_API void* PedFactory_SpawnModularCharacter_Native(
    const char* characterPath,
    const char* headVariation,
    const char* upperVariation, 
    const char* lowerVariation,
    const char* feetVariation,
    const char* handVariation,
    FVector3f_Interop position, 
    FRotator_Interop rotation
);
```

## Build Integration

The plugin is built automatically when you compile the game project in Unreal Engine. It will produce:
- `UnrealEditor-DotNetScripting.dll` (for editor)
- `Game-DotNetScripting.dll` (for packaged game)

## Modding Support

This plugin provides the foundation for the modding system. Modders use the **ModdingTemplate** (located in the game project root) which includes:
- **GameModding.dll** - High-level C# API that calls into this plugin
- **Example mods** - Demonstrating how to use the system
- **Documentation** - Complete modding guide

## Runtime Loading

When the game starts:
1. UE loads the DotNetScripting plugin
2. Plugin initializes the .NET runtime
3. Plugin loads `GameModding.dll` from `Binaries/Win64/DotNet/`
4. Plugin scans and loads mod DLLs from `Binaries/Win64/DotNet/Mods/`
5. Mods are initialized and begin execution

## Development Notes

### Current Implementation Status
- ✅ Complete type-safe interop infrastructure
- ✅ Modular character spawning API (PlayerNiko ready)
- ✅ Memory management and string marshaling
- 🔄 Stub implementations (ready for game-specific logic)
- 🔄 Asset loading integration (ready for UE asset system)

### Future Integration
The plugin is designed to be integrated with your game's systems:
1. **GameMode Integration** - Wire plugin initialization into your game startup
2. **Asset Loading** - Connect modular character functions to UE asset system
3. **Game Logic** - Replace stub implementations with actual game functionality

## Technical Architecture

```
Game Engine (C++)
    ↕ [DotNetScripting Plugin]
.NET Runtime (Hosted)
    ↕ [GameModding.dll API]
User Mods (C#)
```

This plugin serves as the critical bridge that enables high-performance, type-safe modding while maintaining the security and stability of the game engine.

## Files You May Need to Modify

When integrating with your game:
- `GameExports.cpp` - Replace stub implementations with actual game logic
- `DotNetScripting.cpp` - Add game-specific initialization code
- `GameExports.h` - Add new export functions as needed

The core infrastructure is complete and production-ready!
