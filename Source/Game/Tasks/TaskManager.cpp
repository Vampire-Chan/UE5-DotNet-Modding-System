#include "TaskManager.h"
#include "BaseTask.h"
#include "TaskFactory.h"
#include "Peds/OneShot/OneShotTask.h"
#include "Peds/Complex/ComplexTask.h"
#include "Peds/WildComplex/WildComplexTask.h"
#include "Engine/World.h"
#include "GameFramework/Actor.h"
#include "../Peds/Ped.h"

UTaskManager::UTaskManager()
{
    PrimaryComponentTick.bCanEverTick = true;
    PrimaryComponentTick.TickInterval = 0.1f; // Update every 100ms
    
    MaxConcurrentTasks = 1;
    bAllowTaskInterruption = true;
    bAutoStartTasks = true;
    TaskProcessingRate = 0.1f;
    bIsProcessingTasks = false;
    LastProcessingTime = 0.0f;
    CurrentTask = nullptr;
    OwnerPed = nullptr;
    bComponentInitialized = false;
}

void UTaskManager::BeginPlay()
{
    Super::BeginPlay();
    
    bComponentInitialized = true;
    OwnerPed = Cast<APed>(GetOwner());
}

void UTaskManager::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
    Super::TickComponent(DeltaTime, TickType, ThisTickFunction);
    
    ProcessTaskQueue();
}

bool UTaskManager::AddTask(UBaseTask* Task)
{
    if (!Task)
    {
        return false;
    }
    
    // Check if task should interrupt current task
    if (CurrentTask && bAllowTaskInterruption && ShouldInterruptForHigherPriority(Task))
    {
        CurrentTask->StopTask();
        CompletedTasks.Add(CurrentTask);
        CurrentTask = nullptr;
    }
    
    // Add to pending tasks queue
    PendingTasks.Add(Task);
    SortTasksByPriority();
    
    return true;
}

bool UTaskManager::RemoveTask(UBaseTask* Task)
{
    if (!Task)
    {
        return false;
    }
    
    bool bRemoved = false;
    
    // Remove from pending tasks
    bRemoved = PendingTasks.Remove(Task) > 0;
    
    // If it's the current task, stop it
    if (CurrentTask == Task)
    {
        CurrentTask->StopTask();
        CurrentTask = nullptr;
        bRemoved = true;
    }
    
    return bRemoved;
}

bool UTaskManager::StartTask(UBaseTask* Task)
{
    if (!Task || !CanStartTask(Task))
    {
        return false;
    }
    
    // If we have a current task with lower priority, interrupt it
    if (CurrentTask && bAllowTaskInterruption && ShouldInterruptForHigherPriority(Task))
    {
        CurrentTask->StopTask();
        CompletedTasks.Add(CurrentTask);
        CurrentTask = nullptr;
    }
    
    // If no current task, start this one immediately
    if (!CurrentTask)
    {
        CurrentTask = Task;
        PendingTasks.Remove(Task);
        return Task->StartTask();
    }
    
    // Otherwise add to queue
    return AddTask(Task);
}

bool UTaskManager::StopTask(UBaseTask* Task)
{
    if (!Task)
    {
        return false;
    }
    
    if (CurrentTask == Task)
    {
        bool bStopped = Task->StopTask();
        CompletedTasks.Add(CurrentTask);
        CurrentTask = nullptr;
        return bStopped;
    }
    
    // Remove from queues
    return RemoveTask(Task);
}

bool UTaskManager::InterruptCurrentTask()
{
    if (CurrentTask)
    {
        bool bStopped = CurrentTask->StopTask();
        if (bStopped)
        {
            CompletedTasks.Add(CurrentTask);
            CurrentTask = nullptr;
        }
        return bStopped;
    }
    
    return false;
}

void UTaskManager::ClearAllTasks()
{
    // Stop current task
    if (CurrentTask)
    {
        CurrentTask->StopTask();
        CompletedTasks.Add(CurrentTask);
        CurrentTask = nullptr;
    }
    
    // Move all pending tasks to failed
    for (UBaseTask* Task : PendingTasks)
    {
        if (Task)
        {
            FailedTasks.Add(Task);
        }
    }
    
    // Clear pending queue
    PendingTasks.Empty();
}

bool UTaskManager::HasActiveTasks() const
{
    return CurrentTask != nullptr || PendingTasks.Num() > 0;
}

