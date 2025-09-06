#include "ReflectionAPI.h"
#include "Engine/Engine.h"
#include "Engine/World.h"
#include "UObject/UObjectIterator.h"
#include "UObject/UnrealType.h"
#include "UObject/PropertyTag.h"
#include "GameFramework/Actor.h"
#include "Components/ActorComponent.h"
#include "Blueprint/BlueprintSupport.h"
#include "Engine/Blueprint.h"
#include "UObject/Class.h"

/**
 * Implementation of UE5 Reflection API Bridge
 * Provides real-time access to Unreal Engine's reflection system from C#
 */

// Global reflection system state
static bool bReflectionSystemInitialized = false;
static TMap<FString, UClass*> CachedClasses;
static TMap<void*, TWeakObjectPtr<UObject>> ObjectRegistry;

// =================================================================================
// UTILITY FUNCTIONS
// =================================================================================

/**
 * Convert UProperty to FReflectionProperty
 */
static void ConvertToReflectionProperty(const FProperty* Property, FReflectionProperty& OutReflectionProp)
{
    // Copy property name
    FString PropertyName = Property->GetName();
    FCStringAnsi::Strncpy(OutReflectionProp.Name, TCHAR_TO_ANSI(*PropertyName), sizeof(OutReflectionProp.Name) - 1);
    OutReflectionProp.Name[sizeof(OutReflectionProp.Name) - 1] = '\0';

    // Copy type name
    FString TypeName = Property->GetClass()->GetName();
    FCStringAnsi::Strncpy(OutReflectionProp.TypeName, TCHAR_TO_ANSI(*TypeName), sizeof(OutReflectionProp.TypeName) - 1);
    OutReflectionProp.TypeName[sizeof(OutReflectionProp.TypeName) - 1] = '\0';

    // Set property info
    OutReflectionProp.Offset = Property->GetOffset_ForInternal();
    OutReflectionProp.Size = Property->GetSize();
    OutReflectionProp.bIsArray = Property->IsA<FArrayProperty>();
    OutReflectionProp.bIsPointer = Property->IsA<FObjectProperty>() || Property->IsA<FClassProperty>();
    OutReflectionProp.bIsStruct = Property->IsA<FStructProperty>();
    OutReflectionProp.PropertyPtr = const_cast<FProperty*>(Property);

    // Determine property type
    if (Property->IsA<FBoolProperty>())
    {
        OutReflectionProp.PropertyType = static_cast<int32>(EReflectionPropertyType::Bool);
    }
    else if (Property->IsA<FInt8Property>())
    {
        OutReflectionProp.PropertyType = static_cast<int32>(EReflectionPropertyType::Int8);
    }
    else if (Property->IsA<FInt16Property>())
    {
        OutReflectionProp.PropertyType = static_cast<int32>(EReflectionPropertyType::Int16);
    }
    else if (Property->IsA<FIntProperty>())
    {
        OutReflectionProp.PropertyType = static_cast<int32>(EReflectionPropertyType::Int32);
    }
    else if (Property->IsA<FInt64Property>())
    {
        OutReflectionProp.PropertyType = static_cast<int32>(EReflectionPropertyType::Int64);
    }
    else if (Property->IsA<FFloatProperty>())
    {
        OutReflectionProp.PropertyType = static_cast<int32>(EReflectionPropertyType::Float);
    }
    else if (Property->IsA<FDoubleProperty>())
    {
        OutReflectionProp.PropertyType = static_cast<int32>(EReflectionPropertyType::Double);
    }
    else if (Property->IsA<FStrProperty>())
    {
        OutReflectionProp.PropertyType = static_cast<int32>(EReflectionPropertyType::String);
    }
    else if (Property->IsA<FNameProperty>())
    {
        OutReflectionProp.PropertyType = static_cast<int32>(EReflectionPropertyType::Name);
    }
    else if (Property->IsA<FObjectProperty>())
    {
        OutReflectionProp.PropertyType = static_cast<int32>(EReflectionPropertyType::Object);
    }
    else if (Property->IsA<FClassProperty>())
    {
        OutReflectionProp.PropertyType = static_cast<int32>(EReflectionPropertyType::Class);
    }
    else if (Property->IsA<FStructProperty>())
    {
        OutReflectionProp.PropertyType = static_cast<int32>(EReflectionPropertyType::Struct);
    }
    else if (Property->IsA<FArrayProperty>())
    {
        OutReflectionProp.PropertyType = static_cast<int32>(EReflectionPropertyType::Array);
    }
    else if (Property->IsA<FEnumProperty>())
    {
        OutReflectionProp.PropertyType = static_cast<int32>(EReflectionPropertyType::Enum);
    }
    else
    {
        OutReflectionProp.PropertyType = static_cast<int32>(EReflectionPropertyType::Unknown);
    }
}

