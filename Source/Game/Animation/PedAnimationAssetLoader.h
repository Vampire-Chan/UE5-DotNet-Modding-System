#pragma once

#include "CoreMinimal.h"
#include "UObject/NoExportTypes.h"
#include "Engine/DataAsset.h"
#include "Animation/AnimSequence.h"
#include "PedAnimationAssetLoader.generated.h"

/**
 * Data asset for managing animation file paths and loading
 * Maps .onim files to their converted .uasset equivalents
 */
USTRUCT(BlueprintType)
struct FAnimationFileMapping
{
    GENERATED_BODY()

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Animation Mapping")
    FString OnimFilePath;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Animation Mapping")
    FString UAssetPath;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Animation Mapping")
    TSoftObjectPtr<UAnimSequence> AnimationAsset;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Animation Mapping")
    FString AnimationName;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Animation Mapping")
    bool bIsLooped;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Animation Mapping")
    float DefaultPlayRate;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Animation Mapping")
    float DefaultBlendTime;

    FAnimationFileMapping()
    {
        OnimFilePath = TEXT("");
        UAssetPath = TEXT("");
        AnimationName = TEXT("");
        bIsLooped = false;
        DefaultPlayRate = 1.0f;
        DefaultBlendTime = 0.15f;
    }
};

USTRUCT(BlueprintType)
struct FAnimationCategory
{
    GENERATED_BODY()

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Animation Category")
    FString CategoryName;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Animation Category")
    TArray<FAnimationFileMapping> Animations;

    FAnimationCategory()
    {
        CategoryName = TEXT("");
    }
};

/**
 * Primary Asset Loader for Ped Animations
 * Handles loading and caching of all character animations
 */
UCLASS(BlueprintType, Blueprintable)
class GAME_API UPedAnimationAssetLoader : public UPrimaryDataAsset
{
    GENERATED_BODY()

public:
    UPedAnimationAssetLoader();

    // Animation Categories
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Player Movement")
    FAnimationCategory PlayerMovement;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Crouch Movement")
    FAnimationCategory CrouchMovement;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Jump Animations")
    FAnimationCategory JumpAnimations;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Combat Movement")
    FAnimationCategory CombatMovement;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Combat Strafe")
    FAnimationCategory CombatStrafe;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Climb Animations")
    FAnimationCategory ClimbAnimations;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Vehicle Animations")
    FAnimationCategory VehicleAnimations;

    // Loading Functions
    UFUNCTION(BlueprintCallable, Category = "Animation Loading")
    void InitializeAnimationMappings();

    UFUNCTION(BlueprintCallable, Category = "Animation Loading")
    UAnimSequence* GetAnimationByName(const FString& AnimationName);

    UFUNCTION(BlueprintCallable, Category = "Animation Loading")
    TArray<UAnimSequence*> GetAnimationsByCategory(const FString& CategoryName);

    UFUNCTION(BlueprintCallable, Category = "Animation Loading")
    bool LoadAllAnimations();

    UFUNCTION(BlueprintCallable, Category = "Animation Loading")
    bool IsAnimationLoaded(const FString& AnimationName);

    UFUNCTION(BlueprintCallable, Category = "Animation Loading")
    UAnimSequence* LoadAnimationAsset(const FString& AnimationPath);

    // Utility Functions
    UFUNCTION(BlueprintCallable, Category = "Animation Utility")
    TArray<FString> GetAllAnimationNames();

    UFUNCTION(BlueprintCallable, Category = "Animation Utility")
    TArray<FString> GetAnimationNamesByCategory(const FString& CategoryName);

    UFUNCTION(BlueprintCallable, Category = "Animation Utility")
    FAnimationFileMapping GetAnimationMapping(const FString& AnimationName);

private:
    // Internal Functions
    void CreatePlayerMovementMappings();
    void CreateCrouchMovementMappings();
    void CreateJumpAnimationMappings();
    void CreateCombatMovementMappings();
    void CreateClimbAnimationMappings();
    void CreateVehicleAnimationMappings();

    FAnimationFileMapping CreateMapping(const FString& OnimPath, const FString& AnimName, bool bLoop = false, float PlayRate = 1.0f);
    FString ConvertOnimPathToUAsset(const FString& OnimPath);

    // Cache for loaded animations
    UPROPERTY()
    TMap<FString, UAnimSequence*> LoadedAnimations;

    // Flag to track initialization
    bool bInitialized;
};
