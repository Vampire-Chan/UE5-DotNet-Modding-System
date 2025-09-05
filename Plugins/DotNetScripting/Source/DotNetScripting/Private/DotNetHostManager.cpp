#include "DotNetHostManager.h"
#include "Engine/Engine.h"
#include "HAL/PlatformFilemanager.h"
#include "Misc/Paths.h"
#include "Misc/FileHelper.h"
#include <string>

#ifdef PLATFORM_WINDOWS
#include "Windows/AllowWindowsPlatformTypes.h"
#include <windows.h>
#include <string>
#include "Windows/HideWindowsPlatformTypes.h"
#endif

UDotNetHostManager::UDotNetHostManager()
{
    UE_LOG(LogTemp, Log, TEXT("DotNetHostManager: Constructor called"));
}

void UDotNetHostManager::Initialize(FSubsystemCollectionBase& Collection)
{
    Super::Initialize(Collection);
    
    UE_LOG(LogTemp, Log, TEXT("DotNetHostManager: Initializing subsystem"));
    
    // Initialize the .NET runtime
    if (!InitializeDotNetRuntime())
    {
        UE_LOG(LogTemp, Error, TEXT("DotNetHostManager: Failed to initialize .NET runtime"));
    }
}

void UDotNetHostManager::Deinitialize()
{
    UE_LOG(LogTemp, Log, TEXT("DotNetHostManager: Deinitializing subsystem"));
    
    // Shutdown the .NET runtime
    ShutdownDotNetRuntime();
    
    Super::Deinitialize();
}

bool UDotNetHostManager::InitializeDotNetRuntime()
{
    UE_LOG(LogTemp, Log, TEXT("DotNetHostManager: Starting .NET runtime initialization"));

    if (bIsRuntimeInitialized)
    {
        UE_LOG(LogTemp, Warning, TEXT("DotNetHostManager: Runtime already initialized"));
        return true;
    }

    // Step 1: Load hostfxr library
    if (!LoadHostFxr())
    {
        LogDotNetError(TEXT("InitializeDotNetRuntime"), TEXT("Failed to load hostfxr library"));
        return false;
    }

    // Step 2: Initialize hostfxr for runtime config
    if (!InitializeHostFxr())
    {
        LogDotNetError(TEXT("InitializeDotNetRuntime"), TEXT("Failed to initialize hostfxr"));
        return false;
    }

    // Step 3: Get the load assembly function pointer
    int32 Result = HostFxrGetRuntimeDelegate(
        RuntimeContext,
        hdt_load_assembly_and_get_function_pointer,
        (void**)&LoadAssemblyAndGetFunctionPointer
    );

    if (Result != 0 || LoadAssemblyAndGetFunctionPointer == nullptr)
    {
        LogDotNetError(TEXT("InitializeDotNetRuntime"), 
                      FString::Printf(TEXT("Failed to get load assembly delegate. Error code: %d"), Result));
        return false;
    }

    bIsRuntimeInitialized = true;
    UE_LOG(LogTemp, Log, TEXT("DotNetHostManager: .NET runtime initialized successfully"));

    // Step 4: Load the bridge assembly
    FString BridgeAssemblyPath = GetBridgeAssemblyPath();
    if (FPaths::FileExists(BridgeAssemblyPath))
    {
        LoadBridgeAssembly(BridgeAssemblyPath);
    }
    else
    {
        UE_LOG(LogTemp, Warning, TEXT("DotNetHostManager: Bridge assembly not found at: %s"), *BridgeAssemblyPath);
    }

    return true;
}