/**
 * Convert UFunction to FReflectionFunction
 */
static void ConvertToReflectionFunction(const UFunction* Function, FReflectionFunction& OutReflectionFunc)
{
    // Copy function name
    FString FunctionName = Function->GetName();
    FCStringAnsi::Strncpy(OutReflectionFunc.Name, TCHAR_TO_ANSI(*FunctionName), sizeof(OutReflectionFunc.Name) - 1);
    OutReflectionFunc.Name[sizeof(OutReflectionFunc.Name) - 1] = '\0';

    // Get return property
    FProperty* ReturnProperty = Function->GetReturnProperty();
    if (ReturnProperty)
    {
        FString ReturnTypeName = ReturnProperty->GetClass()->GetName();
        FCStringAnsi::Strncpy(OutReflectionFunc.ReturnTypeName, TCHAR_TO_ANSI(*ReturnTypeName), sizeof(OutReflectionFunc.ReturnTypeName) - 1);
    }
    else
    {
        FCStringAnsi::Strncpy(OutReflectionFunc.ReturnTypeName, "void", sizeof(OutReflectionFunc.ReturnTypeName) - 1);
    }
    OutReflectionFunc.ReturnTypeName[sizeof(OutReflectionFunc.ReturnTypeName) - 1] = '\0';

    // Set function info
    OutReflectionFunc.NumParameters = Function->NumParms;
    OutReflectionFunc.ParamsSize = Function->ParmsSize;
    OutReflectionFunc.bIsStatic = Function->HasAnyFunctionFlags(FUNC_Static);
    OutReflectionFunc.bIsBlueprintCallable = Function->HasAnyFunctionFlags(FUNC_BlueprintCallable);
    OutReflectionFunc.FunctionPtr = const_cast<UFunction*>(Function);
}

/**
 * Convert UClass to FReflectionClass
 */
static void ConvertToReflectionClass(const UClass* Class, FReflectionClass& OutReflectionClass)
{
    // Copy class name
    FString ClassName = Class->GetName();
    FCStringAnsi::Strncpy(OutReflectionClass.Name, TCHAR_TO_ANSI(*ClassName), sizeof(OutReflectionClass.Name) - 1);
    OutReflectionClass.Name[sizeof(OutReflectionClass.Name) - 1] = '\0';

    // Copy parent class name
    UClass* ParentClass = Class->GetSuperClass();
    if (ParentClass)
    {
        FString ParentClassName = ParentClass->GetName();
        FCStringAnsi::Strncpy(OutReflectionClass.ParentName, TCHAR_TO_ANSI(*ParentClassName), sizeof(OutReflectionClass.ParentName) - 1);
    }
    else
    {
        FCStringAnsi::Strncpy(OutReflectionClass.ParentName, "None", sizeof(OutReflectionClass.ParentName) - 1);
    }
    OutReflectionClass.ParentName[sizeof(OutReflectionClass.ParentName) - 1] = '\0';

    // Set class info
    OutReflectionClass.ClassSize = Class->GetStructureSize();
    OutReflectionClass.NumProperties = 0;
    OutReflectionClass.NumFunctions = 0;
    
    // Count properties and functions
    for (FProperty* Property = Class->PropertyLink; Property; Property = Property->PropertyLinkNext)
    {
        OutReflectionClass.NumProperties++;
    }
    
    for (TFieldIterator<UFunction> FuncIt(Class); FuncIt; ++FuncIt)
    {
        OutReflectionClass.NumFunctions++;
    }

    OutReflectionClass.bIsActor = Class->IsChildOf(AActor::StaticClass());
    OutReflectionClass.bIsComponent = Class->IsChildOf(UActorComponent::StaticClass());
    OutReflectionClass.bIsBlueprintable = Class->HasAnyClassFlags(CLASS_Blueprintable);
    OutReflectionClass.ClassPtr = const_cast<UClass*>(Class);
}

