#pragma once

#include "CoreMinimal.h"
#include "UObject/NoExportTypes.h"
#include "Engine/World.h"
#include "../../Core/Enums/GameWorldEnums.h"
#include "BaseTask.generated.h"

UENUM(BlueprintType)
enum class ETaskType : uint8
{
    OneShot             UMETA(DisplayName = "One Shot"),
    Complex             UMETA(DisplayName = "Complex"),
    WildComplex         UMETA(DisplayName = "Wild Complex")
};

UENUM(BlueprintType)
enum class ETaskState : uint8
{
    Idle                UMETA(DisplayName = "Idle"),
    Starting            UMETA(DisplayName = "Starting"),
    Running             UMETA(DisplayName = "Running"),
    Paused              UMETA(DisplayName = "Paused"),
    Completed           UMETA(DisplayName = "Completed"),
    Failed              UMETA(DisplayName = "Failed"),
    Cancelled           UMETA(DisplayName = "Cancelled"),
    Interrupted         UMETA(DisplayName = "Interrupted")
};

UENUM(BlueprintType)
enum class ETaskPriority : uint8
{
    Lowest              UMETA(DisplayName = "Lowest"),
    Low                 UMETA(DisplayName = "Low"),
    Normal              UMETA(DisplayName = "Normal"),
    High                UMETA(DisplayName = "High"),
    Critical            UMETA(DisplayName = "Critical"),
    Emergency           UMETA(DisplayName = "Emergency")
};

USTRUCT(BlueprintType)
struct FTaskResult
{
    GENERATED_BODY()

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Task Result")
    bool bSuccess;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Task Result")
    FString ResultMessage;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Task Result")
    float ExecutionTime;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Task Result")
    TMap<FString, FString> ResultData;

    FTaskResult()
    {
        bSuccess = false;
        ResultMessage = TEXT("");
        ExecutionTime = 0.0f;
    }
};

DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FOnTaskStateChanged, class UBaseTask*, Task, ETaskState, NewState);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FOnTaskCompleted, class UBaseTask*, Task, const FTaskResult&, Result);

/**
 * BaseTask - Base class for all ped tasks
 * Provides common functionality for task state management, execution flow, and result handling
 */
UCLASS(BlueprintType, Blueprintable, Abstract)
class GAME_API UBaseTask : public UObject
{
    GENERATED_BODY()

public:
    UBaseTask();

    // Core task functions
    UFUNCTION(BlueprintCallable, Category = "Task")
    virtual bool StartTask();

    UFUNCTION(BlueprintCallable, Category = "Task")
    virtual bool StopTask();

    UFUNCTION(BlueprintCallable, Category = "Task")
    virtual bool PauseTask();

    UFUNCTION(BlueprintCallable, Category = "Task")
    virtual bool ResumeTask();

    UFUNCTION(BlueprintCallable, Category = "Task")
    virtual void TickTask(float DeltaTime);

    UFUNCTION(BlueprintCallable, Category = "Task")
    virtual bool CanStartTask() const;

    UFUNCTION(BlueprintCallable, Category = "Task")
    virtual bool CanInterruptTask() const;

    // State management
    UFUNCTION(BlueprintPure, Category = "Task")
    ETaskState GetTaskState() const { return CurrentState; }

    UFUNCTION(BlueprintPure, Category = "Task")
    bool IsTaskActive() const { return CurrentState == ETaskState::Running; }

    UFUNCTION(BlueprintPure, Category = "Task")
    bool IsTaskCompleted() const { return CurrentState == ETaskState::Completed; }

    UFUNCTION(BlueprintPure, Category = "Task")
    bool IsTaskFailed() const { return CurrentState == ETaskState::Failed; }

    // Task properties
    UFUNCTION(BlueprintPure, Category = "Task")
    ETaskType GetTaskType() const { return TaskType; }

    UFUNCTION(BlueprintPure, Category = "Task")
    ETaskPriority GetTaskPriority() const { return TaskPriority; }

