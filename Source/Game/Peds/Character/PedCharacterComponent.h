#pragma once

namespace tinyxml2
{
    class XMLDocument;
}

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "Engine/SkeletalMesh.h"
#include "Materials/MaterialInterface.h"
#include "Engine/AssetManager.h"
#include "Engine/StreamableManager.h"
#include "../Core/Enums/GameWorldEnums.h"
#include "../Data/Enums/ComponentVariant.h"
#include "../Animation/PedAnimationController.h"
#include "PedCharacterComponent.generated.h"

USTRUCT(BlueprintType)
struct FPedComponentMesh
{
    GENERATED_BODY()

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Component")
    EPedBodyPart BodyPartType;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Component")
    FString ComponentName;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Component")
    FString AltName;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Component")
    int32 MeshIndex;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Component")
    int32 TextureVariantIndex;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Component")
    ETextureRaceType TextureRace;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Component")
    FString TextureRaceString;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Component")
    TSoftObjectPtr<USkeletalMesh> MeshAssetPath;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Component")
    TSoftObjectPtr<UTexture> DiffuseTexturePath;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Component")
    TSoftObjectPtr<UTexture> NormalTexturePath;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Component")
    TSoftObjectPtr<UTexture> SpecularTexturePath;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Component")
    TArray<TSoftObjectPtr<UMaterialInterface>> Materials;

    FPedComponentMesh();

    void GenerateAssetPaths(const FString& CharacterPath);
};

USTRUCT(BlueprintType)
struct FPedPropMesh
{
    GENERATED_BODY()

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Prop")
    EPedPropType PropType;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Prop")
    FString PropName;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Prop")
    FString AltName;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Prop")
    int32 MeshIndex;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Prop")
    int32 TextureVariantIndex;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Prop")
    ETextureRaceType TextureRace;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Prop")
    TSoftObjectPtr<UStaticMesh> PropMeshAssetPath;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Prop")
    TSoftObjectPtr<UTexture> DiffuseTexturePath;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Prop")
    TSoftObjectPtr<UTexture> NormalTexturePath;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Prop")
    TSoftObjectPtr<UTexture> SpecularTexturePath;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Prop")
    TArray<TSoftObjectPtr<UMaterialInterface>> Materials;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Prop")
    FName AttachSocketName;

    FPedPropMesh();

    void GenerateAssetPaths(const FString& CharacterPath);
};

// Helper structs for asset discovery
USTRUCT(BlueprintType)
struct FComponentNumbers
{
    GENERATED_BODY()

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Asset Discovery")
    TArray<int32> Numbers;

    FComponentNumbers()
    {
        Numbers.Empty();
    }
};

USTRUCT(BlueprintType)
struct FComponentVariants
{
    GENERATED_BODY()

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Asset Discovery")
    TArray<int32> Variants;

    FComponentVariants()
    {
        Variants.Empty();
    }
};

USTRUCT(BlueprintType)
struct FPedAssetDiscovery
{
    GENERATED_BODY()

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Asset Discovery")
    FString CharacterName;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Asset Discovery")
    FString BasePath;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Asset Discovery")
    FString SkeletonPath;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Asset Discovery")
    TMap<FString, FComponentNumbers> AvailableComponents; // ComponentName -> Available Numbers

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Asset Discovery")
    TMap<FString, FComponentVariants> AvailableVariants; // ComponentName -> Available Variants

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Asset Discovery")
    TArray<ETextureRaceType> AvailableRaces;

    FPedAssetDiscovery()
    {
        CharacterName = TEXT("");
        BasePath = TEXT("");
        SkeletonPath = TEXT("");
        AvailableComponents.Empty();
        AvailableVariants.Empty();
        AvailableRaces.Empty();
    }

    // Generate expected skeleton path: CharacterName/SKEL_CharacterName.uasset
    FString GenerateSkeletonPath() const
    {
        return FString::Printf(TEXT("%s/SKEL_%s"), *BasePath, *CharacterName);
    }

    // Check if component number exists for body part
    bool HasComponentNumber(EPedBodyPart BodyPart, int32 ComponentNumber) const
    {
        FString ComponentName = GetBodyPartNameStatic(BodyPart);
        if (const FComponentNumbers* ComponentData = AvailableComponents.Find(ComponentName))
        {
            return ComponentData->Numbers.Contains(ComponentNumber);
        }
        return false;
    }