int32 UTaskManager::GetTaskCount() const
{
    int32 Count = PendingTasks.Num();
    if (CurrentTask)
    {
        Count++;
    }
    return Count;
}

UBaseTask* UTaskManager::CreateAndAddTask(TSubclassOf<UBaseTask> TaskClass, AActor* Target)
{
    if (!TaskClass)
    {
        return nullptr;
    }
    
    UBaseTask* NewTask = NewObject<UBaseTask>(this, TaskClass);
    if (NewTask)
    {
        NewTask->Initialize(OwnerPed, Target);
        if (AddTask(NewTask))
        {
            return NewTask;
        }
    }
    
    return nullptr;
}

void UTaskManager::ProcessTaskQueue()
{
    if (!bIsProcessingTasks)
    {
        bIsProcessingTasks = true;
    }
    
    // If current task is finished, completed, or failed, clear it
    if (CurrentTask)
    {
        ETaskState CurrentState = CurrentTask->GetTaskState();
        if (CurrentState == ETaskState::Completed || 
            CurrentState == ETaskState::Failed || 
            CurrentState == ETaskState::Cancelled)
        {
            // Move to appropriate list
            if (CurrentState == ETaskState::Completed)
            {
                CompletedTasks.Add(CurrentTask);
            }
            else
            {
                FailedTasks.Add(CurrentTask);
            }
            CurrentTask = nullptr;
        }
        else
        {
            // Current task is still running
            bIsProcessingTasks = false;
            return;
        }
    }
    
    // Find next task to execute
    if (bAutoStartTasks && PendingTasks.Num() > 0)
    {
        UBaseTask* NextTask = PendingTasks[0];
        if (NextTask && CanStartTask(NextTask))
        {
            CurrentTask = NextTask;
            PendingTasks.RemoveAt(0);
            CurrentTask->StartTask();
        }
    }
    
    bIsProcessingTasks = false;
}

bool UTaskManager::CanStartTask(UBaseTask* Task) const
{
    if (!Task)
    {
        return false;
    }
    
    // Check if we have room for more tasks
    if (CurrentTask && MaxConcurrentTasks <= 1)
    {
        return false;
    }
    
    // Check if task can start
    return Task->CanStartTask();
}

void UTaskManager::OnTaskCompleted(UBaseTask* Task, const FTaskResult& Result)
{
    if (Task && Task == CurrentTask)
    {
        CompletedTasks.Add(Task);
        CurrentTask = nullptr;
    }
}

void UTaskManager::OnTaskStateChanged(UBaseTask* Task, ETaskState NewState)
{
    OnTaskManagerStateChanged.Broadcast(this, Task, NewState);
}

void UTaskManager::SortTasksByPriority()
{
    PendingTasks.Sort([](const UBaseTask& A, const UBaseTask& B) {
        return A.GetTaskPriority() > B.GetTaskPriority();
    });
}

bool UTaskManager::ShouldInterruptForHigherPriority(UBaseTask* NewTask) const
{
    if (!CurrentTask || !NewTask)
    {
        return false;
    }
    
    return NewTask->GetTaskPriority() > CurrentTask->GetTaskPriority();
}

// Helper methods for creating specific tasks
UTask_Aim* UTaskManager::AddAimTask(AActor* TargetActor)
{
    UTask_Aim* AimTask = NewObject<UTask_Aim>(this);
    if (AimTask)
    {
        AimTask->Initialize(OwnerPed, TargetActor);
        if (AddTask(AimTask))
        {
            return AimTask;
        }
    }
    
    return nullptr;
}

UTask_LookAt* UTaskManager::AddLookAtTask(AActor* TargetActor, FVector LookAtLocation)
{
    UTask_LookAt* LookAtTask = NewObject<UTask_LookAt>(this);
    if (LookAtTask)
    {
        LookAtTask->Initialize(OwnerPed, TargetActor);
        LookAtTask->SetLookDirection(LookAtLocation);
        if (AddTask(LookAtTask))
        {
            return LookAtTask;
        }
    }
    
    return nullptr;
}

UTask_Turn* UTaskManager::AddTurnTask(FRotator TargetRotation, AActor* TargetActor)
{
    UTask_Turn* TurnTask = NewObject<UTask_Turn>(this);
    if (TurnTask)
    {
        TurnTask->Initialize(OwnerPed, TargetActor);
        TurnTask->SetTargetRotation(TargetRotation);
        if (AddTask(TurnTask))
        {
            return TurnTask;
        }
    }
    
    return nullptr;
}

