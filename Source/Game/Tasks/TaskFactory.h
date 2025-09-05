#pragma once

#include "CoreMinimal.h"
#include "UObject/NoExportTypes.h"
#include "Engine/World.h"
#include "BaseTask.h"
#include "Peds/OneShot/OneShotTask.h"
#include "Peds/Complex/ComplexTask.h"
#include "Peds/WildComplex/WildComplexTask.h"
#include "TaskFactory.generated.h"

class APed;

/**
 * Task Factory for creating and managing tasks for specific entities
 * Provides centralized task creation with proper initialization and validation
 */
UCLASS()
class GAME_API UTaskFactory : public UObject
{
    GENERATED_BODY()

public:
    UTaskFactory();

    // === OneShot Task Creation ===
    
    /** Create an Aim task for the specified ped */
    static UTask_Aim* CreateAimTask(APed* OwnerPed, AActor* Target, float AimDuration = 3.0f);

    /** Create a Look At task for the specified ped */
    static UTask_LookAt* CreateLookAtTask(APed* OwnerPed, AActor* Target, float LookDuration = 2.0f);

    /** Create a Turn task for the specified ped */
    static UTask_Turn* CreateTurnTask(APed* OwnerPed, FVector TargetDirection, float TurnSpeed = 180.0f);

    /** Create a Shimmy task for the specified ped */
    static UTask_Shimmy* CreateShimmyTask(APed* OwnerPed, FVector ShimmyDirection, float ShimmyDistance = 100.0f);

    /** Create a Drop Down task for the specified ped */
    static UTask_DropDown* CreateDropDownTask(APed* OwnerPed, AActor* LedgeActor = nullptr);

    /** Create a Jump task for the specified ped */
    static UTask_Jump* CreateJumpTask(APed* OwnerPed, const FVector& JumpTarget, float JumpHeight = 200.0f);

    /** Create a Move Towards task for the specified ped */
    static UTask_MoveTowards* CreateMoveTowardsTask(APed* OwnerPed, const FVector& TargetLocation, float MovementSpeed = 300.0f);

    /** Create a Move Towards Actor task for the specified ped */
    static UTask_MoveTowards* CreateMoveTowardsActorTask(APed* OwnerPed, AActor* TargetActor, float MovementSpeed = 300.0f);

    // === Complex Task Creation ===

    /** Create a Climb task for the specified ped */
    static UTask_Climb* CreateClimbTask(APed* OwnerPed, AActor* ClimbTarget, float ClimbHeight = 200.0f);

    /** Create an Enter Vehicle task for the specified ped */
    static UTask_EnterVehicle* CreateEnterVehicleTask(APed* OwnerPed, AActor* Vehicle, int32 SeatIndex = 0);

    /** Create a Grab Ledge and Hold task for the specified ped */
    static UTask_GrabLedgeAndHold* CreateGrabLedgeTask(APed* OwnerPed, AActor* LedgeActor, float HoldDuration = 5.0f);

    /** Create a Climb Ladder task for the specified ped */
    static UTask_ClimbLadder* CreateClimbLadderTask(APed* OwnerPed, AActor* LadderActor, bool bClimbUp = true);

    // === WildComplex Task Creation ===

    /** Create a Fight Against task for the specified ped */
    static UTask_FightAgainst* CreateFightAgainstTask(APed* OwnerPed, APed* Enemy, float FightDuration = 10.0f);

    /** Create a Combat Targets task for the specified ped */
    static UTask_CombatTargets* CreateCombatTargetsTask(APed* OwnerPed, const TArray<APed*>& Targets, float CombatDuration = 15.0f);

    // === Utility Functions ===

    /** Create a task of any type using class reference */
    static UBaseTask* CreateTaskOfClass(TSubclassOf<UBaseTask> TaskClass, APed* OwnerPed, AActor* Target = nullptr);

    /** Validate if a ped can execute a specific task type */
    static bool CanPedExecuteTask(APed* OwnerPed, TSubclassOf<UBaseTask> TaskClass, AActor* Target = nullptr);

    /** Get all available task classes for a ped */
    static TArray<TSubclassOf<UBaseTask>> GetAvailableTaskClasses(APed* OwnerPed);

    /** Create multiple tasks in sequence for a ped */
    static TArray<UBaseTask*> CreateTaskSequence(APed* OwnerPed, const TArray<TSubclassOf<UBaseTask>>& TaskClasses, const TArray<AActor*>& Targets);

    // === Batch Task Creation ===

    /** Create aim tasks for multiple peds targeting the same actor */
    static TArray<UTask_Aim*> CreateMultiPedAimTasks(const TArray<APed*>& Peds, AActor* Target, float AimDuration = 3.0f);

    /** Create combat tasks for multiple peds against multiple targets */
    static TArray<UTask_CombatTargets*> CreateMultiPedCombatTasks(const TArray<APed*>& Peds, const TArray<APed*>& Targets, float CombatDuration = 15.0f);

    // === Task Creation with Validation ===

    /** Create task with full validation and error checking */
    static UBaseTask* CreateValidatedTask(TSubclassOf<UBaseTask> TaskClass, APed* OwnerPed, AActor* Target, bool& bOutSuccess, FString& OutErrorMessage);

protected:
    /** Internal validation for task creation parameters */
    static bool ValidateTaskCreationParams(APed* OwnerPed, TSubclassOf<UBaseTask> TaskClass, FString& OutErrorMessage);

    /** Internal helper to set common task properties */
    static void SetCommonTaskProperties(UBaseTask* Task, APed* OwnerPed, AActor* Target);

    /** Internal helper to configure task-specific properties */
    static void ConfigureTaskSpecificProperties(UBaseTask* Task, const TMap<FString, FString>& Properties);

private:
    /** Cached task class references for performance */
    static TMap<FString, TSubclassOf<UBaseTask>> CachedTaskClasses;

    /** Initialize cached task classes */
    static void InitializeCachedTaskClasses();

    /** Get task class by name from cache */
    static TSubclassOf<UBaseTask> GetCachedTaskClass(const FString& TaskName);
};