    // Get all available component numbers for a body part
    TArray<int32> GetAvailableNumbers(EPedBodyPart BodyPart) const
    {
        FString ComponentName = GetBodyPartNameStatic(BodyPart);
        if (const FComponentNumbers* ComponentData = AvailableComponents.Find(ComponentName))
        {
            return ComponentData->Numbers;
        }
        return TArray<int32>();
    }

private:
    static FString GetBodyPartNameStatic(EPedBodyPart BodyPart)
    {
        switch (BodyPart)
        {
        case EPedBodyPart::Head: return TEXT("Head");
        case EPedBodyPart::Hair: return TEXT("Hair");
        case EPedBodyPart::Eyes: return TEXT("Eyes");
        case EPedBodyPart::Teeth: return TEXT("Teeth");
        case EPedBodyPart::Upper: return TEXT("Upper");
        case EPedBodyPart::Lower: return TEXT("Lower");
        case EPedBodyPart::Hands: return TEXT("Hands");
        case EPedBodyPart::Arms: return TEXT("Arms");
        case EPedBodyPart::Feet: return TEXT("Feet");
        case EPedBodyPart::Nails: return TEXT("Nails");
        default: return TEXT("Unknown");
        }
    }
};

USTRUCT(BlueprintType)
struct FPedMeshConfiguration
{
    GENERATED_BODY()

    // Main body components - following XML structure
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Body Parts")
    FPedComponentMesh Head;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Body Parts")
    FPedComponentMesh Hair;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Body Parts")
    FPedComponentMesh Eyes;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Body Parts")
    FPedComponentMesh Teeth;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Body Parts")
    FPedComponentMesh Upper; // Upper body

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Body Parts")
    FPedComponentMesh Lower; // Lower body

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Body Parts")
    FPedComponentMesh Hands;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Body Parts")
    FPedComponentMesh Arms;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Body Parts")
    FPedComponentMesh Feet;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Body Parts")
    FPedComponentMesh Nails;

    // Props
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Props")
    TArray<FPedPropMesh> Props;

    FPedMeshConfiguration()
    {
        Head.ComponentName = TEXT("Head");
        Hair.ComponentName = TEXT("Hair");
        Eyes.ComponentName = TEXT("Eyes");
        Teeth.ComponentName = TEXT("Teeth");
        Upper.ComponentName = TEXT("Upper");
        Upper.AltName = TEXT("uppr, upperbody");
        Lower.ComponentName = TEXT("Lower");
        Hands.ComponentName = TEXT("Hands");
        Arms.ComponentName = TEXT("Arms");
        Feet.ComponentName = TEXT("Feet");
        Nails.ComponentName = TEXT("Nails");
        Props.Empty();
    }
};

USTRUCT(BlueprintType)
struct FPedVariationSet
{
    GENERATED_BODY()

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Variation")
    FString VariationName;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Variation")
    FPedMeshConfiguration MeshConfig;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Variation")
    FString PedName;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Variation")
    FString PropType;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Variation")
    FString BaseSkeleton;

    FPedVariationSet()
    {
        VariationName = TEXT("Default");
        PedName = TEXT("");
        PropType = TEXT("");
        BaseSkeleton = TEXT("");
    }
};

USTRUCT(BlueprintType)
struct FPedVisualProfile
{
    GENERATED_BODY()

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Visual")
    FString ProfileName;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Visual")
    FPedVariationSet CurrentVariation;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Visual")
    TArray<FPedVariationSet> AvailableVariations;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Visual")
    FVector Scale;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Visual")
    float HeightVariation;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Visual")
    float WeightVariation;

    FPedVisualProfile()
    {
        ProfileName = TEXT("Default");
        Scale = FVector(1.0f, 1.0f, 1.0f);
        HeightVariation = 0.0f;
        WeightVariation = 0.0f;
        AvailableVariations.Empty();
    }
};

/**
 * Component responsible for managing the visual appearance and mesh setup of Ped characters.
 * Handles mesh loading, material application, and visual variations.
 */
