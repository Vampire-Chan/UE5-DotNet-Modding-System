#pragma once

#include "CoreMinimal.h"
#include "DataAssetTypeEnum.generated.h"

/**
 * Enumeration of all possible DataAssetType values from AssetDefinitions.xml
 * Used for categorizing and loading different types of game data files
 */
UENUM(BlueprintType)
enum class EDataAssetType : uint8
{
    // Core Systems
    PakfileManifest     UMETA(DisplayName = "Pakfile Manifest"),
    Shaders             UMETA(DisplayName = "Shaders"),

    // Audio System
    AudioMaster         UMETA(DisplayName = "Audio Master"),

    // Video System
    VideoBank           UMETA(DisplayName = "Video Bank"),

    // Ped Data
    PedData             UMETA(DisplayName = "Ped Data"),
    PedVariations       UMETA(DisplayName = "Ped Variations"),
    PedMesh             UMETA(DisplayName = "Ped Mesh"),
    PedPropMesh         UMETA(DisplayName = "Ped Prop Mesh"),
    PedAttributes       UMETA(DisplayName = "Ped Attributes"),
    PedCombat           UMETA(DisplayName = "Ped Combat"),
    PedHealth           UMETA(DisplayName = "Ped Health"),
    PedIntelligence     UMETA(DisplayName = "Ped Intelligence"),
    PedPerception       UMETA(DisplayName = "Ped Perception"),
    PedRelationships    UMETA(DisplayName = "Ped Relationships"),

    // Vehicle Data
    VehicleData         UMETA(DisplayName = "Vehicle Data"),
    VehicleHandling     UMETA(DisplayName = "Vehicle Handling"),
    VehicleMesh         UMETA(DisplayName = "Vehicle Mesh"),
    VehicleModConfig    UMETA(DisplayName = "Vehicle Mod Config"),

    // Weapon Data
    WeaponData          UMETA(DisplayName = "Weapon Data"),
    WeaponAttachments   UMETA(DisplayName = "Weapon Attachments"),

    // World & Environment
    WorldPlacement      UMETA(DisplayName = "World Placement"),
    InteriorPlacement   UMETA(DisplayName = "Interior Placement"),
    VehicleParking      UMETA(DisplayName = "Vehicle Parking"),
    TimeCycle           UMETA(DisplayName = "Time Cycle"),
    Weather             UMETA(DisplayName = "Weather"),
    
    // Animation Data
    AnimationGroups     UMETA(DisplayName = "Animation Groups"),

    // Other Data
    ObjectData          UMETA(DisplayName = "Object Data"),
    ObjectMesh          UMETA(DisplayName = "Object Mesh"),
    TextureContainers   UMETA(DisplayName = "Texture Containers"),

    // Invalid/Unknown
    Unknown             UMETA(DisplayName = "Unknown")
};

/**
 * Static utility class for DataAssetType operations
 */
class GAME_API FDataAssetTypeUtils
{
public:
    /**
     * Convert string from XML to enum value
     * @param AssetTypeString The string from DataAssetType attribute in XML
     * @return Corresponding enum value, or Unknown if not found
     */
    static EDataAssetType StringToDataAssetType(const FString& AssetTypeString);
    
    /**
     * Convert enum value to string for XML writing
     * @param AssetType The enum value to convert
     * @return String representation for XML
     */
    static FString DataAssetTypeToString(EDataAssetType AssetType);
    
    /**
     * Get display name for UI purposes
     * @param AssetType The enum value
     * @return User-friendly display name
     */
    static FString GetDisplayName(EDataAssetType AssetType);
    
    /**
     * Check if asset type is audio-related
     * @param AssetType The enum value to check
     * @return True if this is an audio asset type
     */
    static bool IsAudioAssetType(EDataAssetType AssetType);
    
    /**
     * Check if asset type is world-related
     * @param AssetType The enum value to check
     * @return True if this is a world/placement asset type
     */
    static bool IsWorldAssetType(EDataAssetType AssetType);
    
    /**
     * Check if asset type is entity-related (peds, vehicles, objects)
     * @param AssetType The enum value to check
     * @return True if this is an entity definition asset type
     */
    static bool IsEntityAssetType(EDataAssetType AssetType);
    
    /**
     * Get loading priority for asset type (lower number = higher priority)
     * @param AssetType The enum value
     * @return Loading priority (0-10, where 0 loads first)
     */
    static int32 GetLoadingPriority(EDataAssetType AssetType);
    
    /**
     * Get all available asset types
     * @return Array of all valid asset types (excluding Unknown)
     */
    static TArray<EDataAssetType> GetAllAssetTypes();
    
private:
    // Static lookup tables for performance
    static TMap<FString, EDataAssetType> StringToEnumMap;
    static TMap<EDataAssetType, FString> EnumToStringMap;
    static TMap<EDataAssetType, int32> LoadingPriorityMap;
    
    // Initialize static maps
    static void InitializeMaps();
    static bool bMapsInitialized;
};