// =================================================================================
// REFLECTION CORE API IMPLEMENTATION
// =================================================================================

REFLECTION_API bool InitializeReflectionSystem()
{
    if (bReflectionSystemInitialized)
    {
        return true;
    }

    UE_LOG(LogTemp, Log, TEXT("Initializing Reflection System..."));

    // Cache commonly used classes
    CachedClasses.Empty();
    
    for (TObjectIterator<UClass> ClassIt; ClassIt; ++ClassIt)
    {
        UClass* Class = *ClassIt;
        if (Class && !Class->HasAnyClassFlags(CLASS_Deprecated | CLASS_NewerVersionExists))
        {
            CachedClasses.Add(Class->GetName(), Class);
        }
    }

    bReflectionSystemInitialized = true;
    
    UE_LOG(LogTemp, Log, TEXT("Reflection System initialized with %d classes"), CachedClasses.Num());
    return true;
}

REFLECTION_API void ShutdownReflectionSystem()
{
    if (!bReflectionSystemInitialized)
    {
        return;
    }

    UE_LOG(LogTemp, Log, TEXT("Shutting down Reflection System..."));

    CachedClasses.Empty();
    ObjectRegistry.Empty();
    
    bReflectionSystemInitialized = false;
}

REFLECTION_API int32 GetAllClasses(FReflectionClass* OutClasses, int32 MaxClasses)
{
    if (!bReflectionSystemInitialized || !OutClasses || MaxClasses <= 0)
    {
        return 0;
    }

    int32 ClassCount = 0;
    
    for (auto& ClassPair : CachedClasses)
    {
        if (ClassCount >= MaxClasses)
        {
            break;
        }

        UClass* Class = ClassPair.Value;
        if (Class && IsValid(Class))
        {
            ConvertToReflectionClass(Class, OutClasses[ClassCount]);
            ClassCount++;
        }
    }

    return ClassCount;
}

REFLECTION_API bool FindClass(const char* ClassName, FReflectionClass* OutClass)
{
    if (!bReflectionSystemInitialized || !ClassName || !OutClass)
    {
        return false;
    }

    FString ClassNameStr = FString(ANSI_TO_TCHAR(ClassName));
    
    UClass** FoundClass = CachedClasses.Find(ClassNameStr);
    if (FoundClass && *FoundClass && IsValid(*FoundClass))
    {
        ConvertToReflectionClass(*FoundClass, *OutClass);
        return true;
    }

    return false;
}

REFLECTION_API int32 GetClassProperties(const char* ClassName, FReflectionProperty* OutProperties, int32 MaxProperties)
{
    if (!bReflectionSystemInitialized || !ClassName || !OutProperties || MaxProperties <= 0)
    {
        return 0;
    }

    FString ClassNameStr = FString(ANSI_TO_TCHAR(ClassName));
    UClass** FoundClass = CachedClasses.Find(ClassNameStr);
    
    if (!FoundClass || !*FoundClass || !IsValid(*FoundClass))
    {
        return 0;
    }

    UClass* Class = *FoundClass;
    int32 PropertyCount = 0;

    for (FProperty* Property = Class->PropertyLink; Property && PropertyCount < MaxProperties; Property = Property->PropertyLinkNext)
    {
        ConvertToReflectionProperty(Property, OutProperties[PropertyCount]);
        PropertyCount++;
    }

    return PropertyCount;
}

