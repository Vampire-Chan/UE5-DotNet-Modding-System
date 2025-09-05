#pragma once

#include "CoreMinimal.h"
#include "../../BaseTask.h"
#include "WildComplexTask.generated.h"

UENUM(BlueprintType)
enum class EWildComplexTaskState : uint8
{
    Initializing        UMETA(DisplayName = "Initializing"),
    Analyzing           UMETA(DisplayName = "Analyzing"),
    Planning            UMETA(DisplayName = "Planning"),
    Executing           UMETA(DisplayName = "Executing"),
    Adapting            UMETA(DisplayName = "Adapting"),
    Finalizing          UMETA(DisplayName = "Finalizing")
};

UENUM(BlueprintType)
enum class ECombatStrategy : uint8
{
    Aggressive          UMETA(DisplayName = "Aggressive"),
    Defensive           UMETA(DisplayName = "Defensive"),
    Tactical            UMETA(DisplayName = "Tactical"),
    Stealth             UMETA(DisplayName = "Stealth"),
    Retreat             UMETA(DisplayName = "Retreat"),
    Adaptive            UMETA(DisplayName = "Adaptive")
};

UENUM(BlueprintType)
enum class EFightingStyle : uint8
{
    Boxing              UMETA(DisplayName = "Boxing"),
    MartialArts         UMETA(DisplayName = "Martial Arts"),
    StreetFighting      UMETA(DisplayName = "Street Fighting"),
    Wrestling           UMETA(DisplayName = "Wrestling"),
    Mixed               UMETA(DisplayName = "Mixed Style")
};

/**
 * WildComplexTask - Base class for highly complex, AI-driven tasks
 * These tasks involve multiple systems, dynamic decision making, and complex state management
 */
UCLASS(BlueprintType, Blueprintable, Abstract)
class GAME_API UWildComplexTask : public UBaseTask
{
    GENERATED_BODY()

public:
    UWildComplexTask();

protected:
    virtual bool ExecuteTask() override;
    virtual void UpdateTask(float DeltaTime) override;
    virtual void CleanupTask() override;

    // WildComplex specific functions
    virtual bool InitializeComplexSystems() { return true; }
    virtual bool AnalyzeSituation(float DeltaTime) { return true; }
    virtual bool PlanActions(float DeltaTime) { return true; }
    virtual bool ExecuteComplexActions(float DeltaTime) { return true; }
    virtual bool AdaptToChanges(float DeltaTime) { return true; }
    virtual bool FinalizeExecution() { return true; }

    // State management
    void SetWildComplexState(EWildComplexTaskState NewState);
    bool ShouldTransitionState() const;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "WildComplex Config")
    float AnalysisUpdateRate;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "WildComplex Config")
    float PlanningUpdateRate;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "WildComplex Config")
    float AdaptationThreshold;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "WildComplex Config")
    bool bEnableDynamicAdaptation;

    UPROPERTY(BlueprintReadOnly, Category = "WildComplex Runtime")
    EWildComplexTaskState CurrentWildState;

    UPROPERTY(BlueprintReadOnly, Category = "WildComplex Runtime")
    float StateTime;

    UPROPERTY(BlueprintReadOnly, Category = "WildComplex Runtime")
    int32 AdaptationCount;

protected:
    bool bSystemsInitialized;

private:
    float LastAnalysisTime;
    float LastPlanningTime;
};

/**
 * Task_FightAgainst - Engage in combat with another character
 */
UCLASS()
class GAME_API UTask_FightAgainst : public UWildComplexTask
{
    GENERATED_BODY()

public:
    UTask_FightAgainst();

    // Public setter methods for TaskManager access
    void SetFightDuration(float Duration) { FightDuration = Duration; }

protected:
    virtual bool InitializeComplexSystems() override;
    virtual bool AnalyzeSituation(float DeltaTime) override;
    virtual bool PlanActions(float DeltaTime) override;
    virtual bool ExecuteComplexActions(float DeltaTime) override;
    virtual bool AdaptToChanges(float DeltaTime) override;
    virtual bool ValidateTaskConditions() const override;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Fight Config")
    class APed* Opponent;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Fight Config")
    EFightingStyle FightingStyle;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Fight Config")
    float AggressionLevel;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Fight Config")
    float SkillLevel;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Fight Config")
    float StaminaConsumptionRate;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Fight Config")
    bool bUseEnvironmentalObjects;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Fight Config")
    float FightDuration;

private:
    // Combat state
    FVector LastKnownOpponentLocation;
    float OpponentDistance;
    float OpponentHealth;
    float CurrentStamina;
    float CombatIntensity;
    
    // Combat actions
    TArray<FString> AvailableAttacks;
    TArray<FString> AvailableDefenses;
    FString CurrentAction;
    float ActionCooldown;
    
    // Analysis data
    bool bOpponentIsAggressive;
    bool bOpponentIsBlocking;
    bool bOpponentIsTired;
    FVector OpponentMovementDirection;
    float OpponentSpeed;

    // Internal functions
    void UpdateCombatAnalysis();
    void SelectBestAction();
    void ExecuteCurrentAction(float DeltaTime);
    bool CheckForCombatEnd() const;
};

/**
 * Task_CombatTargets - Engage multiple targets in combat
 */
UCLASS()
class GAME_API UTask_CombatTargets : public UWildComplexTask
{
    GENERATED_BODY()

public:
    UTask_CombatTargets();

    // Public setter methods for TaskManager access
    void SetTargets(const TArray<APed*>& Targets);
    void SetCombatDuration(float Duration) { CombatDuration = Duration; }

protected:
    virtual bool InitializeComplexSystems() override;
    virtual bool AnalyzeSituation(float DeltaTime) override;
    virtual bool PlanActions(float DeltaTime) override;
    virtual bool ExecuteComplexActions(float DeltaTime) override;
    virtual bool AdaptToChanges(float DeltaTime) override;
    virtual bool ValidateTaskConditions() const override;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Combat Config")
    TArray<AActor*> CombatTargets;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Combat Config")
    ECombatStrategy CombatStrategy;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Combat Config")
    float EngagementRange;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Combat Config")
    int32 MaxSimultaneousTargets;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Combat Config")
    bool bUseCover;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Combat Config")
    bool bUseWeapons;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Combat Config")
    float CombatDuration;

private:
    // Target management
    struct FTargetInfo
    {
        AActor* Target;
        float Distance;
        float ThreatLevel;
        float LastSeenTime;
        FVector LastKnownPosition;
        bool bIsAlive;
        bool bIsAggressive;
        float Health;
    };

    TArray<FTargetInfo> TargetDatabase;
    AActor* PrimaryTarget;
    AActor* SecondaryTarget;
    
    // Combat state
    FVector CurrentCoverPosition;
    bool bInCover;
    float SuppressiveFire;
    int32 ActiveTargets;
    
    // Strategy execution
    float StrategyTimer;
    bool bNeedStrategyChange;
    ECombatStrategy PreviousStrategy;
    
    // Weapon state
    bool bHasWeapon;
    bool bIsReloading;
    int32 AmmoCount;
    float WeaponRange;

    // Internal functions
    void UpdateTargetDatabase();
    void SelectPrimaryTarget();
    void DetermineOptimalStrategy();
    void ExecuteCurrentStrategy(float DeltaTime);
    void HandleCoverMovement(float DeltaTime);
    void HandleWeaponCombat(float DeltaTime);
    void HandleMeleeCombat(float DeltaTime);
    bool ShouldRetreat() const;
    bool ShouldAdvance() const;
};