void UDotNetHostManager::ShutdownDotNetRuntime()
{
    if (!bIsRuntimeInitialized)
    {
        return;
    }

    UE_LOG(LogTemp, Log, TEXT("DotNetHostManager: Shutting down .NET runtime"));

    // Unload all mods
    TArray<FString> ModNames;
    LoadedMods.GenerateKeyArray(ModNames);
    for (const FString& ModName : ModNames)
    {
        UnloadMod(ModName);
    }

    // Close hostfxr
    if (RuntimeContext && HostFxrClose)
    {
        HostFxrClose(RuntimeContext);
        RuntimeContext = nullptr;
    }

    // Unload hostfxr library
#ifdef PLATFORM_WINDOWS
    if (HostFxrLibrary)
    {
        FreeLibrary((HMODULE)HostFxrLibrary);
        HostFxrLibrary = nullptr;
    }
#endif

    LoadAssemblyAndGetFunctionPointer = nullptr;
    HostFxrInitializeForRuntimeConfig = nullptr;
    HostFxrGetRuntimeDelegate = nullptr;
    HostFxrClose = nullptr;

    bIsRuntimeInitialized = false;
    UE_LOG(LogTemp, Log, TEXT("DotNetHostManager: .NET runtime shutdown complete"));
}

bool UDotNetHostManager::LoadHostFxr()
{
#ifdef PLATFORM_WINDOWS
    // Try to get hostfxr path from nethost
    char_t* HostFxrPath = nullptr;
    size_t BufferSize = 0;
    
    int32 Result = get_hostfxr_path(HostFxrPath, &BufferSize, nullptr);
    if (Result == 0x80008098) // HostApiBufferTooSmall
    {
        HostFxrPath = new char_t[BufferSize];
        Result = get_hostfxr_path(HostFxrPath, &BufferSize, nullptr);
    }

    if (Result != 0)
    {
        UE_LOG(LogTemp, Error, TEXT("DotNetHostManager: Failed to get hostfxr path. Error: %d"), Result);
        if (HostFxrPath) delete[] HostFxrPath;
        return false;
    }

    // Load hostfxr library
    HostFxrLibrary = LoadLibrary(HostFxrPath);
    if (!HostFxrLibrary)
    {
        UE_LOG(LogTemp, Error, TEXT("DotNetHostManager: Failed to load hostfxr library from: %s"), HostFxrPath);
        delete[] HostFxrPath;
        return false;
    }

    UE_LOG(LogTemp, Log, TEXT("DotNetHostManager: Loaded hostfxr from: %s"), HostFxrPath);
    delete[] HostFxrPath;

    // Get function pointers
    HostFxrInitializeForRuntimeConfig = (hostfxr_initialize_for_runtime_config_fn)GetProcAddress((HMODULE)HostFxrLibrary, "hostfxr_initialize_for_runtime_config");
    HostFxrGetRuntimeDelegate = (hostfxr_get_runtime_delegate_fn)GetProcAddress((HMODULE)HostFxrLibrary, "hostfxr_get_runtime_delegate");
    HostFxrClose = (hostfxr_close_fn)GetProcAddress((HMODULE)HostFxrLibrary, "hostfxr_close");

    if (!HostFxrInitializeForRuntimeConfig || !HostFxrGetRuntimeDelegate || !HostFxrClose)
    {
        UE_LOG(LogTemp, Error, TEXT("DotNetHostManager: Failed to get required hostfxr functions"));
        return false;
    }

    return true;
#else
    UE_LOG(LogTemp, Error, TEXT("DotNetHostManager: Platform not supported yet"));
    return false;
#endif
}

bool UDotNetHostManager::InitializeHostFxr()
{
    FString RuntimeConfigPath = GetDotNetRuntimeConfigPath();
    
    if (!FPaths::FileExists(RuntimeConfigPath))
    {
        UE_LOG(LogTemp, Error, TEXT("DotNetHostManager: Runtime config not found: %s"), *RuntimeConfigPath);
        return false;
    }

    // Convert to wide string for Windows
#ifdef PLATFORM_WINDOWS
    const TCHAR* ConfigPathWide = *RuntimeConfigPath;
    int32 Result = HostFxrInitializeForRuntimeConfig(ConfigPathWide, nullptr, &RuntimeContext);
#else
    int32 Result = HostFxrInitializeForRuntimeConfig(TCHAR_TO_UTF8(*RuntimeConfigPath), nullptr, &RuntimeContext);
#endif

    if (Result != 0 || RuntimeContext == nullptr)
    {
        UE_LOG(LogTemp, Error, TEXT("DotNetHostManager: Failed to initialize hostfxr for runtime config. Error: %d"), Result);
        return false;
    }

    UE_LOG(LogTemp, Log, TEXT("DotNetHostManager: Hostfxr initialized with config: %s"), *RuntimeConfigPath);
    return true;
}