REFLECTION_API int32 GetClassFunctions(const char* ClassName, FReflectionFunction* OutFunctions, int32 MaxFunctions)
{
    if (!bReflectionSystemInitialized || !ClassName || !OutFunctions || MaxFunctions <= 0)
    {
        return 0;
    }

    FString ClassNameStr = FString(ANSI_TO_TCHAR(ClassName));
    UClass** FoundClass = CachedClasses.Find(ClassNameStr);
    
    if (!FoundClass || !*FoundClass || !IsValid(*FoundClass))
    {
        return 0;
    }

    UClass* Class = *FoundClass;
    int32 FunctionCount = 0;

    for (TFieldIterator<UFunction> FuncIt(Class); FuncIt && FunctionCount < MaxFunctions; ++FuncIt)
    {
        UFunction* Function = *FuncIt;
        if (Function)
        {
            ConvertToReflectionFunction(Function, OutFunctions[FunctionCount]);
            FunctionCount++;
        }
    }

    return FunctionCount;
}

// =================================================================================
// OBJECT MANIPULATION API IMPLEMENTATION
// =================================================================================

REFLECTION_API void* CreateObject(const char* ClassName, void* Outer)
{
    if (!bReflectionSystemInitialized || !ClassName)
    {
        return nullptr;
    }

    FString ClassNameStr = FString(ANSI_TO_TCHAR(ClassName));
    UClass** FoundClass = CachedClasses.Find(ClassNameStr);
    
    if (!FoundClass || !*FoundClass || !IsValid(*FoundClass))
    {
        return nullptr;
    }

    UClass* Class = *FoundClass;
    UObject* OuterObject = Outer ? static_cast<UObject*>(Outer) : GetTransientPackage();
    
    UObject* NewObject = NewObject<UObject>(OuterObject, Class);
    if (NewObject)
    {
        ObjectRegistry.Add(NewObject, NewObject);
        return NewObject;
    }

    return nullptr;
}

REFLECTION_API bool DestroyObject(void* Object)
{
    if (!Object)
    {
        return false;
    }

    UObject* UObj = static_cast<UObject*>(Object);
    if (!IsValid(UObj))
    {
        return false;
    }

    ObjectRegistry.Remove(Object);
    
    // Mark for garbage collection
    UObj->MarkAsGarbage();
    return true;
}

REFLECTION_API bool GetPropertyValue(void* Object, const char* PropertyName, void* OutValue, int32 ValueSize)
{
    if (!Object || !PropertyName || !OutValue || ValueSize <= 0)
    {
        return false;
    }

    UObject* UObj = static_cast<UObject*>(Object);
    if (!IsValid(UObj))
    {
        return false;
    }

    FString PropertyNameStr = FString(ANSI_TO_TCHAR(PropertyName));
    FProperty* Property = UObj->GetClass()->FindPropertyByName(*PropertyNameStr);
    
    if (!Property)
    {
        return false;
    }

    // Simple value copy - more complex types need special handling
    if (Property->GetSize() <= ValueSize)
    {
        Property->CopyCompleteValue(OutValue, Property->ContainerPtrToValuePtr<void>(UObj));
        return true;
    }

    return false;
}

REFLECTION_API bool SetPropertyValue(void* Object, const char* PropertyName, const void* Value, int32 ValueSize)
{
    if (!Object || !PropertyName || !Value || ValueSize <= 0)
    {
        return false;
    }

    UObject* UObj = static_cast<UObject*>(Object);
    if (!IsValid(UObj))
    {
        return false;
    }

    FString PropertyNameStr = FString(ANSI_TO_TCHAR(PropertyName));
    FProperty* Property = UObj->GetClass()->FindPropertyByName(*PropertyNameStr);
    
    if (!Property)
    {
        return false;
    }

    // Simple value copy - more complex types need special handling
    if (Property->GetSize() <= ValueSize)
    {
        Property->CopyCompleteValue(Property->ContainerPtrToValuePtr<void>(UObj), Value);
        return true;
    }

    return false;
}