UCLASS(ClassGroup=(Custom), meta=(BlueprintSpawnableComponent))
class GAME_API UPedCharacterComponent : public UActorComponent
{
    GENERATED_BODY()

public:
    UPedCharacterComponent();

protected:
    virtual void BeginPlay() override;

public:
    virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

    // Asset loading callbacks (public for async loading)
    void OnComponentMeshLoaded(const FString& ComponentName, USkeletalMesh* LoadedMesh);
    void OnPropMeshLoaded(const FString& PropName, UStaticMesh* LoadedMesh);
    void OnComponentMaterialLoaded(const FString& ComponentName, int32 MaterialIndex, UMaterialInterface* LoadedMaterial);
    void LoadComponentMaterialAsync(const FString& ComponentName, int32 MaterialIndex, const TSoftObjectPtr<UMaterialInterface>& MaterialPtr);

    // Mesh and Visual Management
    UFUNCTION(BlueprintCallable, Category = "Ped Character")
    void LoadVisualProfile(const FPedVisualProfile& Profile);

    UFUNCTION(BlueprintCallable, Category = "Ped Character")
    void LoadVariationSet(const FString& VariationName);

    UFUNCTION(BlueprintCallable, Category = "Ped Character")
    void SetMeshConfiguration(const FPedMeshConfiguration& MeshConfig);

    UFUNCTION(BlueprintCallable, Category = "Ped Character")
    void ApplyComponentMesh(const FPedComponentMesh& ComponentMesh, USkeletalMeshComponent* TargetComponent);

    UFUNCTION(BlueprintCallable, Category = "Ped Character")
    void ApplyPropMesh(const FPedPropMesh& PropMesh);

    // XML Data Loading
    UFUNCTION(BlueprintCallable, Category = "Ped Character")
    void LoadFromXMLData(const FString& PedName);

    UFUNCTION(BlueprintCallable, Category = "Ped Character")
    void LoadPedMeshXML();

    UFUNCTION(BlueprintCallable, Category = "Ped Character")
    void LoadBaseSkeletonFromXML();

    UFUNCTION(BlueprintCallable, Category = "Ped Character")
    void LoadPedVariationsXML();

    UFUNCTION(BlueprintCallable, Category = "Ped Character")
    void LoadPedPropsXML();

    // Component-specific operations
    UFUNCTION(BlueprintCallable, Category = "Ped Character")
    void SetComponentByName(const FString& ComponentName, const FPedComponentMesh& ComponentMesh);

    UFUNCTION(BlueprintCallable, Category = "Ped Character")
    FPedComponentMesh GetComponentByName(const FString& ComponentName);

    UFUNCTION(BlueprintCallable, Category = "Ped Character")
    void SetComponentByType(EPedBodyPart BodyPart, const FPedComponentMesh& ComponentMesh);

    UFUNCTION(BlueprintCallable, Category = "Ped Character")
    FPedComponentMesh GetComponentByType(EPedBodyPart BodyPart);

    UFUNCTION(BlueprintCallable, Category = "Ped Character")
    FString GetBodyPartName(EPedBodyPart BodyPart);

    UFUNCTION(BlueprintCallable, Category = "Ped Character")
    EPedBodyPart GetBodyPartFromName(const FString& BodyPartName);

    // === ENHANCED MANUAL COMPONENT CONTROL ===
    
    UFUNCTION(BlueprintCallable, Category = "Ped Character")
    void SetComponentManual(EPedBodyPart BodyPart, int32 MeshIndex, int32 TextureIndex = 0, const FString& TextureRace = TEXT("Uni"));

    UFUNCTION(BlueprintCallable, Category = "Ped Character")
    void SetComponentMaterialBySlot(EPedBodyPart BodyPart, int32 MaterialSlot, const FString& MaterialName);

    UFUNCTION(BlueprintCallable, Category = "Ped Character")
    void SetComponentMaterialByIndex(EPedBodyPart BodyPart, int32 MaterialSlot, int32 MaterialIndex);

    UFUNCTION(BlueprintCallable, Category = "Ped Character")
    void ApplyMaterialFallbackToPed(const FString& FallbackMaterialName = TEXT("M_Ped"));

