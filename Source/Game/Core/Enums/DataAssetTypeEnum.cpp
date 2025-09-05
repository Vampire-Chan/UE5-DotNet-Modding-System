#include "DataAssetTypeEnum.h"

// Static member initialization
TMap<FString, EDataAssetType> FDataAssetTypeUtils::StringToEnumMap;
TMap<EDataAssetType, FString> FDataAssetTypeUtils::EnumToStringMap;
TMap<EDataAssetType, int32> FDataAssetTypeUtils::LoadingPriorityMap;
bool FDataAssetTypeUtils::bMapsInitialized = false;

void FDataAssetTypeUtils::InitializeMaps()
{
    if (bMapsInitialized)
        return;

    // String to Enum mapping (matches XML DataAssetType values exactly)
    StringToEnumMap.Add(TEXT("PakfileManifest"), EDataAssetType::PakfileManifest);
    StringToEnumMap.Add(TEXT("Shaders"), EDataAssetType::Shaders);
    StringToEnumMap.Add(TEXT("AudioMaster"), EDataAssetType::AudioMaster);
    StringToEnumMap.Add(TEXT("VideoBank"), EDataAssetType::VideoBank);
    StringToEnumMap.Add(TEXT("PedData"), EDataAssetType::PedData);
    StringToEnumMap.Add(TEXT("PedVariations"), EDataAssetType::PedVariations);
    StringToEnumMap.Add(TEXT("PedMesh"), EDataAssetType::PedMesh);
    StringToEnumMap.Add(TEXT("PedPropMesh"), EDataAssetType::PedPropMesh);
    StringToEnumMap.Add(TEXT("PedAttributes"), EDataAssetType::PedAttributes);
    StringToEnumMap.Add(TEXT("PedCombat"), EDataAssetType::PedCombat);
    StringToEnumMap.Add(TEXT("PedHealth"), EDataAssetType::PedHealth);
    StringToEnumMap.Add(TEXT("PedIntelligence"), EDataAssetType::PedIntelligence);
    StringToEnumMap.Add(TEXT("PedPerception"), EDataAssetType::PedPerception);
    StringToEnumMap.Add(TEXT("PedRelationships"), EDataAssetType::PedRelationships);
    StringToEnumMap.Add(TEXT("VehicleData"), EDataAssetType::VehicleData);
    StringToEnumMap.Add(TEXT("VehicleHandling"), EDataAssetType::VehicleHandling);
    StringToEnumMap.Add(TEXT("VehicleMesh"), EDataAssetType::VehicleMesh);
    StringToEnumMap.Add(TEXT("VehicleModConfig"), EDataAssetType::VehicleModConfig);
    StringToEnumMap.Add(TEXT("WeaponData"), EDataAssetType::WeaponData);
    StringToEnumMap.Add(TEXT("WeaponAttachments"), EDataAssetType::WeaponAttachments);
    StringToEnumMap.Add(TEXT("WorldPlacement"), EDataAssetType::WorldPlacement);
    StringToEnumMap.Add(TEXT("InteriorPlacement"), EDataAssetType::InteriorPlacement);
    StringToEnumMap.Add(TEXT("VehicleParking"), EDataAssetType::VehicleParking);
    StringToEnumMap.Add(TEXT("TimeCycle"), EDataAssetType::TimeCycle);
    StringToEnumMap.Add(TEXT("Weather"), EDataAssetType::Weather);
    StringToEnumMap.Add(TEXT("AnimationGroups"), EDataAssetType::AnimationGroups);
    StringToEnumMap.Add(TEXT("ObjectData"), EDataAssetType::ObjectData);
    StringToEnumMap.Add(TEXT("ObjectMesh"), EDataAssetType::ObjectMesh);
    StringToEnumMap.Add(TEXT("TextureContainers"), EDataAssetType::TextureContainers);
    
    // Enum to String mapping (reverse lookup)
    for (auto const& [key, val] : StringToEnumMap)
    {
        EnumToStringMap.Add(val, key);
    }
    EnumToStringMap.Add(EDataAssetType::Unknown, TEXT("Unknown"));
    
    bMapsInitialized = true;
}

EDataAssetType FDataAssetTypeUtils::StringToDataAssetType(const FString& AssetTypeString)
{
    InitializeMaps();
    
    if (const EDataAssetType* FoundType = StringToEnumMap.Find(AssetTypeString))
    {
        return *FoundType;
    }
    
    return EDataAssetType::Unknown;
}

FString FDataAssetTypeUtils::DataAssetTypeToString(EDataAssetType AssetType)
{
    InitializeMaps();
    
    if (const FString* FoundString = EnumToStringMap.Find(AssetType))
    {
        return *FoundString;
    }
    
    return TEXT("Unknown");
}

FString FDataAssetTypeUtils::GetDisplayName(EDataAssetType AssetType)
{
    // This can be expanded with a switch statement for more descriptive names if needed
    return DataAssetTypeToString(AssetType);
}

// Implementations for IsAudioAssetType, IsWorldAssetType, IsEntityAssetType, GetLoadingPriority, GetAllAssetTypes can be added here
// For brevity, they are omitted but would be similar to the original file, updated with the new enum values.