REFLECTION_API bool CallFunction(void* Object, const char* FunctionName, void* Parameters, void* ReturnValue)
{
    if (!Object || !FunctionName)
    {
        return false;
    }

    UObject* UObj = static_cast<UObject*>(Object);
    if (!IsValid(UObj))
    {
        return false;
    }

    FString FunctionNameStr = FString(ANSI_TO_TCHAR(FunctionName));
    UFunction* Function = UObj->GetClass()->FindFunctionByName(*FunctionNameStr);
    
    if (!Function)
    {
        return false;
    }

    // Allocate parameter buffer
    uint8* ParamBuffer = nullptr;
    if (Function->ParmsSize > 0)
    {
        ParamBuffer = (uint8*)FMemory::Malloc(Function->ParmsSize);
        FMemory::Memzero(ParamBuffer, Function->ParmsSize);
        
        // Copy parameters if provided
        if (Parameters)
        {
            FMemory::Memcpy(ParamBuffer, Parameters, Function->ParmsSize);
        }
    }

    // Call the function
    UObj->ProcessEvent(Function, ParamBuffer);

    // Copy return value if function has one
    if (ReturnValue && Function->GetReturnProperty())
    {
        FProperty* ReturnProperty = Function->GetReturnProperty();
        ReturnProperty->CopyCompleteValue(ReturnValue, ReturnProperty->ContainerPtrToValuePtr<void>(ParamBuffer));
    }

    // Clean up
    if (ParamBuffer)
    {
        FMemory::Free(ParamBuffer);
    }

    return true;
}

// =================================================================================
// WORLD AND ACTOR API IMPLEMENTATION
// =================================================================================

REFLECTION_API void* GetCurrentWorld()
{
    if (GEngine && GEngine->GetWorldContexts().Num() > 0)
    {
        return GEngine->GetWorldContexts()[0].World();
    }
    return nullptr;
}

REFLECTION_API void* SpawnActor(const char* ClassName, float X, float Y, float Z, float Pitch, float Yaw, float Roll)
{
    UWorld* World = static_cast<UWorld*>(GetCurrentWorld());
    if (!World || !ClassName)
    {
        return nullptr;
    }

    FString ClassNameStr = FString(ANSI_TO_TCHAR(ClassName));
    UClass** FoundClass = CachedClasses.Find(ClassNameStr);
    
    if (!FoundClass || !*FoundClass || !IsValid(*FoundClass))
    {
        return nullptr;
    }

    UClass* ActorClass = *FoundClass;
    if (!ActorClass->IsChildOf(AActor::StaticClass()))
    {
        return nullptr;
    }

    FVector Location(X, Y, Z);
    FRotator Rotation(Pitch, Yaw, Roll);
    FActorSpawnParameters SpawnParams;
    
    AActor* SpawnedActor = World->SpawnActor<AActor>(ActorClass, Location, Rotation, SpawnParams);
    if (SpawnedActor)
    {
        ObjectRegistry.Add(SpawnedActor, SpawnedActor);
        return SpawnedActor;
    }

    return nullptr;
}

REFLECTION_API void* FindActorByName(const char* ActorName)
{
    UWorld* World = static_cast<UWorld*>(GetCurrentWorld());
    if (!World || !ActorName)
    {
        return nullptr;
    }

    FString ActorNameStr = FString(ANSI_TO_TCHAR(ActorName));
    
    for (TActorIterator<AActor> ActorItr(World); ActorItr; ++ActorItr)
    {
        AActor* Actor = *ActorItr;
        if (Actor && Actor->GetName() == ActorNameStr)
        {
            return Actor;
        }
    }

    return nullptr;
}

