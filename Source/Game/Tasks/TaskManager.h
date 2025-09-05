#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "BaseTask.h"
#include "TaskManager.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE_ThreeParams(FOnTaskManagerStateChanged, class UTaskManager*, Manager, class UBaseTask*, Task, ETaskState, NewState);

/**
 * TaskManager - Manages and coordinates multiple tasks for a Ped
 * Handles task prioritization, execution order, and interruption logic
 */
UCLASS(BlueprintType, Blueprintable, ClassGroup=(Custom), meta=(BlueprintSpawnableComponent))
class GAME_API UTaskManager : public UActorComponent
{
    GENERATED_BODY()

public:
    UTaskManager();

protected:
    virtual void BeginPlay() override;
    virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

public:
    // Task management
    UFUNCTION(BlueprintCallable, Category = "Task Manager")
    bool AddTask(UBaseTask* Task);

    UFUNCTION(BlueprintCallable, Category = "Task Manager")
    bool RemoveTask(UBaseTask* Task);

    UFUNCTION(BlueprintCallable, Category = "Task Manager")
    bool StartTask(UBaseTask* Task);

    UFUNCTION(BlueprintCallable, Category = "Task Manager")
    bool StopTask(UBaseTask* Task);

    UFUNCTION(BlueprintCallable, Category = "Task Manager")
    bool InterruptCurrentTask();

    UFUNCTION(BlueprintCallable, Category = "Task Manager")
    void ClearAllTasks();

    // Task queries
    UFUNCTION(BlueprintPure, Category = "Task Manager")
    UBaseTask* GetCurrentTask() const { return CurrentTask; }

    UFUNCTION(BlueprintPure, Category = "Task Manager")
    TArray<UBaseTask*> GetPendingTasks() const { return PendingTasks; }

    UFUNCTION(BlueprintPure, Category = "Task Manager")
    TArray<UBaseTask*> GetCompletedTasks() const { return CompletedTasks; }

    UFUNCTION(BlueprintPure, Category = "Task Manager")
    bool HasActiveTasks() const;

    UFUNCTION(BlueprintPure, Category = "Task Manager")
    int32 GetTaskCount() const;

    // Task creation helpers
    UFUNCTION(BlueprintCallable, Category = "Task Manager")
    UBaseTask* CreateAndAddTask(TSubclassOf<UBaseTask> TaskClass, AActor* Target = nullptr);

    // OneShot task helpers
    UFUNCTION(BlueprintCallable, Category = "Task Manager|OneShot")
    class UTask_Aim* AddAimTask(AActor* Target);

    UFUNCTION(BlueprintCallable, Category = "Task Manager|OneShot")
    class UTask_LookAt* AddLookAtTask(AActor* Target, FVector Direction = FVector::ZeroVector);

    UFUNCTION(BlueprintCallable, Category = "Task Manager|OneShot")
    class UTask_Turn* AddTurnTask(FRotator TargetRotation, AActor* Target = nullptr);

    UFUNCTION(BlueprintCallable, Category = "Task Manager|OneShot")
    class UTask_Shimmy* AddShimmyTask(FVector Direction, float Distance);

    UFUNCTION(BlueprintCallable, Category = "Task Manager|OneShot")
    class UTask_DropDown* AddDropDownTask(float DropHeight = 200.0f);

    // Complex task helpers
    UFUNCTION(BlueprintCallable, Category = "Task Manager|Complex")
    class UTask_Climb* AddClimbTask(FVector ClimbTarget, float ClimbHeight = 200.0f);

    UFUNCTION(BlueprintCallable, Category = "Task Manager|Complex")
    class UTask_EnterVehicle* AddEnterVehicleTask(AActor* Vehicle, int32 SeatIndex = 0);

    UFUNCTION(BlueprintCallable, Category = "Task Manager|Complex")
    class UTask_GrabLedgeAndHold* AddGrabLedgeTask(FVector LedgeLocation = FVector::ZeroVector, float HoldDuration = 5.0f);

    UFUNCTION(BlueprintCallable, Category = "Task Manager|Complex")
    class UTask_ClimbLadder* AddClimbLadderTask(AActor* Ladder, bool bClimbUp = true);

    // WildComplex task helpers
    UFUNCTION(BlueprintCallable, Category = "Task Manager|WildComplex")
    class UTask_FightAgainst* AddFightTask(AActor* Opponent);

    UFUNCTION(BlueprintCallable, Category = "Task Manager|WildComplex")
    class UTask_CombatTargets* AddCombatTargetsTask(const TArray<AActor*>& Targets);

    // Events
    UPROPERTY(BlueprintAssignable, Category = "Task Manager Events")
    FOnTaskManagerStateChanged OnTaskManagerStateChanged;

protected:
    // Task execution logic
    void ProcessTaskQueue();
    bool CanStartTask(UBaseTask* Task) const;
    void OnTaskCompleted(UBaseTask* Task, const FTaskResult& Result);
    void OnTaskStateChanged(UBaseTask* Task, ETaskState NewState);

    // Priority management
    void SortTasksByPriority();
    bool ShouldInterruptForHigherPriority(UBaseTask* NewTask) const;

    // Task arrays
    UPROPERTY(BlueprintReadOnly, Category = "Task Manager")
    UBaseTask* CurrentTask;

    UPROPERTY(BlueprintReadOnly, Category = "Task Manager")
    TArray<UBaseTask*> PendingTasks;

    UPROPERTY(BlueprintReadOnly, Category = "Task Manager")
    TArray<UBaseTask*> CompletedTasks;

    UPROPERTY(BlueprintReadOnly, Category = "Task Manager")
    TArray<UBaseTask*> FailedTasks;

    // Configuration
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Task Manager Config")
    int32 MaxConcurrentTasks;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Task Manager Config")
    bool bAllowTaskInterruption;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Task Manager Config")
    bool bAutoStartTasks;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Task Manager Config")
    float TaskProcessingRate;

    // Runtime state
    UPROPERTY(BlueprintReadOnly, Category = "Task Manager Runtime")
    bool bIsProcessingTasks;

    UPROPERTY(BlueprintReadOnly, Category = "Task Manager Runtime")
    float LastProcessingTime;

private:
    // Owner reference
    class APed* OwnerPed;

    // Internal tracking
    bool bComponentInitialized;
};