    UFUNCTION(BlueprintCallable, Category = "Ped Character")
    void SetComponentByVariation(EPedBodyPart BodyPart, int32 ComponentNumber, int32 TextureVariantIndex = 0, ETextureRaceType Race = ETextureRaceType::Universal);

    UFUNCTION(BlueprintCallable, Category = "Ped Character")
    void SetComponentByAssetPath(EPedBodyPart BodyPart, const FString& AssetPath);

    UFUNCTION(BlueprintCallable, Category = "Ped Character")
    void GenerateComponentAssetPaths(FPedComponentMesh& ComponentMesh, const FString& CharacterPath);

    UFUNCTION(BlueprintCallable, Category = "Ped Character")
    void SetPropByVariation(const FString& PropName, int32 PropNumber, int32 TextureVariantIndex = 0, ETextureRaceType Race = ETextureRaceType::Universal, FName AttachSocket = NAME_None);

    UFUNCTION(BlueprintCallable, Category = "Ped Character")
    ETextureRaceType GetRaceFromString(const FString& RaceString);

    // Asset Discovery System
    UFUNCTION(BlueprintCallable, Category = "Asset Discovery")
    FPedAssetDiscovery DiscoverCharacterAssets(const FString& CharacterName, const FString& BasePath);

    UFUNCTION(BlueprintCallable, Category = "Asset Discovery")
    TArray<int32> GetAvailableComponentNumbers(EPedBodyPart BodyPart, const FString& CharacterPath);

    UFUNCTION(BlueprintCallable, Category = "Asset Discovery")
    TArray<EComponentVariant> GetAvailableComponentVariants(EPedBodyPart BodyPart, int32 ComponentNumber, const FString& CharacterPath);

    UFUNCTION(BlueprintCallable, Category = "Asset Discovery")
    TArray<ETextureRaceType> GetAvailableRaces(EPedBodyPart BodyPart, int32 ComponentNumber, EComponentVariant Variant, const FString& CharacterPath);

    UFUNCTION(BlueprintCallable, Category = "Asset Discovery")
    bool ValidateComponentExists(EPedBodyPart BodyPart, int32 ComponentNumber, EComponentVariant Variant, ETextureRaceType Race, const FString& CharacterPath);

    // Automatic Asset Loading with Fallback Material System
    UFUNCTION(BlueprintCallable, Category = "Asset Loading")
    bool LoadComponentWithFallback(const FString& ComponentName, int32 ComponentNumber, EComponentVariant Variant, ETextureRaceType Race, const FString& CharacterPath);

    // Helper functions for XML material mapping
    UFUNCTION(BlueprintCallable, Category = "Asset Loading")
    bool HasComponentInXML(const FString& ComponentName, int32 ComponentNumber);

    UFUNCTION(BlueprintCallable, Category = "Asset Loading")
    UMaterialInterface* GetMaterialFromXMLMapping(const FString& ComponentName, int32 ComponentNumber, const FString& TextureAssetPath);

    // === ENHANCED MATERIAL FALLBACK SYSTEM ===
    
    UFUNCTION(BlueprintCallable, Category = "Material System")
    TSoftObjectPtr<UMaterialInterface> LoadMaterialWithFallback(const FString& MaterialName);

    UFUNCTION(BlueprintCallable, Category = "Material System")
    void ApplyMaterialFallbackToComponent(FPedComponentMesh& ComponentMesh);

    UFUNCTION(BlueprintCallable, Category = "Material System")
    FString GetMaterialNameFromXMLByIndex(const FString& ComponentName, int32 MaterialIndex);

    // Enum conversion utilities
    UFUNCTION(BlueprintCallable, Category = "Utilities")
    FString GetRaceStringFromEnum(ETextureRaceType Race);

    UFUNCTION(BlueprintCallable, Category = "Utilities")
    FString GetVariantStringFromEnum(EComponentVariant Variant);

    UFUNCTION(BlueprintCallable, Category = "Ped Character")
    void AddProp(const FPedPropMesh& PropMesh);

    UFUNCTION(BlueprintCallable, Category = "Ped Character")
    void RemoveProp(const FString& PropName);

    UFUNCTION(BlueprintCallable, Category = "Ped Character")
    void ClearAllProps();