REFLECTION_API int32 GetAllActorsOfClass(const char* ClassName, void** OutActors, int32 MaxActors)
{
    UWorld* World = static_cast<UWorld*>(GetCurrentWorld());
    if (!World || !ClassName || !OutActors || MaxActors <= 0)
    {
        return 0;
    }

    FString ClassNameStr = FString(ANSI_TO_TCHAR(ClassName));
    UClass** FoundClass = CachedClasses.Find(ClassNameStr);
    
    if (!FoundClass || !*FoundClass || !IsValid(*FoundClass))
    {
        return 0;
    }

    UClass* ActorClass = *FoundClass;
    int32 ActorCount = 0;

    for (TActorIterator<AActor> ActorItr(World, ActorClass); ActorItr && ActorCount < MaxActors; ++ActorItr)
    {
        AActor* Actor = *ActorItr;
        if (Actor)
        {
            OutActors[ActorCount] = Actor;
            ActorCount++;
        }
    }

    return ActorCount;
}

REFLECTION_API bool GetActorLocation(void* Actor, float* OutX, float* OutY, float* OutZ)
{
    if (!Actor || !OutX || !OutY || !OutZ)
    {
        return false;
    }

    AActor* ActorObj = static_cast<AActor*>(Actor);
    if (!IsValid(ActorObj))
    {
        return false;
    }

    FVector Location = ActorObj->GetActorLocation();
    *OutX = Location.X;
    *OutY = Location.Y;
    *OutZ = Location.Z;

    return true;
}

REFLECTION_API bool SetActorLocation(void* Actor, float X, float Y, float Z)
{
    if (!Actor)
    {
        return false;
    }

    AActor* ActorObj = static_cast<AActor*>(Actor);
    if (!IsValid(ActorObj))
    {
        return false;
    }

    FVector NewLocation(X, Y, Z);
    return ActorObj->SetActorLocation(NewLocation);
}

REFLECTION_API bool GetActorRotation(void* Actor, float* OutPitch, float* OutYaw, float* OutRoll)
{
    if (!Actor || !OutPitch || !OutYaw || !OutRoll)
    {
        return false;
    }

    AActor* ActorObj = static_cast<AActor*>(Actor);
    if (!IsValid(ActorObj))
    {
        return false;
    }

    FRotator Rotation = ActorObj->GetActorRotation();
    *OutPitch = Rotation.Pitch;
    *OutYaw = Rotation.Yaw;
    *OutRoll = Rotation.Roll;

    return true;
}

REFLECTION_API bool SetActorRotation(void* Actor, float Pitch, float Yaw, float Roll)
{
    if (!Actor)
    {
        return false;
    }

    AActor* ActorObj = static_cast<AActor*>(Actor);
    if (!IsValid(ActorObj))
    {
        return false;
    }

    FRotator NewRotation(Pitch, Yaw, Roll);
    return ActorObj->SetActorRotation(NewRotation);
}

// =================================================================================
// COMPONENT API IMPLEMENTATION
// =================================================================================

REFLECTION_API void* AddComponent(void* Actor, const char* ComponentClassName)
{
    if (!Actor || !ComponentClassName)
    {
        return nullptr;
    }

    AActor* ActorObj = static_cast<AActor*>(Actor);
    if (!IsValid(ActorObj))
    {
        return nullptr;
    }

    FString ComponentClassNameStr = FString(ANSI_TO_TCHAR(ComponentClassName));
    UClass** FoundClass = CachedClasses.Find(ComponentClassNameStr);
    
    if (!FoundClass || !*FoundClass || !IsValid(*FoundClass))
    {
        return nullptr;
    }

    UClass* ComponentClass = *FoundClass;
    if (!ComponentClass->IsChildOf(UActorComponent::StaticClass()))
    {
        return nullptr;
    }

    UActorComponent* NewComponent = NewObject<UActorComponent>(ActorObj, ComponentClass);
    if (NewComponent)
    {
        ActorObj->AddInstanceComponent(NewComponent);
        NewComponent->RegisterComponent();
        ObjectRegistry.Add(NewComponent, NewComponent);
        return NewComponent;
    }

    return nullptr;
}

