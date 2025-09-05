#pragma once

#include "CoreMinimal.h"
#include "UObject/NoExportTypes.h"
#include "Engine/Engine.h"
#include "Subsystems/GameInstanceSubsystem.h"

// .NET hosting includes
#include "nethost.h"
#include "coreclr_delegates.h"
#include "hostfxr.h"

#include "DotNetHostManager.generated.h"

// Forward declarations
class UDotNetModInterface;

/**
 * Delegate for .NET mod events
 */
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnModLoaded, const FString&, ModName);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnModUnloaded, const FString&, ModName);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FOnModError, const FString&, ModName, const FString&, Error);

/**
 * .NET Runtime Host Manager
 * Manages the .NET runtime, loads/unloads assemblies, and provides the bridge between UE and C#
 */
UCLASS(BlueprintType, Blueprintable)
class DOTNETSCRIPTING_API UDotNetHostManager : public UGameInstanceSubsystem
{
    GENERATED_BODY()

public:
    UDotNetHostManager();

    // USubsystem interface
    virtual void Initialize(FSubsystemCollectionBase& Collection) override;
    virtual void Deinitialize() override;

    // Core .NET hosting functions
    UFUNCTION(BlueprintCallable, Category = "DotNet Host")
    bool InitializeDotNetRuntime();

    UFUNCTION(BlueprintCallable, Category = "DotNet Host")
    void ShutdownDotNetRuntime();

    UFUNCTION(BlueprintCallable, Category = "DotNet Host")
    bool IsRuntimeInitialized() const { return bIsRuntimeInitialized; }

    // Mod management
    UFUNCTION(BlueprintCallable, Category = "DotNet Mods")
    bool LoadMod(const FString& ModPath, const FString& ModName);

    UFUNCTION(BlueprintCallable, Category = "DotNet Mods")
    bool UnloadMod(const FString& ModName);

    UFUNCTION(BlueprintCallable, Category = "DotNet Mods")
    TArray<FString> GetLoadedMods() const;

    UFUNCTION(BlueprintCallable, Category = "DotNet Mods")
    bool IsModLoaded(const FString& ModName) const;

    // Bridge assembly management
    UFUNCTION(BlueprintCallable, Category = "DotNet Bridge")
    bool LoadBridgeAssembly(const FString& BridgeAssemblyPath);

    // Interop functions for calling C# from C++
    UFUNCTION(BlueprintCallable, Category = "DotNet Interop")
    bool CallCSharpFunction(const FString& ClassName, const FString& MethodName, const TArray<FString>& Parameters, FString& Result);

    // Events
    UPROPERTY(BlueprintAssignable, Category = "DotNet Events")
    FOnModLoaded OnModLoaded;

    UPROPERTY(BlueprintAssignable, Category = "DotNet Events")
    FOnModUnloaded OnModUnloaded;

    UPROPERTY(BlueprintAssignable, Category = "DotNet Events")
    FOnModError OnModError;

    // Static access for C++ code
    static UDotNetHostManager* Get(const UObject* WorldContext);

private:
    // .NET runtime state
    bool bIsRuntimeInitialized = false;
    bool bIsBridgeInitialized = false;
    hostfxr_handle RuntimeContext = nullptr;
    load_assembly_and_get_function_pointer_fn LoadAssemblyAndGetFunctionPointer = nullptr;

    // Bridge function pointers
    typedef int (*component_entry_point_fn)(void*, void*);
    typedef int (*load_mod_fn)(void*);
    typedef void (*tick_mods_fn)(float);
    
    component_entry_point_fn InitializeBridgeFunction = nullptr;
    load_mod_fn LoadModFunction = nullptr;
    tick_mods_fn TickModsFunction = nullptr;
    
    void* LogFunctionPtr = nullptr;
    void* SpawnActorFunctionPtr = nullptr;

    // Loaded mods tracking
    UPROPERTY()
    TMap<FString, UDotNetModInterface*> LoadedMods;

    // Bridge assembly function pointers
    TMap<FString, void*> BridgeFunctions;

    // Internal helper functions
    bool InitializeHostFxr();
    bool LoadHostFxr();
    void InitializeBridgeAssembly();
    FString GetDotNetRuntimeConfigPath() const;
    FString GetBridgeAssemblyPath() const;
    void LogDotNetError(const FString& Context, const FString& Error);

    // Function pointer types for .NET hosting
    hostfxr_initialize_for_runtime_config_fn HostFxrInitializeForRuntimeConfig = nullptr;
    hostfxr_get_runtime_delegate_fn HostFxrGetRuntimeDelegate = nullptr;
    hostfxr_close_fn HostFxrClose = nullptr;

    // Library handles
    void* HostFxrLibrary = nullptr;
};

/**
 * Interface for .NET mod instances
 */
UCLASS(BlueprintType, Abstract)
class DOTNETSCRIPTING_API UDotNetModInterface : public UObject
{
    GENERATED_BODY()

public:
    UFUNCTION(BlueprintImplementableEvent, Category = "DotNet Mod")
    void OnModLoaded();

    UFUNCTION(BlueprintImplementableEvent, Category = "DotNet Mod")
    void OnModUnloaded();

    UFUNCTION(BlueprintImplementableEvent, Category = "DotNet Mod")
    void OnTick(float DeltaTime);

    // Mod metadata
    UPROPERTY(BlueprintReadOnly, Category = "DotNet Mod")
    FString ModName;

    UPROPERTY(BlueprintReadOnly, Category = "DotNet Mod")
    FString ModVersion;

    UPROPERTY(BlueprintReadOnly, Category = "DotNet Mod")
    FString ModAuthor;

    UPROPERTY(BlueprintReadOnly, Category = "DotNet Mod")
    FString ModDescription;
};