UTask_Shimmy* UTaskManager::AddShimmyTask(FVector ShimmyDirection, float ShimmyDistance)
{
    UTask_Shimmy* ShimmyTask = NewObject<UTask_Shimmy>(this);
    if (ShimmyTask)
    {
        ShimmyTask->Initialize(OwnerPed);
        ShimmyTask->SetShimmyDirection(ShimmyDirection);
        ShimmyTask->SetShimmyDistance(ShimmyDistance);
        if (AddTask(ShimmyTask))
        {
            return ShimmyTask;
        }
    }
    
    return nullptr;
}

UTask_DropDown* UTaskManager::AddDropDownTask(float DropHeight)
{
    UTask_DropDown* DropDownTask = NewObject<UTask_DropDown>(this);
    if (DropDownTask)
    {
        DropDownTask->Initialize(OwnerPed);
        DropDownTask->SetDropHeight(DropHeight);
        if (AddTask(DropDownTask))
        {
            return DropDownTask;
        }
    }
    
    return nullptr;
}

UTask_Climb* UTaskManager::AddClimbTask(FVector ClimbTarget, float ClimbHeight)
{
    UTask_Climb* ClimbTask = NewObject<UTask_Climb>(this);
    if (ClimbTask)
    {
        ClimbTask->Initialize(OwnerPed);
        ClimbTask->SetClimbTarget(ClimbTarget);
        ClimbTask->SetClimbHeight(ClimbHeight);
        if (AddTask(ClimbTask))
        {
            return ClimbTask;
        }
    }
    
    return nullptr;
}

UTask_EnterVehicle* UTaskManager::AddEnterVehicleTask(AActor* Vehicle, int32 SeatIndex)
{
    UTask_EnterVehicle* EnterVehicleTask = NewObject<UTask_EnterVehicle>(this);
    if (EnterVehicleTask)
    {
        EnterVehicleTask->Initialize(OwnerPed, Vehicle);
        EnterVehicleTask->SetSeatIndex(SeatIndex);
        if (AddTask(EnterVehicleTask))
        {
            return EnterVehicleTask;
        }
    }
    
    return nullptr;
}

UTask_GrabLedgeAndHold* UTaskManager::AddGrabLedgeTask(FVector LedgeLocation, float HoldDuration)
{
    UTask_GrabLedgeAndHold* GrabLedgeTask = NewObject<UTask_GrabLedgeAndHold>(this);
    if (GrabLedgeTask)
    {
        GrabLedgeTask->Initialize(OwnerPed);
        GrabLedgeTask->SetLedgeLocation(LedgeLocation);
        GrabLedgeTask->SetHoldDuration(HoldDuration);
        if (AddTask(GrabLedgeTask))
        {
            return GrabLedgeTask;
        }
    }
    
    return nullptr;
}

UTask_ClimbLadder* UTaskManager::AddClimbLadderTask(AActor* Ladder, bool bClimbUp)
{
    UTask_ClimbLadder* ClimbLadderTask = NewObject<UTask_ClimbLadder>(this);
    if (ClimbLadderTask)
    {
        ClimbLadderTask->Initialize(OwnerPed, Ladder);
        ClimbLadderTask->SetClimbDirection(bClimbUp);
        if (AddTask(ClimbLadderTask))
        {
            return ClimbLadderTask;
        }
    }
    
    return nullptr;
}

UTask_FightAgainst* UTaskManager::AddFightTask(AActor* Target)
{
    UTask_FightAgainst* FightTask = NewObject<UTask_FightAgainst>(this);
    if (FightTask)
    {
        FightTask->Initialize(OwnerPed, Target);
        if (AddTask(FightTask))
        {
            return FightTask;
        }
    }
    
    return nullptr;
}

UTask_CombatTargets* UTaskManager::AddCombatTargetsTask(const TArray<AActor*>& Targets)
{
    UTask_CombatTargets* CombatTask = NewObject<UTask_CombatTargets>(this);
    if (CombatTask)
    {
        CombatTask->Initialize(OwnerPed);
        
        // Convert AActor* array to APed* array
        TArray<APed*> PedTargets;
        for (AActor* Actor : Targets)
        {
            if (APed* PedActor = Cast<APed>(Actor))
            {
                PedTargets.Add(PedActor);
            }
        }
        
        CombatTask->SetTargets(PedTargets);
        if (AddTask(CombatTask))
        {
            return CombatTask;
        }
    }
    
    return nullptr;
}