bool UDotNetHostManager::LoadMod(const FString& ModPath, const FString& ModName)
{
    if (!bIsRuntimeInitialized)
    {
        LogDotNetError(TEXT("LoadMod"), TEXT("Runtime not initialized"));
        return false;
    }

    if (LoadedMods.Contains(ModName))
    {
        UE_LOG(LogTemp, Warning, TEXT("DotNetHostManager: Mod '%s' is already loaded"), *ModName);
        return true;
    }

    if (!FPaths::FileExists(ModPath))
    {
        LogDotNetError(TEXT("LoadMod"), FString::Printf(TEXT("Mod assembly not found: %s"), *ModPath));
        return false;
    }

    UE_LOG(LogTemp, Log, TEXT("DotNetHostManager: Loading mod '%s' from '%s'"), *ModName, *ModPath);

    // Create mod interface instance
    UDotNetModInterface* ModInterface = NewObject<UDotNetModInterface>(this);
    ModInterface->ModName = ModName;

        // Use the bridge to load the mod assembly
        if (bIsBridgeInitialized && LoadModFunction)
        {
            FString ModPathStr = *ModPath;
            const char* ModPathCStr = TCHAR_TO_UTF8(*ModPathStr);
            int ModCount = LoadModFunction(const_cast<char*>(ModPathCStr));        if (ModCount <= 0)
        {
            LogDotNetError(TEXT("LoadMod"), FString::Printf(TEXT("Failed to load mod assembly: %s"), *ModPath));
            return false;
        }
        
        UE_LOG(LogTemp, Log, TEXT("DotNetHostManager: Loaded %d mod class(es) from assembly"), ModCount);
    }
    else
    {
        UE_LOG(LogTemp, Warning, TEXT("DotNetHostManager: Bridge not initialized, loading mod as placeholder"));
    }

    LoadedMods.Add(ModName, ModInterface);
    
    // Fire event
    OnModLoaded.Broadcast(ModName);
    
    UE_LOG(LogTemp, Log, TEXT("DotNetHostManager: Successfully loaded mod '%s'"), *ModName);
    return true;
}

bool UDotNetHostManager::UnloadMod(const FString& ModName)
{
    if (!LoadedMods.Contains(ModName))
    {
        UE_LOG(LogTemp, Warning, TEXT("DotNetHostManager: Mod '%s' is not loaded"), *ModName);
        return false;
    }

    UE_LOG(LogTemp, Log, TEXT("DotNetHostManager: Unloading mod '%s'"), *ModName);

    UDotNetModInterface* ModInterface = LoadedMods[ModName];
    if (ModInterface)
    {
        ModInterface->OnModUnloaded();
    }

    LoadedMods.Remove(ModName);
    
    // Fire event
    OnModUnloaded.Broadcast(ModName);
    
    UE_LOG(LogTemp, Log, TEXT("DotNetHostManager: Successfully unloaded mod '%s'"), *ModName);
    return true;
}

TArray<FString> UDotNetHostManager::GetLoadedMods() const
{
    TArray<FString> ModNames;
    LoadedMods.GenerateKeyArray(ModNames);
    return ModNames;
}

bool UDotNetHostManager::IsModLoaded(const FString& ModName) const
{
    return LoadedMods.Contains(ModName);
}

bool UDotNetHostManager::LoadBridgeAssembly(const FString& BridgeAssemblyPath)
{
    if (!bIsRuntimeInitialized)
    {
        LogDotNetError(TEXT("LoadBridgeAssembly"), TEXT("Runtime not initialized"));
        return false;
    }

    UE_LOG(LogTemp, Log, TEXT("DotNetHostManager: Loading bridge assembly from: %s"), *BridgeAssemblyPath);

    // Initialize the bridge assembly after runtime initialization
    InitializeBridgeAssembly();

    return true;
}