REFLECTION_API void* GetComponent(void* Actor, const char* ComponentClassName)
{
    if (!Actor || !ComponentClassName)
    {
        return nullptr;
    }

    AActor* ActorObj = static_cast<AActor*>(Actor);
    if (!IsValid(ActorObj))
    {
        return nullptr;
    }

    FString ComponentClassNameStr = FString(ANSI_TO_TCHAR(ComponentClassName));
    UClass** FoundClass = CachedClasses.Find(ComponentClassNameStr);
    
    if (!FoundClass || !*FoundClass || !IsValid(*FoundClass))
    {
        return nullptr;
    }

    UClass* ComponentClass = *FoundClass;
    UActorComponent* Component = ActorObj->GetComponentByClass(ComponentClass);
    
    return Component;
}

REFLECTION_API bool RemoveComponent(void* Actor, void* Component)
{
    if (!Actor || !Component)
    {
        return false;
    }

    AActor* ActorObj = static_cast<AActor*>(Actor);
    UActorComponent* ComponentObj = static_cast<UActorComponent*>(Component);
    
    if (!IsValid(ActorObj) || !IsValid(ComponentObj))
    {
        return false;
    }

    ComponentObj->UnregisterComponent();
    ComponentObj->DestroyComponent();
    ObjectRegistry.Remove(Component);
    
    return true;
}

// =================================================================================
// UTILITY FUNCTIONS IMPLEMENTATION
// =================================================================================

REFLECTION_API EReflectionPropertyType GetReflectionPropertyType(const char* UE5TypeName)
{
    if (!UE5TypeName)
    {
        return EReflectionPropertyType::Unknown;
    }

    FString TypeName = FString(ANSI_TO_TCHAR(UE5TypeName));
    
    if (TypeName == TEXT("BoolProperty")) return EReflectionPropertyType::Bool;
    if (TypeName == TEXT("Int8Property")) return EReflectionPropertyType::Int8;
    if (TypeName == TEXT("Int16Property")) return EReflectionPropertyType::Int16;
    if (TypeName == TEXT("IntProperty")) return EReflectionPropertyType::Int32;
    if (TypeName == TEXT("Int64Property")) return EReflectionPropertyType::Int64;
    if (TypeName == TEXT("FloatProperty")) return EReflectionPropertyType::Float;
    if (TypeName == TEXT("DoubleProperty")) return EReflectionPropertyType::Double;
    if (TypeName == TEXT("StrProperty")) return EReflectionPropertyType::String;
    if (TypeName == TEXT("NameProperty")) return EReflectionPropertyType::Name;
    if (TypeName == TEXT("ObjectProperty")) return EReflectionPropertyType::Object;
    if (TypeName == TEXT("ClassProperty")) return EReflectionPropertyType::Class;
    if (TypeName == TEXT("StructProperty")) return EReflectionPropertyType::Struct;
    if (TypeName == TEXT("ArrayProperty")) return EReflectionPropertyType::Array;
    if (TypeName == TEXT("EnumProperty")) return EReflectionPropertyType::Enum;
    
    return EReflectionPropertyType::Unknown;
}

REFLECTION_API int32 GetReflectionPropertySize(EReflectionPropertyType PropertyType)
{
    switch (PropertyType)
    {
    case EReflectionPropertyType::Bool: return sizeof(bool);
    case EReflectionPropertyType::Int8: return sizeof(int8);
    case EReflectionPropertyType::Int16: return sizeof(int16);
    case EReflectionPropertyType::Int32: return sizeof(int32);
    case EReflectionPropertyType::Int64: return sizeof(int64);
    case EReflectionPropertyType::UInt8: return sizeof(uint8);
    case EReflectionPropertyType::UInt16: return sizeof(uint16);
    case EReflectionPropertyType::UInt32: return sizeof(uint32);
    case EReflectionPropertyType::UInt64: return sizeof(uint64);
    case EReflectionPropertyType::Float: return sizeof(float);
    case EReflectionPropertyType::Double: return sizeof(double);
    case EReflectionPropertyType::String: return sizeof(FString);
    case EReflectionPropertyType::Name: return sizeof(FName);
    case EReflectionPropertyType::Vector: return sizeof(FVector);
    case EReflectionPropertyType::Rotator: return sizeof(FRotator);
    case EReflectionPropertyType::Transform: return sizeof(FTransform);
    case EReflectionPropertyType::Object: return sizeof(void*);
    case EReflectionPropertyType::Class: return sizeof(void*);
    default: return 0;
    }
}

