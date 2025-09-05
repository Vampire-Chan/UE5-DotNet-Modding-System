#include "BaseTask.h"
#include "../Peds/Ped.h"
#include "Engine/World.h"
#include "Engine/Engine.h"

UBaseTask::UBaseTask()
{
    TaskType = ETaskType::OneShot;
    TaskPriority = ETaskPriority::Normal;
    TaskName = TEXT("BaseTask");
    TaskDescription = TEXT("Base task implementation");
    TimeoutDuration = 30.0f;
    bCanBeInterrupted = true;
    bRequiresTarget = false;
    bLoopTask = false;
    
    TaskTarget = nullptr;
    OwnerPed = nullptr;
    CurrentState = ETaskState::Idle;
    ExecutionTime = 0.0f;
    StartTime = 0.0f;
    
    bIsInitialized = false;
    bHasTimeout = true;
}

bool UBaseTask::StartTask()
{
    if (CurrentState != ETaskState::Idle)
    {
        UE_LOG(LogTemp, Warning, TEXT("Task %s: Cannot start task in state %d"), *TaskName, (int32)CurrentState);
        return false;
    }

    if (!CanStartTask())
    {
        UE_LOG(LogTemp, Warning, TEXT("Task %s: Cannot start task - conditions not met"), *TaskName);
        return false;
    }

    if (bRequiresTarget && !TaskTarget)
    {
        UE_LOG(LogTemp, Error, TEXT("Task %s: Cannot start task - no target specified but required"), *TaskName);
        return false;
    }

    SetTaskState(ETaskState::Starting);

    // Initialize the task
    if (!InitializeTask())
    {
        UE_LOG(LogTemp, Error, TEXT("Task %s: Failed to initialize task"), *TaskName);
        CompleteTask(false, TEXT("Failed to initialize"));
        return false;
    }

    bIsInitialized = true;
    StartTime = GetWorld() ? GetWorld()->GetTimeSeconds() : 0.0f;
    ExecutionTime = 0.0f;

    // Start task execution
    SetTaskState(ETaskState::Running);
    
    if (!ExecuteTask())
    {
        UE_LOG(LogTemp, Error, TEXT("Task %s: Failed to execute task"), *TaskName);
        CompleteTask(false, TEXT("Failed to execute"));
        return false;
    }

    UE_LOG(LogTemp, Log, TEXT("Task %s: Started successfully"), *TaskName);
    return true;
}

bool UBaseTask::StopTask()
{
    if (CurrentState == ETaskState::Idle || CurrentState == ETaskState::Completed || CurrentState == ETaskState::Failed)
    {
        return true;
    }

    UE_LOG(LogTemp, Log, TEXT("Task %s: Stopping task"), *TaskName);
    
    CleanupTask();
    SetTaskState(ETaskState::Cancelled);
    
    return true;
}

bool UBaseTask::PauseTask()
{
    if (CurrentState != ETaskState::Running)
    {
        return false;
    }

    if (!bCanBeInterrupted)
    {
        UE_LOG(LogTemp, Warning, TEXT("Task %s: Cannot pause - task is not interruptible"), *TaskName);
        return false;
    }

    SetTaskState(ETaskState::Paused);
    UE_LOG(LogTemp, Log, TEXT("Task %s: Paused"), *TaskName);
    return true;
}

bool UBaseTask::ResumeTask()
{
    if (CurrentState != ETaskState::Paused)
    {
        return false;
    }

    SetTaskState(ETaskState::Running);
    UE_LOG(LogTemp, Log, TEXT("Task %s: Resumed"), *TaskName);
    return true;
}

void UBaseTask::TickTask(float DeltaTime)
{
    if (CurrentState != ETaskState::Running)
    {
        return;
    }

    ExecutionTime += DeltaTime;

    // Check for timeout
    if (bHasTimeout && HasTimedOut())
    {
        UE_LOG(LogTemp, Warning, TEXT("Task %s: Timed out after %f seconds"), *TaskName, TimeoutDuration);
        CompleteTask(false, TEXT("Task timed out"));
        return;
    }

    // Update task logic
    UpdateTask(DeltaTime);

    // Validate task conditions
    if (!ValidateTaskConditions())
    {
        UE_LOG(LogTemp, Warning, TEXT("Task %s: Task conditions no longer valid"), *TaskName);
        CompleteTask(false, TEXT("Task conditions invalid"));
        return;
    }
}