    // Animation Integration
    UFUNCTION(BlueprintCallable, Category = "Ped Character")
    void SetupAnimationController(USkeletalMeshComponent* SkeletalMeshComponent);

    UFUNCTION(BlueprintCallable, Category = "Ped Character")
    UPedAnimationController* GetAnimationController() const;

    // === ENHANCED ANIMATION CONTROL ===
    
    UFUNCTION(BlueprintCallable, Category = "Animation Control")
    void SetAnimationGroupFromXML(const FString& AnimationGroupName);

    UFUNCTION(BlueprintCallable, Category = "Animation Control")
    void PlayMovementAnimation(const FString& AnimationName, bool bLoop = true, float BlendInTime = 0.15f, float BlendOutTime = 0.15f);

    UFUNCTION(BlueprintCallable, Category = "Animation Control")
    void SetMovementBlendSpace(const FString& BlendSpaceName);

    UFUNCTION(BlueprintCallable, Category = "Animation Control")
    void SetAnimationSpeed(float NewSpeed = 1.0f);

    UFUNCTION(BlueprintCallable, Category = "Animation Control")
    void SwitchToMovementSet(const FString& MovementSetName);

    UFUNCTION(BlueprintCallable, Category = "Animation Control")
    TArray<FString> GetAvailableAnimationGroups();

    UFUNCTION(BlueprintCallable, Category = "Animation Control")
    TArray<FString> GetAvailableAnimationsInGroup(const FString& GroupName);

    // === HELPER FUNCTIONS FOR YOUR SPECIFIC ANIMATIONS ===
    
    UFUNCTION(BlueprintCallable, Category = "Animation Control")
    void PlayIdleAnimation();

    UFUNCTION(BlueprintCallable, Category = "Animation Control")
    void PlayWalkAnimation();

    UFUNCTION(BlueprintCallable, Category = "Animation Control")
    void PlayRunAnimation();

    UFUNCTION(BlueprintCallable, Category = "Animation Control")
    void PlaySprintAnimation();

    UFUNCTION(BlueprintCallable, Category = "Animation Control")
    bool HasPlayerMovementAnimations();

    // === MASTER SKELETON SYSTEM ===
    
    UFUNCTION(BlueprintCallable, Category = "Master Skeleton")
    void SetupMasterSkeletonSystem();

    UFUNCTION(BlueprintCallable, Category = "Master Skeleton")
    void UpdateMasterSkeleton(USkeletalMeshComponent* NewMasterComponent, const FString& NewMasterName);

    UFUNCTION(BlueprintCallable, Category = "Master Skeleton")
    USkeletalMesh* GetMasterSkeleton();

    UFUNCTION(BlueprintCallable, Category = "Master Skeleton")
    void SynchronizeAllComponentsToMasterSkeleton();

    UFUNCTION(BlueprintCallable, Category = "Master Skeleton")
    bool ValidateSkeletonCompatibility(USkeletalMesh* TestSkeleton, USkeletalMesh* ReferenceSkeleton);

    UFUNCTION(BlueprintCallable, Category = "Master Skeleton")
    void SwapComponentWithSkeletonUpdate(const FString& ComponentType, int32 FromIndex, int32 ToIndex);

    UFUNCTION(BlueprintCallable, Category = "Master Skeleton")
    USkeletalMeshComponent* GetMasterSkeletonComponent() const;

    UFUNCTION(BlueprintCallable, Category = "Master Skeleton")
    FString GetMasterSkeletonComponentName() const;

    UFUNCTION(BlueprintCallable, Category = "Master Skeleton")
    bool IsComponentMasterSkeleton(const FString& ComponentName) const;

    UFUNCTION(BlueprintCallable, Category = "Master Skeleton")
    TArray<FString> GetFollowerComponentNames() const;

    // ========== PED COMPONENTS ACCESS ==========
    
    UFUNCTION(BlueprintCallable, Category = "Master Skeleton")
    void AddPedComponent(const FString& ComponentName, USkeletalMeshComponent* Component);
    
    UFUNCTION(BlueprintCallable, Category = "Master Skeleton") 
    int32 GetPedComponentCount() const;
    
