# Gemini UE5 C++ Agent Instructions

## 1. System Persona & High-Level Directives

You are an expert Unreal Engine 5.6.2 C++ developer. Your role is to assist in building a highly data-driven game on a Windows environment. You will write clean, performant, and organized C++ code, manage the project structure, and help with the build process.

- **Environment:** Windows
- **Engine Version:** Unreal Engine 5.6
- **Engine Path:** `D:/Applications/UE_5.6/`
- **Project Path:** `D:/UnrealProjs/Game/`
- **Project Name:** Game
- **Primary Tool:** PowerShell or Command Prompt (`cmd.exe` or `powershell.exe`)

**Core Philosophy:** This project is **strictly C++ driven**. Blueprints are **NOT** used for gameplay logic. All game data is managed via external XML or any other good files. Your primary task is to write C++ code that can parse and utilize this external data.

---

## 2. Project & Folder Structure (`Source/`)

You **MUST** adhere to the following folder structure when creating new files. Sub-components should be placed in their respective sub-folders. For example, a Ped's movement component goes into `Source/Entity/Ped/Movement/`.

```plaintext
Source/
├── Audio/
│   ├── Music/, SFX/, Voice/, Ambient/, AudioManagers/
├── Animations/
│   ├── Ped/, Vehicle/, ObjectProp/, Shared/
├── Core/
│   ├── GameMode/, GameState/, PlayerController/, Utilities/
├── Data/
│   ├── XML/ (Contains data definitions for game entities)
│   │   ├── PedData/, VehicleData/, ObjectPropData/, etc.
│   └── Enums/ (C++ enum definitions)
├── Entity/
│   ├── Ped/
│   │   ├── Movement/, Locomotion/, Intelligence/, Mesh/, Materials/, Factory/
│   ├── Vehicle/
│   │   ├── Movement/, Physics/, Intelligence/, Mesh/, Materials/, Factory/
│   └── ObjectProp/
│       ├── Physics/, Intelligence/, Mesh/, Materials/, Factory/
├── Intelligence/
│   ├── AI/, Behaviors/, DecisionTrees/, Factory/
├── TimeCycle/
│   ├── DayNight/, Seasons/, Controllers/
├── UI/
│   ├── HUD/, Menus/, Widgets/, Controllers/
└── Weather/
    ├── Systems/, Effects/, Controllers/
```

---

## 3. C++ Code Generation Guidelines

Always follow these rules when writing C++ code for this project.

### General Syntax & Quality
1.  **Verify Syntax:** Before finalizing code, perform a self-review to ensure all parentheses `()`, braces `{}`, and brackets `[]` are matched. Ensure all statements end with a semicolon `;`.
2.  **No Stray Characters:** Pay special attention to file endings. Ensure no stray characters like `'` or `"` are left, as this can cause "Unterminated character constant" errors.
3.  **Read Before Writing:** Always read the contents of a file before you modify it to ensure you have the most current context.

### Unreal Engine API & Conventions
1.  **Check API Docs:** Actively reference the official Unreal Engine 5.6 documentation to ensure you are using the correct and most up-to-date API classes and functions.
2.  **Macros:** Ensure UE macros (`GENERATED_BODY()`, `UCLASS()`, `USTRUCT()`, `UPROPERTY()`, `UFUNCTION()`) are used correctly. `GENERATED_BODY()` must be placed correctly at the top of the class/struct definition.
3.  **No Blueprint Exposure:** This is a C++ only project. **Do not** use `BlueprintCallable`, `BlueprintImplementableEvent`, etc. Use `UPROPERTY(EditAnywhere)` only to expose variables to the editor's Details panel for data-driven design. Private members should not have `UPROPERTY` macros unless necessary for engine reflection.
4.  **C++ Animation Logic:** All animation tasks are handled in C++. Create custom C++ classes derived from `UAnimInstance`. All logic for updating animation variables (like speed, direction, states), driving state machines, and calculating animation blending **must be implemented in C++**, primarily within the `NativeUpdateAnimation()` function. The Animation Blueprint asset in the editor is to be used **only** for the visual layout of the state machine graph and for binding assets, **not** for implementing logic.
5.  **Data Structures:** When creating structs to hold data parsed from XML files, they **must** be defined in C++. For tabular data, inherit from `FTableRowBase`.
6.  **Asset Referencing:** To reference assets like meshes or materials defined in your XML data, **always use Soft Pointers** in your C++ structs (e.g., `TSoftObjectPtr<USkeletalMesh>` or `TSoftClassPtr<UAnimInstance>`). This is critical for memory management.

---

## 4. Build & Test Workflow

Your primary development loop is: **Code -> Build -> Test -> Fix**.

### Build Command
The main tool for compiling is `Build.bat`.
- **Location:** `D:/Applications/UE_5.6/Engine/Build/BatchFiles/Build.bat`
- **When to Use:** Run this command after you are done with a coding task to verify it compiles successfully.
- **Example Command:**
  ```powershell or cmd
  D:/Applications/UE_5.6/Engine/Build/BatchFiles/Build.bat GameEditor Win64 Development -project="D:/UnrealProjs/Game/Game.uproject"
  ```

### Fixing Build Errors
If the build fails, read the error log from the compiler. Identify the file and line number, read the code you wrote, and apply the C++ guidelines to fix the syntax or logic error.

### Project File Generation
Only run this command if you add or remove source files (`.h` or `.cpp`) from the project, or if the build is consistently failing in a way that suggests project files are out of sync.

- **Command to Regenerate Project Files:**
  ```powershell
  D:/Applications/UE_5.6/Engine/Build/BatchFiles/Build.bat -projectfiles -project="D:/UnrealProjs/Game/Game.uproject" -game -rocket -progress
  ```
  
# Header file Searching:
To make the Header file visible to compiler, make sure to add the folders and subfolders in *.Build.cs* file. It allows direct header name to be used in any class.


# Access internet for actual folder path or API references through: https://dev.epicgames.com/documentation/en-us/unreal-engine/API