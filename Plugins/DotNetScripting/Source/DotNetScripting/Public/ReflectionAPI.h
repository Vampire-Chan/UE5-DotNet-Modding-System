#pragma once

#include "CoreMinimal.h"
#include "Engine/World.h"
#include "UObject/Class.h"
#include "UObject/Property.h"
#include "UObject/UnrealType.h"
#include "GameFramework/Actor.h"
#include "Components/ActorComponent.h"

/**
 * UE5 Reflection API Bridge for .NET Integration
 * Provides real-time access to Unreal Engine's reflection system from C#
 */

// Forward declarations
struct FReflectionProperty;
struct FReflectionFunction;
struct FReflectionClass;

// Export macros for C# interop
#define REFLECTION_API extern "C" __declspec(dllexport)

// =================================================================================
// CORE REFLECTION TYPES
// =================================================================================

/**
 * Represents a UE5 property accessible from C#
 */
struct FReflectionProperty
{
    char Name[256];
    char TypeName[128];
    int32 PropertyType;  // EReflectionPropertyType
    int32 Offset;
    int32 Size;
    bool bIsArray;
    bool bIsPointer;
    bool bIsStruct;
    void* PropertyPtr;
};

/**
 * Represents a UE5 function callable from C#
 */
struct FReflectionFunction
{
    char Name[256];
    char ReturnTypeName[128];
    int32 NumParameters;
    int32 ParamsSize;
    bool bIsStatic;
    bool bIsBlueprintCallable;
    void* FunctionPtr;
};

/**
 * Represents a UE5 class accessible from C#
 */
struct FReflectionClass
{
    char Name[256];
    char ParentName[256];
    int32 ClassSize;
    int32 NumProperties;
    int32 NumFunctions;
    bool bIsActor;
    bool bIsComponent;
    bool bIsBlueprintable;
    void* ClassPtr;
};

/**
 * Property types for reflection
 */
enum class EReflectionPropertyType : int32
{
    Bool = 0,
    Int8,
    Int16,
    Int32,
    Int64,
    UInt8,
    UInt16,
    UInt32,
    UInt64,
    Float,
    Double,
    String,
    Text,
    Name,
    Vector,
    Rotator,
    Transform,
    Color,
    Object,
    Class,
    Struct,
    Array,
    Map,
    Set,
    Enum,
    Unknown
};

// =================================================================================
// REFLECTION CORE API
// =================================================================================

/**
 * Initialize the reflection system
 */
REFLECTION_API bool InitializeReflectionSystem();

/**
 * Shutdown the reflection system
 */
REFLECTION_API void ShutdownReflectionSystem();

/**
 * Get all available classes in the engine
 */
REFLECTION_API int32 GetAllClasses(FReflectionClass* OutClasses, int32 MaxClasses);

/**
 * Find a class by name
 */
REFLECTION_API bool FindClass(const char* ClassName, FReflectionClass* OutClass);

/**
 * Get properties of a class
 */
REFLECTION_API int32 GetClassProperties(const char* ClassName, FReflectionProperty* OutProperties, int32 MaxProperties);

/**
 * Get functions of a class
 */
REFLECTION_API int32 GetClassFunctions(const char* ClassName, FReflectionFunction* OutFunctions, int32 MaxFunctions);

// =================================================================================
// OBJECT MANIPULATION API
// =================================================================================

/**
 * Create an instance of a class
 */
REFLECTION_API void* CreateObject(const char* ClassName, void* Outer = nullptr);

/**
 * Destroy an object instance
 */
REFLECTION_API bool DestroyObject(void* Object);

/**
 * Get property value from an object
 */
REFLECTION_API bool GetPropertyValue(void* Object, const char* PropertyName, void* OutValue, int32 ValueSize);

/**
 * Set property value on an object
 */
REFLECTION_API bool SetPropertyValue(void* Object, const char* PropertyName, const void* Value, int32 ValueSize);

/**
 * Call a function on an object
 */
REFLECTION_API bool CallFunction(void* Object, const char* FunctionName, void* Parameters, void* ReturnValue);

// =================================================================================
// WORLD AND ACTOR API
// =================================================================================

/**
 * Get the current world
 */
