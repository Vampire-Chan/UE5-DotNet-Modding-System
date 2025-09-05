#pragma once

#include "CoreMinimal.h"
#include "UObject/NoExportTypes.h"
#include "Engine/DataAsset.h"
#include "PedAnimationDictionary.h"
#include "AnimationGroupsLoader.generated.h"

/**
 * Entity Type for Animation Dictionaries
 */
UENUM(BlueprintType)
enum class EAnimationEntityType : uint8
{
    PED         UMETA(DisplayName = "Ped/Character"),
    VEHICLE     UMETA(DisplayName = "Vehicle"),
    OBJECT      UMETA(DisplayName = "Object/Prop"),
    BUILDING    UMETA(DisplayName = "Building"),
    UNKNOWN     UMETA(DisplayName = "Unknown")
};

/**
 * Animation Dictionary Definition from XML
 */
USTRUCT(BlueprintType)
struct FAnimationDictionaryDefinition
{
    GENERATED_BODY()

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Dictionary Definition")
    FString Name;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Dictionary Definition")
    FString Path;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Dictionary Definition")
    EAnimationEntityType EntityType;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Dictionary Definition")
    FString Description;

    FAnimationDictionaryDefinition()
    {
        Name = TEXT("");
        Path = TEXT("");
        EntityType = EAnimationEntityType::UNKNOWN;
        Description = TEXT("");
    }
};

/**
 * Animation Groups Loader
 * Reads AnimationGroups.xml and automatically sets up animation dictionaries
 */
UCLASS(BlueprintType, Blueprintable)
class GAME_API UAnimationGroupsLoader : public UObject
{
    GENERATED_BODY()

public:
    UAnimationGroupsLoader();

    // Loaded Animation Dictionary Definitions
    UPROPERTY(BlueprintReadOnly, Category = "Animation Groups")
    TArray<FAnimationDictionaryDefinition> LoadedDictionaries;

    // Loading Functions
    UFUNCTION(BlueprintCallable, Category = "Animation Groups Loading")
    bool LoadAnimationGroupsFromXML(const FString& XMLFilePath = TEXT(""));

    UFUNCTION(BlueprintCallable, Category = "Animation Groups Loading")
    void SetupDictionariesFromGroups(UPedAnimationDictionary* TargetDictionary);

    UFUNCTION(BlueprintCallable, Category = "Animation Groups Loading")
    void SetupContextsFromGroups(UPedAnimationDictionary* TargetDictionary);

    // Query Functions
    UFUNCTION(BlueprintCallable, Category = "Animation Groups Query")
    TArray<FAnimationDictionaryDefinition> GetDictionariesByEntityType(EAnimationEntityType EntityType);

    UFUNCTION(BlueprintCallable, Category = "Animation Groups Query")
    FAnimationDictionaryDefinition GetDictionaryByName(const FString& DictionaryName);

    UFUNCTION(BlueprintCallable, Category = "Animation Groups Query")
    TArray<FString> GetAllDictionaryNames();

    UFUNCTION(BlueprintCallable, Category = "Animation Groups Query")
    TArray<FString> GetDictionaryNamesByEntityType(EAnimationEntityType EntityType);

    // Utility Functions
    UFUNCTION(BlueprintCallable, Category = "Animation Groups Utility")
    FString ConvertPathToUAssetPath(const FString& GamePath);

    UFUNCTION(BlueprintCallable, Category = "Animation Groups Utility")
    EAnimationEntityType StringToEntityType(const FString& EntityTypeString);

    UFUNCTION(BlueprintCallable, Category = "Animation Groups Utility")
    FString EntityTypeToString(EAnimationEntityType EntityType);

private:
    // Internal XML parsing functions
    bool ParseXMLContent(const FString& XMLContent);
    FAnimationDictionaryDefinition ParseDictionaryElement(const FString& ElementContent);
    FString ExtractAttributeValue(const FString& ElementContent, const FString& AttributeName);
    TArray<FString> SplitXMLElements(const FString& XMLContent, const FString& ElementName);

    // Default animation entry creation for known folder structures
    void CreateDefaultAnimationEntries(FAnimationDictionary& Dictionary, const FString& DictionaryName);
    void CreatePlayerMovementEntries(FAnimationDictionary& Dictionary);
    void CreateCrouchMovementEntries(FAnimationDictionary& Dictionary);
    void CreateJumpStandardEntries(FAnimationDictionary& Dictionary);
    void CreateCombatStrafeEntries(FAnimationDictionary& Dictionary);
    void CreateVehicleStandardEntries(FAnimationDictionary& Dictionary);

    // Helper function to create animation entry
    FAnimationEntry CreateAnimEntry(const FString& AnimName, const FString& FileName, bool bLoop = false, float PlayRate = 1.0f);
};