bool UDotNetHostManager::CallCSharpFunction(const FString& ClassName, const FString& MethodName, const TArray<FString>& Parameters, FString& Result)
{
    if (!bIsRuntimeInitialized)
    {
        LogDotNetError(TEXT("CallCSharpFunction"), TEXT("Runtime not initialized"));
        return false;
    }

    UE_LOG(LogTemp, Log, TEXT("DotNetHostManager: Calling C# function %s::%s"), *ClassName, *MethodName);

    // TODO: Implement actual C# function calling using the bridge assembly
    Result = TEXT("Not implemented yet");
    
    return true;
}

UDotNetHostManager* UDotNetHostManager::Get(const UObject* WorldContext)
{
    if (const UWorld* World = GEngine->GetWorldFromContextObject(WorldContext, EGetWorldErrorMode::LogAndReturnNull))
    {
        if (UGameInstance* GameInstance = World->GetGameInstance())
        {
            return GameInstance->GetSubsystem<UDotNetHostManager>();
        }
    }
    return nullptr;
}

FString UDotNetHostManager::GetDotNetRuntimeConfigPath() const
{
    // Look for the runtime config in the plugin's directory
    FString PluginDir = FPaths::Combine(FPaths::ProjectPluginsDir(), TEXT("DotNetScripting"));
    return FPaths::Combine(PluginDir, TEXT("Source"), TEXT("DotNetScripting"), TEXT("Private"), TEXT("DotNetScripting.runtimeconfig.json"));
}

FString UDotNetHostManager::GetBridgeAssemblyPath() const
{
    // Look for the bridge assembly in the plugin's binaries directory
    FString PluginDir = FPaths::Combine(FPaths::ProjectPluginsDir(), TEXT("DotNetScripting"));
    return FPaths::Combine(PluginDir, TEXT("Binaries"), TEXT("DotNet"), TEXT("UnrealEngine.Bridge.dll"));
}

void UDotNetHostManager::LogDotNetError(const FString& Context, const FString& Error)
{
    FString FullError = FString::Printf(TEXT("%s: %s"), *Context, *Error);
    UE_LOG(LogTemp, Error, TEXT("DotNetHostManager: %s"), *FullError);
    
    // Fire error event
    OnModError.Broadcast(Context, Error);
}