REFLECTION_API bool IsTypeMarshallable(EReflectionPropertyType PropertyType)
{
    switch (PropertyType)
    {
    case EReflectionPropertyType::Bool:
    case EReflectionPropertyType::Int8:
    case EReflectionPropertyType::Int16:
    case EReflectionPropertyType::Int32:
    case EReflectionPropertyType::Int64:
    case EReflectionPropertyType::UInt8:
    case EReflectionPropertyType::UInt16:
    case EReflectionPropertyType::UInt32:
    case EReflectionPropertyType::UInt64:
    case EReflectionPropertyType::Float:
    case EReflectionPropertyType::Double:
    case EReflectionPropertyType::Vector:
    case EReflectionPropertyType::Rotator:
    case EReflectionPropertyType::Object:
        return true;
    default:
        return false;
    }
}

REFLECTION_API bool GetObjectClassName(void* Object, char* OutClassName, int32 MaxLength)
{
    if (!Object || !OutClassName || MaxLength <= 0)
    {
        return false;
    }

    UObject* UObj = static_cast<UObject*>(Object);
    if (!IsValid(UObj))
    {
        return false;
    }

    FString ClassName = UObj->GetClass()->GetName();
    FCStringAnsi::Strncpy(OutClassName, TCHAR_TO_ANSI(*ClassName), MaxLength - 1);
    OutClassName[MaxLength - 1] = '\0';
    
    return true;
}

REFLECTION_API bool IsObjectValid(void* Object)
{
    if (!Object)
    {
        return false;
    }

    UObject* UObj = static_cast<UObject*>(Object);
    return IsValid(UObj);
}

REFLECTION_API void PrintObjectProperties(void* Object)
{
    if (!Object)
    {
        UE_LOG(LogTemp, Warning, TEXT("PrintObjectProperties: Object is null"));
        return;
    }

    UObject* UObj = static_cast<UObject*>(Object);
    if (!IsValid(UObj))
    {
        UE_LOG(LogTemp, Warning, TEXT("PrintObjectProperties: Object is invalid"));
        return;
    }

    UE_LOG(LogTemp, Log, TEXT("=== Properties of %s ==="), *UObj->GetClass()->GetName());
    
    for (FProperty* Property = UObj->GetClass()->PropertyLink; Property; Property = Property->PropertyLinkNext)
    {
        FString PropertyValue;
        Property->ExportTextItem(PropertyValue, Property->ContainerPtrToValuePtr<void>(UObj), nullptr, UObj, PPF_None);
        
        UE_LOG(LogTemp, Log, TEXT("%s (%s): %s"), 
            *Property->GetName(), 
            *Property->GetClass()->GetName(), 
            *PropertyValue);
    }
}

REFLECTION_API void GetReflectionStats(int32* OutNumClasses, int32* OutNumProperties, int32* OutNumFunctions)
{
    if (OutNumClasses)
    {
        *OutNumClasses = CachedClasses.Num();
    }
    
    if (OutNumProperties || OutNumFunctions)
    {
        int32 TotalProperties = 0;
        int32 TotalFunctions = 0;
        
        for (auto& ClassPair : CachedClasses)
        {
            UClass* Class = ClassPair.Value;
            if (Class && IsValid(Class))
            {
                for (FProperty* Property = Class->PropertyLink; Property; Property = Property->PropertyLinkNext)
                {
                    TotalProperties++;
                }
                
                for (TFieldIterator<UFunction> FuncIt(Class); FuncIt; ++FuncIt)
                {
                    TotalFunctions++;
                }
            }
        }
        
        if (OutNumProperties)
        {
            *OutNumProperties = TotalProperties;
        }
        
        if (OutNumFunctions)
        {
            *OutNumFunctions = TotalFunctions;
        }
    }
}