    // Task configuration methods
    virtual void Initialize(APed* Owner, AActor* Target = nullptr);
    virtual void SetPriority(ETaskPriority Priority) { TaskPriority = Priority; }
    virtual void SetTimeout(float TimeoutSeconds) { TimeoutDuration = TimeoutSeconds; bHasTimeout = true; }
    virtual bool RequiresTarget() const { return bRequiresTarget; }

    UFUNCTION(BlueprintPure, Category = "Task")
    FString GetTaskName() const { return TaskName; }

    UFUNCTION(BlueprintPure, Category = "Task")
    float GetExecutionTime() const { return ExecutionTime; }

    UFUNCTION(BlueprintPure, Category = "Task")
    float GetTimeoutDuration() const { return TimeoutDuration; }

    // Configuration
    UFUNCTION(BlueprintCallable, Category = "Task")
    void SetTaskPriority(ETaskPriority Priority) { TaskPriority = Priority; }

    UFUNCTION(BlueprintCallable, Category = "Task")
    void SetTimeoutDuration(float Duration) { TimeoutDuration = Duration; }

    UFUNCTION(BlueprintCallable, Category = "Task")
    void SetTaskTarget(AActor* Target) { TaskTarget = Target; }

    UFUNCTION(BlueprintPure, Category = "Task")
    AActor* GetTaskTarget() const { return TaskTarget; }

    // Events
    UPROPERTY(BlueprintAssignable, Category = "Task Events")
    FOnTaskStateChanged OnTaskStateChanged;

    UPROPERTY(BlueprintAssignable, Category = "Task Events")
    FOnTaskCompleted OnTaskCompleted;

protected:
    // Virtual functions to be implemented by derived classes
    virtual bool InitializeTask() { return true; }
    virtual bool ExecuteTask() { return true; }
    virtual void UpdateTask(float DeltaTime) {}
    virtual void CleanupTask() {}
    virtual bool ValidateTaskConditions() const { return true; }

    // State management helpers
    void SetTaskState(ETaskState NewState);
    void CompleteTask(bool bSuccess, const FString& Message = TEXT(""));

    // Task properties
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Task Config")
    ETaskType TaskType;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Task Config")
    ETaskPriority TaskPriority;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Task Config")
    FString TaskName;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Task Config")
    FString TaskDescription;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Task Config")
    float TimeoutDuration;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Task Config")
    bool bCanBeInterrupted;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Task Config")
    bool bRequiresTarget;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Task Config")
    bool bLoopTask;

    // Task target and context
    UPROPERTY(BlueprintReadOnly, Category = "Task Runtime")
    AActor* TaskTarget;

    UPROPERTY(BlueprintReadOnly, Category = "Task Runtime")
    class APed* OwnerPed;

    // Runtime state
    UPROPERTY(BlueprintReadOnly, Category = "Task Runtime")
    ETaskState CurrentState;

    UPROPERTY(BlueprintReadOnly, Category = "Task Runtime")
    float ExecutionTime;

    UPROPERTY(BlueprintReadOnly, Category = "Task Runtime")
    float StartTime;

    UPROPERTY(BlueprintReadOnly, Category = "Task Runtime")
    FTaskResult LastResult;

    // Internal tracking
    bool bIsInitialized;
    bool bHasTimeout;

public:
    // Task factory function
    UFUNCTION(BlueprintCallable, Category = "Task Factory", meta = (DeterminesOutputType = "TaskClass"))
    static UBaseTask* CreateTask(TSubclassOf<UBaseTask> TaskClass, APed* Owner, AActor* Target = nullptr);

    // Utility functions
    UFUNCTION(BlueprintPure, Category = "Task Utils")
    bool HasTimedOut() const;

    UFUNCTION(BlueprintPure, Category = "Task Utils")
    float GetRemainingTime() const;

    UFUNCTION(BlueprintPure, Category = "Task Utils")
    float GetProgress() const;

    UFUNCTION(BlueprintCallable, Category = "Task Utils")
    void AddTaskData(const FString& Key, const FString& Value);

    UFUNCTION(BlueprintPure, Category = "Task Utils")
    FString GetTaskData(const FString& Key) const;
};
