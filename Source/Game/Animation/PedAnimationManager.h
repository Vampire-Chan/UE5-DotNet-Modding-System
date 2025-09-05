#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "PedAnimationController.h"
#include "PedAnimationAssetLoader.h"
#include "PedAnimationDictionary.h"
#include "AnimationGroupsLoader.h"
#include "GameFramework/Character.h"
#include "PedAnimationManager.generated.h"

/**
 * Animation Manager Component for Ped/Player Characters
 * Handles high-level animation logic and state management
 */
UCLASS(BlueprintType, Blueprintable, ClassGroup=(Custom), meta=(BlueprintSpawnableComponent))
class GAME_API UPedAnimationManager : public UActorComponent
{
    GENERATED_BODY()

public:
    UPedAnimationManager();

protected:
    virtual void BeginPlay() override;
    virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

public:
    // Animation Asset Loader Reference
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Animation System")
    UPedAnimationAssetLoader* AnimationAssetLoader;

    // Animation Dictionary Reference
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Animation System")
    UPedAnimationDictionary* AnimationDictionary;

    // Animation Groups Loader Reference
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Animation System")
    UAnimationGroupsLoader* AnimationGroupsLoader;

    // Animation Groups XML File Path
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Animation System")
    FString AnimationGroupsXMLPath;

    // Animation Controller Reference (set automatically)
    UPROPERTY(BlueprintReadOnly, Category = "Animation System")
    UPedAnimationController* AnimationController;

    // Character Reference (set automatically)
    UPROPERTY(BlueprintReadOnly, Category = "Animation System")
    ACharacter* OwnerCharacter;

    // Animation Settings
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Animation Settings")
    float MovementThreshold = 5.0f;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Animation Settings")
    float WalkSpeedThreshold = 150.0f;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Animation Settings")
    float RunSpeedThreshold = 400.0f;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Animation Settings")
    float TurnAngleThreshold = 15.0f;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Animation Settings")
    bool bAutoManageAnimations = true;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Animation Settings")
    bool bEnableSmoothing = true;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Animation Settings")
    float SmoothingSpeed = 10.0f;

    // Public Animation Control Functions
    UFUNCTION(BlueprintCallable, Category = "Animation Control")
    void PlayAnimationByName(const FString& AnimationName, bool bForcePlay = false);

    UFUNCTION(BlueprintCallable, Category = "Animation Control")
    void StopCurrentAnimation();

    UFUNCTION(BlueprintCallable, Category = "Animation Control")
    void SetMovementState(EPedMovementState NewState);

    UFUNCTION(BlueprintCallable, Category = "Animation Control")
    void SetStanceState(EPedStanceState NewState);

    // Dictionary Management Functions
    UFUNCTION(BlueprintCallable, Category = "Dictionary Management")
    bool SwapAnimationDictionary(const FString& ContextName, const FString& NewDictionaryName);

    UFUNCTION(BlueprintCallable, Category = "Dictionary Management")
    FString GetCurrentDictionary(const FString& ContextName);

    UFUNCTION(BlueprintCallable, Category = "Dictionary Management")
    TArray<FString> GetAvailableDictionaries(const FString& ContextName);

    UFUNCTION(BlueprintCallable, Category = "Dictionary Management")
    void SwapMovementDictionary(const FString& NewDictionaryName);

    UFUNCTION(BlueprintCallable, Category = "Dictionary Management")
    void SwapCrouchDictionary(const FString& NewDictionaryName);

    UFUNCTION(BlueprintCallable, Category = "Dictionary Management")
    void SwapJumpDictionary(const FString& NewDictionaryName);

    UFUNCTION(BlueprintCallable, Category = "Dictionary Management")
    void SwapCombatDictionary(const FString& NewDictionaryName);

    UFUNCTION(BlueprintCallable, Category = "Dictionary Management")
    void SwapInVehicleDictionary(const FString& NewDictionaryName);

    UFUNCTION(BlueprintCallable, Category = "Dictionary Management")
    void SwapCoverDictionary(const FString& NewDictionaryName);

    UFUNCTION(BlueprintCallable, Category = "Dictionary Management")
    void SwapInteractionDictionary(const FString& NewDictionaryName);

    UFUNCTION(BlueprintCallable, Category = "Dictionary Management")
    void SwapVehicleDictionary(const FString& NewDictionaryName);

    // Region Management
    UFUNCTION(BlueprintCallable, Category = "Animation Region")
    void SetAnimationRegion(EPedAnimationRegion NewRegion);

    UFUNCTION(BlueprintCallable, Category = "Animation Region")
    EPedAnimationRegion GetCurrentAnimationRegion() const;

    UFUNCTION(BlueprintCallable, Category = "Animation Region")
    bool PlayAnimationInRegion(EPedAnimationRegion Region, const FString& AnimationName, float BlendTime = 0.2f, bool bLoop = false);

    // Vehicle Animation Sync
    UFUNCTION(BlueprintCallable, Category = "Vehicle Animation")
    void UpdateVehicleAnimationInputs(float SteeringInput, float ThrottleInput, float BrakeInput, int32 CurrentGear, float RPM, float Speed);