void UDotNetHostManager::InitializeBridgeAssembly()
{
    if (!LoadAssemblyAndGetFunctionPointer)
    {
        UE_LOG(LogTemp, Error, TEXT("Cannot initialize bridge assembly: .NET runtime not initialized"));
        return;
    }

    // Load the bridge assembly
    FString BridgeAssemblyPath = FPaths::Combine(
        FPaths::ProjectPluginsDir(),
        TEXT("DotNetScripting"),
        TEXT("DotNet"),
        TEXT("UnrealEngine.Bridge"),
        TEXT("bin"),
        TEXT("x64"),
        TEXT("Release"),
        TEXT("net9.0"),
        TEXT("UnrealEngine.Bridge.dll")
    );

    if (!FPlatformFileManager::Get().GetPlatformFile().FileExists(*BridgeAssemblyPath))
    {
        UE_LOG(LogTemp, Error, TEXT("Bridge assembly not found at: %s"), *BridgeAssemblyPath);
        return;
    }

    // Load the bridge assembly and get the initialize function
    const char_t* AssemblyPathStr = *BridgeAssemblyPath;
    const char_t* TypeName = TEXT("UnrealEngine.Bridge.BridgeEntryPoint, UnrealEngine.Bridge");
    const char_t* MethodName = TEXT("Initialize");

    component_entry_point_fn InitializeFunction = nullptr;
    int result = LoadAssemblyAndGetFunctionPointer(
        AssemblyPathStr,
        TypeName,
        MethodName,
        UNMANAGEDCALLERSONLY_METHOD,
        nullptr,
        (void**)&InitializeFunction
    );

    if (result != 0 || !InitializeFunction)
    {
        UE_LOG(LogTemp, Error, TEXT("Failed to get bridge initialize function. Error: %d"), result);
        return;
    }

    // Create function pointers for UE functionality
    static auto LogFunction = [](int InLogLevel, const char* Category, const char* Message) -> void {
        FString CategoryStr = UTF8_TO_TCHAR(Category);
        FString MessageStr = UTF8_TO_TCHAR(Message);
        
        switch (InLogLevel)
        {
        case 0: // Fatal
            UE_LOG(LogTemp, Fatal, TEXT("[%s] %s"), *CategoryStr, *MessageStr);
            break;
        case 1: // Error
            UE_LOG(LogTemp, Error, TEXT("[%s] %s"), *CategoryStr, *MessageStr);
            break;
        case 2: // Warning
            UE_LOG(LogTemp, Warning, TEXT("[%s] %s"), *CategoryStr, *MessageStr);
            break;
        case 3: // Display
            UE_LOG(LogTemp, Display, TEXT("[%s] %s"), *CategoryStr, *MessageStr);
            break;
        case 4: // Info
            UE_LOG(LogTemp, Log, TEXT("[%s] %s"), *CategoryStr, *MessageStr);
            break;
        case 5: // Verbose
            UE_LOG(LogTemp, Verbose, TEXT("[%s] %s"), *CategoryStr, *MessageStr);
            break;
        case 6: // VeryVerbose
            UE_LOG(LogTemp, VeryVerbose, TEXT("[%s] %s"), *CategoryStr, *MessageStr);
            break;
        default:
            UE_LOG(LogTemp, Log, TEXT("[%s] %s"), *CategoryStr, *MessageStr);
            break;
        }
    };

    static auto SpawnActorFunction = [](const char* ClassName, float X, float Y, float Z, float RX, float RY, float RZ) -> void* {
        // TODO: Implement actor spawning - expose UWorld::SpawnActor functionality
        FString ClassNameStr = UTF8_TO_TCHAR(ClassName);
        FVector Location(X, Y, Z);
        FRotator Rotation(RX, RY, RZ);
        
        UE_LOG(LogTemp, Log, TEXT("C# Mod requested spawn of %s at (%f, %f, %f)"), *ClassNameStr, X, Y, Z);
        return nullptr; // For now
    };

    // Store function pointers 
    LogFunctionPtr = reinterpret_cast<void*>(+LogFunction);
    SpawnActorFunctionPtr = reinterpret_cast<void*>(+SpawnActorFunction);

    // Initialize the bridge with our function pointers
    result = InitializeFunction(LogFunctionPtr, SpawnActorFunctionPtr);
    
    if (result == 0)
    {
        UE_LOG(LogTemp, Log, TEXT("Bridge assembly initialized successfully"));
        bIsBridgeInitialized = true;

        // Get the LoadMod function
        MethodName = TEXT("LoadMod");
        result = LoadAssemblyAndGetFunctionPointer(
            AssemblyPathStr,
            TypeName,
            MethodName,
            UNMANAGEDCALLERSONLY_METHOD,
            nullptr,
            (void**)&LoadModFunction
        );

        if (result != 0 || !LoadModFunction)
        {
            UE_LOG(LogTemp, Error, TEXT("Failed to get LoadMod function. Error: %d"), result);
        }

        // Get the TickMods function
        MethodName = TEXT("TickMods");
        result = LoadAssemblyAndGetFunctionPointer(
            AssemblyPathStr,
            TypeName,
            MethodName,
            UNMANAGEDCALLERSONLY_METHOD,
            nullptr,
            (void**)&TickModsFunction
        );

        if (result != 0 || !TickModsFunction)
        {
            UE_LOG(LogTemp, Error, TEXT("Failed to get TickMods function. Error: %d"), result);
        }
    }
    else
    {
        UE_LOG(LogTemp, Error, TEXT("Failed to initialize bridge assembly. Error: %d"), result);
    }
}