    UFUNCTION(BlueprintCallable, Category = "Master Skeleton")
    USkeletalMeshComponent* GetPedComponent(const FString& ComponentName) const;

    // Visual Variations
    UFUNCTION(BlueprintCallable, Category = "Ped Character")
    void ApplyRandomVariations();

    UFUNCTION(BlueprintCallable, Category = "Ped Character")
    void SetHeightVariation(float Variation);

    UFUNCTION(BlueprintCallable, Category = "Ped Character")
    void SetWeightVariation(float Variation);

    UFUNCTION(BlueprintCallable, Category = "Ped Character")
    void SetOverallScale(const FVector& NewScale);

    // Asset Loading with proper component support
    UFUNCTION(BlueprintCallable, Category = "Ped Character")
    void LoadComponentMeshAsync(const FPedComponentMesh& ComponentMesh);

    UFUNCTION(BlueprintCallable, Category = "Ped Character")
    void LoadPropMeshAsync(const FPedPropMesh& PropMesh);

    // Getters
    UFUNCTION(BlueprintPure, Category = "Ped Character")
    const FPedVisualProfile& GetCurrentProfile() const { return CurrentProfile; }

    UFUNCTION(BlueprintPure, Category = "Ped Character")
    const FPedVariationSet& GetCurrentVariation() const { return CurrentProfile.CurrentVariation; }

    UFUNCTION(BlueprintPure, Category = "Ped Character")
    bool IsProfileLoaded() const { return bIsProfileLoaded; }

    UFUNCTION(BlueprintPure, Category = "Ped Character")
    bool IsLoadingAssets() const { return bIsLoadingAssets; }

    // Get the character asset folder name (e.g., "PlayerNiko")
    UFUNCTION(BlueprintPure, Category = "Ped Character")
    const FString& GetCharacterAssetFolder() const { return CharacterAssetFolder; }

    UFUNCTION(BlueprintPure, Category = "Ped Character")
    float GetLoadingProgress() const { return LoadingProgress; }

    UFUNCTION(BlueprintPure, Category = "Ped Character")
    TArray<FString> GetAvailableVariations() const;

    // Events - updated for new structure
    DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnProfileLoaded, const FPedVisualProfile&, LoadedProfile);
    DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnVariationLoaded, const FPedVariationSet&, LoadedVariation);
    DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FOnComponentLoaded, const FString&, ComponentName, const FPedComponentMesh&, ComponentMesh);
    DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnPropLoaded, const FPedPropMesh&, PropMesh);
    DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnAllAssetsLoaded);

    // Configuration
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Configuration")
    FPedVisualProfile DefaultProfile;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Configuration")
    bool bAutoLoadOnBeginPlay;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Configuration")
    bool bApplyRandomVariationsOnLoad;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Configuration")
    float MaxHeightVariation;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Configuration")
    float MaxWeightVariation;

    // XML file paths
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Configuration")
    FString PedMeshXMLPath;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Configuration")
    FString PedVariationsXMLPath;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Configuration")
    FString PedPropsXMLPath;

    UPROPERTY(BlueprintAssignable, Category = "Events")
    FOnProfileLoaded OnProfileLoaded;

    UPROPERTY(BlueprintAssignable, Category = "Events")
    FOnVariationLoaded OnVariationLoaded;

    UPROPERTY(BlueprintAssignable, Category = "Events")
    FOnComponentLoaded OnComponentLoaded;

    UPROPERTY(BlueprintAssignable, Category = "Events")
    FOnPropLoaded OnPropLoaded;

    UPROPERTY(BlueprintAssignable, Category = "Events")
    FOnAllAssetsLoaded OnAllAssetsLoaded;