    // Combat Animation System
    UFUNCTION(BlueprintCallable, Category = "Combat Animation")
    bool PlayCombatAnimation(const FString& AnimationName, EPedWeaponType WeaponType = EPedWeaponType::None, float BlendTime = 0.15f, bool bLoop = false);

    UFUNCTION(BlueprintCallable, Category = "Combat Animation")
    bool SwitchWeaponAnimationSet(EPedWeaponType NewWeaponType);

    UFUNCTION(BlueprintCallable, Category = "Combat Animation")
    FString GetWeaponSpecificAnimationName(const FString& BaseAnimationName, EPedWeaponType WeaponType);

    UFUNCTION(BlueprintCallable, Category = "Combat Animation")
    void SetCombatState(EPedCombatState NewCombatState);

    UFUNCTION(BlueprintCallable, Category = "Combat Animation")
    void SetWeaponType(EPedWeaponType NewWeaponType);

    UFUNCTION(BlueprintCallable, Category = "Combat Animation")
    void EnterCombatMode(EPedWeaponType WeaponType);

    UFUNCTION(BlueprintCallable, Category = "Combat Animation")
    void ExitCombatMode();

    // Movement State Queries
    UFUNCTION(BlueprintCallable, BlueprintPure, Category = "Animation State")
    EPedMovementState GetCurrentMovementState() const;

    UFUNCTION(BlueprintCallable, BlueprintPure, Category = "Animation State")
    EPedStanceState GetCurrentStanceState() const;

    UFUNCTION(BlueprintCallable, BlueprintPure, Category = "Animation State")
    float GetCurrentSpeed() const;

    UFUNCTION(BlueprintCallable, BlueprintPure, Category = "Animation State")
    FString GetCurrentAnimationName() const;

    // Utility Functions
    UFUNCTION(BlueprintCallable, Category = "Animation Utility")
    TArray<FString> GetAvailableAnimations(const FString& CategoryName = TEXT(""));

    UFUNCTION(BlueprintCallable, Category = "Animation Utility")
    bool IsAnimationAvailable(const FString& AnimationName);

    UFUNCTION(BlueprintCallable, Category = "Animation Utility")
    void InitializeAnimationSystem();

    // Animation Groups Functions
    UFUNCTION(BlueprintCallable, Category = "Animation Groups")
    bool LoadAnimationGroupsFromXML();

    UFUNCTION(BlueprintCallable, Category = "Animation Groups")
    TArray<FString> GetAvailableAnimationGroups(const FString& EntityType = TEXT("PED"));

    // Dictionary Validation Functions
    UFUNCTION(BlueprintCallable, Category = "Dictionary Validation")
    bool ValidateCurrentDictionaries();

    UFUNCTION(BlueprintCallable, Category = "Dictionary Validation")
    FString GetDictionaryValidationReport(const FString& DictionaryName);

    UFUNCTION(BlueprintCallable, Category = "Dictionary Validation")
    TArray<FString> GetMissingClipsForDictionary(const FString& DictionaryName);

    UFUNCTION(BlueprintCallable, Category = "Dictionary Validation")
    bool CanSafelyUseDictionary(const FString& ContextName, const FString& DictionaryName);

    UFUNCTION(BlueprintCallable, Category = "Dictionary Validation")
    void SetupValidationRules();

    UFUNCTION(BlueprintCallable, Category = "Animation Groups")
    void SetupDictionariesFromXML();

    // Events
    UFUNCTION(BlueprintImplementableEvent, Category = "Animation Events")
    void OnMovementStateChanged(EPedMovementState OldState, EPedMovementState NewState);

    UFUNCTION(BlueprintImplementableEvent, Category = "Animation Events")
    void OnStanceStateChanged(EPedStanceState OldState, EPedStanceState NewState);

    UFUNCTION(BlueprintImplementableEvent, Category = "Animation Events")
    void OnAnimationStarted(const FString& AnimationName);

    UFUNCTION(BlueprintImplementableEvent, Category = "Animation Events")
    void OnAnimationFinished(const FString& AnimationName);

private:
    // Internal state tracking
    EPedMovementState LastMovementState;
    EPedStanceState LastStanceState;
    float LastSpeed;
    FString LastAnimationName;

    // Smoothing variables
    float SmoothedSpeed;
    float SmoothedDirection;

    // Timing
    float TimeSinceLastStateChange;
    float TimeSinceLastAnimation;

    // Internal functions
    void UpdateMovementDetection();
    void HandleAutoAnimationManagement();
    void UpdateSmoothingValues(float DeltaTime);
    void CheckForStateChanges();
    
    // Animation selection helpers
    FString SelectBestIdleAnimation();
    FString SelectBestWalkAnimation();
    FString SelectBestRunAnimation();
    FString SelectBestSprintAnimation();
    FString SelectBestJumpAnimation();
    FString SelectBestCrouchAnimation();
    
    // Combat helper functions
    FString GetWeaponPrefix(EPedWeaponType WeaponType);
    FString GetWeaponDictionaryName(EPedWeaponType WeaponType);
};