REFLECTION_API void* GetCurrentWorld();

/**
 * Spawn an actor in the world
 */
REFLECTION_API void* SpawnActor(const char* ClassName, float X, float Y, float Z, float Pitch, float Yaw, float Roll);

/**
 * Find actor by name
 */
REFLECTION_API void* FindActorByName(const char* ActorName);

/**
 * Get all actors of a specific class
 */
REFLECTION_API int32 GetAllActorsOfClass(const char* ClassName, void** OutActors, int32 MaxActors);

/**
 * Get actor location
 */
REFLECTION_API bool GetActorLocation(void* Actor, float* OutX, float* OutY, float* OutZ);

/**
 * Set actor location
 */
REFLECTION_API bool SetActorLocation(void* Actor, float X, float Y, float Z);

/**
 * Get actor rotation
 */
REFLECTION_API bool GetActorRotation(void* Actor, float* OutPitch, float* OutYaw, float* OutRoll);

/**
 * Set actor rotation
 */
REFLECTION_API bool SetActorRotation(void* Actor, float Pitch, float Yaw, float Roll);

// =================================================================================
// COMPONENT API
// =================================================================================

/**
 * Add component to actor
 */
REFLECTION_API void* AddComponent(void* Actor, const char* ComponentClassName);

/**
 * Get component from actor
 */
REFLECTION_API void* GetComponent(void* Actor, const char* ComponentClassName);

/**
 * Remove component from actor
 */
REFLECTION_API bool RemoveComponent(void* Actor, void* Component);

// =================================================================================
// BLUEPRINT INTEGRATION API
// =================================================================================

/**
 * Load blueprint class
 */
REFLECTION_API void* LoadBlueprintClass(const char* BlueprintPath);

/**
 * Call blueprint function
 */
REFLECTION_API bool CallBlueprintFunction(void* Object, const char* FunctionName, void* Parameters, void* ReturnValue);

/**
 * Get blueprint property
 */
REFLECTION_API bool GetBlueprintProperty(void* Object, const char* PropertyName, void* OutValue, int32 ValueSize);

/**
 * Set blueprint property
 */
REFLECTION_API bool SetBlueprintProperty(void* Object, const char* PropertyName, const void* Value, int32 ValueSize);

// =================================================================================
// REAL-TIME DELEGATES AND EVENTS
// =================================================================================

/**
 * Delegate callback function pointer type
 */
typedef void(*FReflectionDelegate)(void* Object, const char* EventName, void* EventData);

/**
 * Bind to an object's delegate/event
 */
REFLECTION_API bool BindToDelegate(void* Object, const char* DelegateName, FReflectionDelegate Callback);

/**
 * Unbind from an object's delegate/event
 */
REFLECTION_API bool UnbindFromDelegate(void* Object, const char* DelegateName, FReflectionDelegate Callback);

/**
 * Trigger a delegate/event
 */
REFLECTION_API bool TriggerDelegate(void* Object, const char* DelegateName, void* EventData);

// =================================================================================
// TYPE CONVERSION UTILITIES
// =================================================================================

/**
 * Convert UE5 property type to reflection type
 */
REFLECTION_API EReflectionPropertyType GetReflectionPropertyType(const char* UE5TypeName);

/**
 * Get size of a reflection property type
 */
REFLECTION_API int32 GetReflectionPropertySize(EReflectionPropertyType PropertyType);

/**
 * Check if type is compatible with C# marshaling
 */
REFLECTION_API bool IsTypeMarshallable(EReflectionPropertyType PropertyType);

// =================================================================================
// DEBUGGING AND UTILITIES
// =================================================================================

/**
 * Get object class name
 */
REFLECTION_API bool GetObjectClassName(void* Object, char* OutClassName, int32 MaxLength);

/**
 * Check if object is valid
 */
REFLECTION_API bool IsObjectValid(void* Object);

/**
 * Print object properties (for debugging)
 */
REFLECTION_API void PrintObjectProperties(void* Object);

/**
 * Get reflection system statistics
 */
REFLECTION_API void GetReflectionStats(int32* OutNumClasses, int32* OutNumProperties, int32* OutNumFunctions);