bool UBaseTask::CanStartTask() const
{
    if (!OwnerPed)
    {
        return false;
    }

    if (bRequiresTarget && !TaskTarget)
    {
        return false;
    }

    return ValidateTaskConditions();
}

bool UBaseTask::CanInterruptTask() const
{
    return bCanBeInterrupted && (CurrentState == ETaskState::Running || CurrentState == ETaskState::Paused);
}

void UBaseTask::SetTaskState(ETaskState NewState)
{
    if (CurrentState == NewState)
    {
        return;
    }

    ETaskState OldState = CurrentState;
    CurrentState = NewState;

    UE_LOG(LogTemp, VeryVerbose, TEXT("Task %s: State changed from %d to %d"), *TaskName, (int32)OldState, (int32)NewState);

    // Broadcast state change
    OnTaskStateChanged.Broadcast(this, NewState);
}

void UBaseTask::CompleteTask(bool bSuccess, const FString& Message)
{
    FTaskResult Result;
    Result.bSuccess = bSuccess;
    Result.ResultMessage = Message;
    Result.ExecutionTime = ExecutionTime;
    Result.ResultData = LastResult.ResultData;

    LastResult = Result;

    // Cleanup task resources
    CleanupTask();

    // Set final state
    SetTaskState(bSuccess ? ETaskState::Completed : ETaskState::Failed);

    // Broadcast completion
    OnTaskCompleted.Broadcast(this, Result);

    UE_LOG(LogTemp, Log, TEXT("Task %s: Completed with result: %s (%s)"), 
           *TaskName, 
           bSuccess ? TEXT("Success") : TEXT("Failure"), 
           *Message);
}

UBaseTask* UBaseTask::CreateTask(TSubclassOf<UBaseTask> TaskClass, APed* Owner, AActor* Target)
{
    if (!TaskClass || !Owner)
    {
        UE_LOG(LogTemp, Error, TEXT("CreateTask: Invalid TaskClass or Owner"));
        return nullptr;
    }

    UBaseTask* NewTask = NewObject<UBaseTask>(Owner, TaskClass);
    if (NewTask)
    {
        NewTask->OwnerPed = Owner;
        NewTask->TaskTarget = Target;
        
        UE_LOG(LogTemp, Log, TEXT("Created task: %s for ped: %s"), 
               *NewTask->GetTaskName(), 
               *Owner->GetCharacterName());
    }

    return NewTask;
}

void UBaseTask::Initialize(APed* Owner, AActor* Target)
{
    if (!Owner)
    {
        UE_LOG(LogTemp, Error, TEXT("BaseTask::Initialize: Invalid Owner"));
        return;
    }

    OwnerPed = Owner;
    TaskTarget = Target;
    
    UE_LOG(LogTemp, Log, TEXT("Initialized task: %s for ped: %s"), 
           *GetTaskName(), 
           *Owner->GetCharacterName());
}

bool UBaseTask::HasTimedOut() const
{
    if (!bHasTimeout || TimeoutDuration <= 0.0f)
    {
        return false;
    }

    return ExecutionTime >= TimeoutDuration;
}

float UBaseTask::GetRemainingTime() const
{
    if (!bHasTimeout || TimeoutDuration <= 0.0f)
    {
        return -1.0f;
    }

    return FMath::Max(0.0f, TimeoutDuration - ExecutionTime);
}

float UBaseTask::GetProgress() const
{
    if (!bHasTimeout || TimeoutDuration <= 0.0f)
    {
        return 0.0f;
    }

    return FMath::Clamp(ExecutionTime / TimeoutDuration, 0.0f, 1.0f);
}

void UBaseTask::AddTaskData(const FString& Key, const FString& Value)
{
    LastResult.ResultData.Add(Key, Value);
}

FString UBaseTask::GetTaskData(const FString& Key) const
{
    const FString* FoundValue = LastResult.ResultData.Find(Key);
    return FoundValue ? *FoundValue : FString();
}
