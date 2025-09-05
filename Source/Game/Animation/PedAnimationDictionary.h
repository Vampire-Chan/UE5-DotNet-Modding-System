#pragma once

#include "CoreMinimal.h"
#include "UObject/NoExportTypes.h"
#include "Engine/DataAsset.h"
#include "Animation/AnimSequence.h"
#include "PedAnimationDictionary.generated.h"

/**
 * Animation Dictionary Entry - maps animation names to file paths within a folder
 */
USTRUCT(BlueprintType)
struct FAnimationEntry
{
    GENERATED_BODY()

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Animation Entry")
    FString AnimationName;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Animation Entry")
    FString FileName; // Just the filename without path (e.g., "idle.onim")

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Animation Entry")
    bool bIsLooped;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Animation Entry")
    float DefaultPlayRate;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Animation Entry")
    float DefaultBlendTime;

    FAnimationEntry()
    {
        AnimationName = TEXT("");
        FileName = TEXT("");
        bIsLooped = false;
        DefaultPlayRate = 1.0f;
        DefaultBlendTime = 0.15f;
    }

    FAnimationEntry(const FString& InAnimName, const FString& InFileName, bool bLoop = false, float PlayRate = 1.0f, float BlendTime = 0.15f)
    {
        AnimationName = InAnimName;
        FileName = InFileName;
        bIsLooped = bLoop;
        DefaultPlayRate = PlayRate;
        DefaultBlendTime = BlendTime;
    }
};

/**
 * Animation Dictionary - defines a complete set of animations for a specific context
 */
USTRUCT(BlueprintType)
struct FAnimationDictionary
{
    GENERATED_BODY()

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Dictionary Info")
    FString DictionaryName;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Dictionary Info")
    FString BaseFolderPath; // e.g., "Data/Animations/move_player"

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Dictionary Info")
    FString Description;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Animation Entries")
    TArray<FAnimationEntry> Animations;

    // Validation: Required clips for this dictionary type
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Validation")
    TArray<FString> RequiredClips; // Must-have clips for this dictionary to be valid

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Validation")
    TArray<FString> OptionalClips; // Nice-to-have clips

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Validation")
    float MinimumValidationScore; // 0.0-1.0, minimum % of required clips that must exist

    FAnimationDictionary()
    {
        DictionaryName = TEXT("");
        BaseFolderPath = TEXT("");
        Description = TEXT("");
        MinimumValidationScore = 0.7f; // 70% of required clips must exist
    }
};

/**
 * Animation Context - groups related dictionaries that can be swapped
 */
USTRUCT(BlueprintType)
struct FAnimationContext
{
    GENERATED_BODY()

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Context Info")
    FString ContextName; // e.g., "Movement", "Combat", "Vehicle"

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Context Info")
    FString CurrentDictionaryName; // Currently active dictionary

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Available Dictionaries")
    TArray<FString> AvailableDictionaries; // List of dictionary names that can be used for this context

    FAnimationContext()
    {
        ContextName = TEXT("");
        CurrentDictionaryName = TEXT("");
    }
};

/**
 * Dynamic Animation Dictionary Manager
 * Handles swapping of animation sets at runtime
 */
UCLASS(BlueprintType, Blueprintable)
class GAME_API UPedAnimationDictionary : public UPrimaryDataAsset
{
    GENERATED_BODY()

public:
    UPedAnimationDictionary();

    // Dictionary Storage
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Dictionaries")
    TArray<FAnimationDictionary> AllDictionaries;

    // Animation Contexts
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Contexts")
    TArray<FAnimationContext> AnimationContexts;

    // Dictionary Management
    UFUNCTION(BlueprintCallable, Category = "Dictionary Management")
    void InitializeDefaultDictionaries();

    UFUNCTION(BlueprintCallable, Category = "Dictionary Management")
    bool SwapDictionary(const FString& ContextName, const FString& NewDictionaryName);

    UFUNCTION(BlueprintCallable, Category = "Dictionary Management")
    FAnimationDictionary GetDictionaryByName(const FString& DictionaryName);

    UFUNCTION(BlueprintCallable, Category = "Dictionary Management")
    FString GetCurrentDictionaryForContext(const FString& ContextName);

    UFUNCTION(BlueprintCallable, Category = "Dictionary Management")
    TArray<FString> GetAvailableDictionariesForContext(const FString& ContextName);