private:
    // Current state
    UPROPERTY()
    FPedVisualProfile CurrentProfile;

    UPROPERTY()
    UPedAnimationController* AnimationController;

    UPROPERTY()
    USkeletalMeshComponent* CachedSkeletalMeshComponent;

    // Asset folder name from Peds.xml PropType (e.g., "PlayerNiko")
    UPROPERTY()
    FString CharacterAssetFolder;

    // === ENHANCED MATERIAL FALLBACK SYSTEM ===
    
    UPROPERTY()
    TSoftObjectPtr<UMaterialInterface> DefaultMaterial_M_Ped;

    UPROPERTY()
    TSoftObjectPtr<UMaterialInterface> DefaultMaterial_M_PedAlpha;

    UPROPERTY()
    TSoftObjectPtr<UMaterialInterface> DefaultMaterial_M_PedHairAlpha;

    UPROPERTY()
    TMap<FString, TSoftObjectPtr<UMaterialInterface>> CachedMaterials;

    // === XML DOCUMENT CACHING ===
    
    UPROPERTY()
    TMap<FString, FString> CachedXMLData; // Cache parsed XML data

    // Cache available animations per group (not serialized due to TArray value limitation)
    TMap<FString, TArray<FString>> AnimationGroupCache;

    UPROPERTY()
    FString CurrentAnimationGroup;

    // Track material slot names for each body part (not serialized due to TArray value limitation)  
    TMap<EPedBodyPart, TArray<FString>> MaterialSlotNames;

    // === MASTER SKELETON SYSTEM ===
    
    UPROPERTY()
    USkeletalMesh* MasterSkeleton;

    UPROPERTY()
    EPedBodyPart MasterSkeletonBodyPart; // Which body part provides the master skeleton

    UPROPERTY()
    int32 MasterSkeletonIndex; // Which index (000, 001, etc.) is the master

    UPROPERTY()
    TMap<EPedBodyPart, USkeletalMesh*> ComponentSkeletons; // Track each component's skeleton

    UPROPERTY()
    TMap<EPedBodyPart, int32> ComponentIndices; // Track current index for each component

    // === COMPONENT DISCOVERY CACHE ===
    
    // Which indices are available for each body part (not exposed to Blueprint due to TArray limitation)
    TMap<EPedBodyPart, TArray<int32>> AvailableComponentIndices;

    // === MASTER SKELETON RUNTIME VARIABLES ===
    
    UPROPERTY()
    USkeletalMeshComponent* CurrentMasterSkeleton; // Current master skeleton component
    
    UPROPERTY()
    FString MasterSkeletonComponentName; // Name of the master skeleton component

    // Map to track all skeletal mesh components by name (for master skeleton system)
    TMap<FString, USkeletalMeshComponent*> PedComponents;

    // Loading state
    bool bIsProfileLoaded;
    bool bIsLoadingAssets;
    float LoadingProgress;
    int32 TotalAssetsToLoad;
    int32 AssetsLoaded;

    // Loaded assets cache - organized by component type
    UPROPERTY()
    TMap<FString, USkeletalMesh*> LoadedComponentMeshes;

    UPROPERTY()
    TMap<FString, UStaticMesh*> LoadedPropMeshes;

    // Note: TMap with TArray requires special handling in UE5
    TMap<FString, TArray<UMaterialInterface*>> LoadedComponentMaterials;

    // XML Data cache
    TMap<FString, FPedVariationSet> VariationSets;
    TMap<FString, FPedComponentMesh> ComponentDefinitions;
    TMap<FString, FPedPropMesh> PropDefinitions;
    TMap<FString, TMap<int32, FString>> MaterialMappings;

    // Internal functions
    void InitializeComponent();
    void UpdateLoadingProgress();
    void CheckIfAllAssetsLoaded();
    void ApplyLoadedAssetsToMesh();
    FVector CalculateScaleWithVariations() const;
    USkeletalMeshComponent* FindSkeletalMeshComponent() const;

    // XML parsing functions
    void ParsePedMeshXMLTiny(tinyxml2::XMLDocument& Doc);
    void ParsePedVariationsXMLTiny(tinyxml2::XMLDocument& Doc);
    void ParsePedPropsXMLTiny(tinyxml2::XMLDocument& Doc);

    // Helper functions
    FPedComponentMesh* FindComponentInConfig(const FString& ComponentName);
    FPedPropMesh* FindPropInConfig(const FString& PropName);
    void ApplyComponentToSkeletalMesh(const FPedComponentMesh& ComponentMesh, USkeletalMeshComponent* TargetComponent);
    UStaticMeshComponent* CreatePropComponent(const FPedPropMesh& PropMesh);

    // === MASTER SKELETON HELPER FUNCTIONS ===
    void UpdateFollowerComponents();
};
