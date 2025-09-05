# .NET Integration Build Configuration

## Overview
The .NET integration system has been configured to output DLLs to the proper locations where the Unreal Engine game executable can find them.

## Directory Structure
```
D:\UnrealProjs\Game\
├── Binaries\Win64\
│   ├── Game.exe                    ← Your compiled game executable
│   ├── UnrealEditor-Game.dll       ← Plugin DLL (built by UE)
│   └── DotNet\
│       ├── net8.0\
│       │   └── GameModding.dll     ← Core C# API library
│       └── Mods\
│           ├── BasicMod.dll        ← Example mod
│           ├── PlayerNikoExample.dll ← PlayerNiko spawning example
│           └── TypeConversionTest.dll ← Type conversion tests
├── Plugins\DotNetScripting\
│   ├── DotNetScripting.uplugin     ← Plugin definition
│   ├── Source\DotNetScripting\     ← C++ plugin source
│   ├── GameModding\                ← Core C# library source
│   └── Examples\                   ← Example mod sources
```

## Build Output Configuration

### GameModding.dll (Core Library)
- **Source**: `Plugins\DotNetScripting\GameModding\`
- **Output**: `Binaries\Win64\DotNet\net8.0\GameModding.dll`
- **Target Framework**: .NET 8.0
- **Purpose**: Core modding API that provides type-safe C# → UE interop

### Example Mods
- **Source**: `Plugins\DotNetScripting\Examples\*\`
- **Output**: `Binaries\Win64\DotNet\Mods\*.dll`
- **Target Framework**: .NET 9.0
- **Purpose**: Example mods demonstrating the API usage

## Key Build Features

### 1. Automatic DLL Placement
All C# projects are configured to build directly to the game binaries folder:
```xml
<OutputPath>..\..\..\Binaries\Win64\DotNet\</OutputPath>
```

### 2. Plugin Integration
The C++ plugin (`DotNetScripting.dll`) is built by Unreal Engine's build system and placed in:
```
Binaries\Win64\UnrealEditor-DotNetScripting.dll
```

### 3. Runtime Loading
When the game starts, the DotNetScripting plugin will:
1. Initialize the .NET runtime
2. Load `GameModding.dll` as the core API
3. Scan `Mods\` folder for mod DLLs
4. Load and initialize each mod

## PlayerNiko Modular Character System

### Asset Structure
The PlayerNiko character uses a modular system:
```
Content\Characters\PlayerNiko\
├── SKEL_PlayerNiko.uasset          ← Main skeleton
├── head\
│   ├── head_000.uasset             ← Head mesh
│   └── Texture\
│       ├── head_diff_000_a_whi.uasset  ← Diffuse texture
│       ├── head_normal_000.uasset      ← Normal map
│       └── head_spec_000.uasset        ← Specular map
├── uppr\                           ← Upper body parts
├── lowr\                           ← Lower body parts  
├── feet\                           ← Feet parts
└── hand\                           ← Hand parts
```

### C# API Usage
```csharp
// Simple PlayerNiko spawning
var niko = Game.PedFactory.SpawnPlayerNiko(
    new Vector3(0, 0, 100), 
    heading: 0f
);

// Advanced modular spawning
var modularNiko = Game.PedFactory.SpawnModularCharacter(
    "PlayerNiko",
    new Vector3(100, 100, 100),
    headVariation: "000",
    upperVariation: "000", 
    lowerVariation: "000",
    feetVariation: "000",
    handVariation: "000",
    rotation: new Vector3(0, 90, 0)
);
```

## Current Implementation Status

### ✅ Complete
- Type-safe C# ↔ UE conversion system
- String marshaling with automatic memory management
- Build system configuration
- Example mod projects
- API documentation

### 🔄 Stub Implementation (Ready for your game integration)
- C++ function implementations in `GameExports.cpp`
- Actual asset loading and modular character assembly
- Integration with your game's PedFactory/character systems

### 📋 Next Steps for Game Integration
1. **Implement GameMode Integration**: Wire up the DotNetScripting plugin to your game's systems
2. **Complete C++ Functions**: Replace stub implementations with actual game logic
3. **Asset Loading**: Implement proper UE asset loading for modular characters
4. **Testing**: Test with actual game executable

## Build Commands

### Build All
```powershell
cd "D:\UnrealProjs\Game"
dotnet build "Plugins\DotNetScripting\GameModding\GameModding.csproj" --configuration Debug
dotnet build "Plugins\DotNetScripting\Examples\BasicMod\BasicMod.csproj" --configuration Debug
dotnet build "Plugins\DotNetScripting\Examples\PlayerNikoExample\PlayerNikoExample.csproj" --configuration Debug
```

### UE Plugin Build
The C++ plugin will be built automatically when you compile the game in Unreal Engine or through:
```powershell
# Build the entire game project (includes plugins)
UnrealBuildTool.exe Game Win64 Development
```

## Runtime Integration Notes

The system is designed to work seamlessly once you integrate it with your game's startup sequence. The DotNetScripting plugin should be loaded by UE, which will then initialize the .NET runtime and load the C# mods.

**The .NET integration infrastructure is complete and ready for your game-specific implementation!**