    // Animation Path Resolution
    UFUNCTION(BlueprintCallable, Category = "Animation Resolution")
    FString GetAnimationPath(const FString& ContextName, const FString& AnimationName);

    UFUNCTION(BlueprintCallable, Category = "Animation Resolution")
    FAnimationEntry GetAnimationEntry(const FString& ContextName, const FString& AnimationName);

    UFUNCTION(BlueprintCallable, Category = "Animation Resolution")
    TArray<FString> GetAllAnimationNamesInContext(const FString& ContextName);

    // Utility Functions
    UFUNCTION(BlueprintCallable, Category = "Dictionary Utility")
    bool AddDictionary(const FAnimationDictionary& NewDictionary);

    UFUNCTION(BlueprintCallable, Category = "Dictionary Utility")
    bool RemoveDictionary(const FString& DictionaryName);

    UFUNCTION(BlueprintCallable, Category = "Dictionary Utility")
    bool AddContext(const FAnimationContext& NewContext);

    UFUNCTION(BlueprintCallable, Category = "Dictionary Utility")
    TArray<FString> GetAllContextNames();

    UFUNCTION(BlueprintCallable, Category = "Dictionary Utility")
    TArray<FString> GetAllDictionaryNames();

    // Events
    UFUNCTION(BlueprintImplementableEvent, Category = "Dictionary Events")
    void OnDictionarySwapped(const FString& ContextName, const FString& OldDictionary, const FString& NewDictionary);

    // Dictionary Validation System
    UFUNCTION(BlueprintCallable, Category = "Dictionary Validation")
    bool ValidateDictionary(const FString& DictionaryName, FString& OutValidationReport);

    UFUNCTION(BlueprintCallable, Category = "Dictionary Validation")
    bool ValidateDictionaryForContext(const FString& ContextName, const FString& DictionaryName, FString& OutValidationReport);

    UFUNCTION(BlueprintCallable, Category = "Dictionary Validation")
    float GetDictionaryValidationScore(const FString& DictionaryName);

    UFUNCTION(BlueprintCallable, Category = "Dictionary Validation")
    TArray<FString> GetMissingRequiredClips(const FString& DictionaryName);

    UFUNCTION(BlueprintCallable, Category = "Dictionary Validation")
    TArray<FString> GetAvailableClips(const FString& DictionaryName);

    UFUNCTION(BlueprintCallable, Category = "Dictionary Validation")
    bool CanSafelySwapDictionary(const FString& ContextName, const FString& NewDictionaryName);

    // Safe Dictionary Swapping with Fallback
    UFUNCTION(BlueprintCallable, Category = "Safe Dictionary Management")
    bool SafeSwapDictionary(const FString& ContextName, const FString& NewDictionaryName, bool bAllowFallback = true);

    UFUNCTION(BlueprintCallable, Category = "Safe Dictionary Management")
    void SetupDictionaryValidationRules();

    // Built-in Validation Rules - replaces separate AnimationValidationRules class
    UFUNCTION(BlueprintCallable, Category = "Built-in Validation")
    void InitializeBuiltInValidationRules();

    UFUNCTION(BlueprintCallable, Category = "Built-in Validation")
    TArray<FString> GetRequiredClipsForDictionary(const FString& DictionaryName);

    UFUNCTION(BlueprintCallable, Category = "Built-in Validation")
    TArray<FString> GetOptionalClipsForDictionary(const FString& DictionaryName);

private:
    // Internal helper functions
    FAnimationContext* FindContext(const FString& ContextName);
    FAnimationDictionary* FindDictionary(const FString& DictionaryName);
    void CreateDefaultMovementDictionaries();
    void CreateDefaultCombatDictionaries();
    void CreateDefaultVehicleDictionaries();
    void CreateDefaultJumpDictionaries();
    FAnimationEntry CreateEntry(const FString& AnimName, const FString& FileName, bool bLoop = false, float PlayRate = 1.0f);

    // Validation helper functions
    bool ValidateRequiredClips(const FAnimationDictionary& Dictionary, TArray<FString>& OutMissingClips);
    float CalculateValidationScore(const FAnimationDictionary& Dictionary);
    FString GenerateValidationReport(const FAnimationDictionary& Dictionary);
};
