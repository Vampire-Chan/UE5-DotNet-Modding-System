# Game Modding Template

This folder contains everything needed to create mods for the game using C#/.NET.

## Folder Structure

```
ModdingTemplate/
├── GameModding/                    ← Core modding API library
│   ├── GameAPI.cs                  ← High-level user-friendly API
│   ├── GameImports.cs              ← Low-level imports from game
│   ├── TypeConversions.cs          ← Type conversion utilities
│   └── GameModding.csproj          ← Project file
├── Examples/                       ← Example mod projects
│   ├── BasicMod/                   ← Simple example mod
│   ├── PlayerNikoExample/          ← PlayerNiko spawning examples  
│   └── TypeConversionTest/         ← Type conversion testing
└── README.md                       ← This file
```

## Quick Start for Modders

### 1. Prerequisites
- .NET 8.0 or later SDK
- Visual Studio 2022, VS Code, or any C# IDE

### 2. Creating Your First Mod

1. **Copy an example project** (e.g., BasicMod) to create your own mod
2. **Rename the folder and project file** to your mod name
3. **Update the project reference** to point to `..\..\GameModding\GameModding.csproj`
4. **Write your mod code** using the GameModding API

### 3. Basic Mod Template

```csharp
using System;
using GameModding;

namespace YourModName
{
    public class YourMod
    {
        public static void OnLoad()
        {
            Game.Log.Info("Your mod loaded successfully!");
            
            // Your mod logic here
        }
    }
}
```

### 4. Building Your Mod

Use the provided build script:
```batch
BuildModdingTemplate.bat
```

Or build manually:
```powershell
dotnet build "ModdingTemplate\GameModding\GameModding.csproj" --configuration Debug
dotnet build "ModdingTemplate\Examples\YourMod\YourMod.csproj" --configuration Debug
```

## API Overview

### Game.Log - Logging System
```csharp
Game.Log.Info("Information message");
Game.Log.Warning("Warning message");
Game.Log.Error("Error message");
```

### Game.PedFactory - Character Spawning
```csharp
// Basic spawning
var ped = Game.PedFactory.Spawn("characterName", "variation", position, heading);

// PlayerNiko spawning (simple)
var niko = Game.PedFactory.SpawnPlayerNiko(new Vector3(0, 0, 100), 0f);

// PlayerNiko spawning (advanced with variations)
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

### Game.World - World Information
```csharp
Vector3 playerPos = Game.World.GetPlayerPosition();
Game.World.SetPlayerPosition(new Vector3(100, 200, 300));
```

### Game.Math - Utility Functions
```csharp
float distance = Game.Math.Distance(pos1, pos2);
float distance2D = Game.Math.Distance2D(pos1, pos2);
Vector3 randomPos = Game.Math.RandomPosition(center, radius);
```

### Ped Class - Character Control
```csharp
if (ped != null && ped.IsValid)
{
    ped.Position = new Vector3(100, 200, 300);
    ped.Heading = 90f;
    
    float distance = ped.DistanceTo(otherPosition);
    bool removed = ped.Remove();
}
```

## Available Character Assets

### PlayerNiko Modular Character
Located in: `Content/Characters/PlayerNiko/`

**Components:**
- **Head**: `head/head_000.uasset` + textures in `head/Texture/`
- **Upper Body**: `uppr/uppr_000.uasset` + textures in `uppr/Texture/`
- **Lower Body**: `lowr/` (various parts)
- **Feet**: `feet/` (various parts)
- **Hands**: `hand/` (various parts)
- **Skeleton**: `SKEL_PlayerNiko.uasset` (main skeleton)

**Usage:**
```csharp
// Spawn with default parts (000 variations)
var niko = Game.PedFactory.SpawnPlayerNiko(spawnPosition);

// Spawn with specific variations
var customNiko = Game.PedFactory.SpawnModularCharacter(
    "PlayerNiko", 
    spawnPosition,
    headVariation: "000",
    upperVariation: "001",  // Different upper body
    lowerVariation: "000",
    // ... etc
);
```

## Type System

### Vector3
```csharp
var position = new Vector3(x, y, z);
position.X = 100f;
position.Y = 200f;
position.Z = 300f;

// Math operations
var newPos = position + new Vector3(10, 0, 0);
var scaled = position * 2.0f;
```

### Type Safety
The system provides automatic, type-safe conversions between C# types and Unreal Engine types:
- C# `Vector3` ↔ UE `FVector3f`
- C# `float` ↔ UE `FRotator` (for headings)
- C# `string` ↔ UE `FString` (with proper memory management)

## Examples

Check the `Examples/` folder for complete, working examples:

1. **BasicMod** - Simple logging and basic API usage
2. **PlayerNikoExample** - Complete PlayerNiko spawning and control
3. **TypeConversionTest** - Testing type conversion system

## Building and Integration

Your compiled mods will be placed in the game's `Binaries\Win64\DotNet\Mods\` folder where the game can automatically load them.

The game uses the DotNetScripting plugin (located in `Plugins\DotNetScripting\`) to provide the C++ ↔ C# bridge.

## Support

This modding template provides a complete, type-safe, and high-performance modding system. The API is designed to be familiar to C# developers while providing access to the game's core systems.

Happy modding! 🎮
