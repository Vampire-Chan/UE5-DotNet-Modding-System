#include "TaskFactory.h"
#include "Peds/Ped.h"
#include "Engine/World.h"

// Initialize static variables
TMap<FString, TSubclassOf<UBaseTask>> UTaskFactory::CachedTaskClasses;

UTaskFactory::UTaskFactory()
{
    // Initialize cached task classes on first use
    if (CachedTaskClasses.Num() == 0)
    {
        InitializeCachedTaskClasses();
    }
}

// === OneShot Task Creation ===

UTask_Aim* UTaskFactory::CreateAimTask(APed* OwnerPed, AActor* Target, float AimDuration)
{
    if (!OwnerPed || !Target)
    {
        UE_LOG(LogTemp, Error, TEXT("CreateAimTask: Invalid OwnerPed or Target"));
        return nullptr;
    }

    UTask_Aim* AimTask = NewObject<UTask_Aim>(OwnerPed);
    if (AimTask)
    {
        SetCommonTaskProperties(AimTask, OwnerPed, Target);
        AimTask->SetAimDuration(AimDuration);
        
        UE_LOG(LogTemp, Log, TEXT("Created Aim task for ped: %s targeting: %s"), 
               *OwnerPed->GetCharacterName(), *Target->GetName());
    }
    
    return AimTask;
}

UTask_LookAt* UTaskFactory::CreateLookAtTask(APed* OwnerPed, AActor* Target, float LookDuration)
{
    if (!OwnerPed || !Target)
    {
        UE_LOG(LogTemp, Error, TEXT("CreateLookAtTask: Invalid OwnerPed or Target"));
        return nullptr;
    }

    UTask_LookAt* LookAtTask = NewObject<UTask_LookAt>(OwnerPed);
    if (LookAtTask)
    {
        SetCommonTaskProperties(LookAtTask, OwnerPed, Target);
        LookAtTask->SetLookDuration(LookDuration);
        
        UE_LOG(LogTemp, Log, TEXT("Created LookAt task for ped: %s targeting: %s"), 
               *OwnerPed->GetCharacterName(), *Target->GetName());
    }
    
    return LookAtTask;
}

UTask_Turn* UTaskFactory::CreateTurnTask(APed* OwnerPed, FVector TargetDirection, float TurnSpeed)
{
    if (!OwnerPed)
    {
        UE_LOG(LogTemp, Error, TEXT("CreateTurnTask: Invalid OwnerPed"));
        return nullptr;
    }

    UTask_Turn* TurnTask = NewObject<UTask_Turn>(OwnerPed);
    if (TurnTask)
    {
        SetCommonTaskProperties(TurnTask, OwnerPed, nullptr);
        TurnTask->SetTargetDirection(TargetDirection);
        TurnTask->SetTurnSpeed(TurnSpeed);
        
        UE_LOG(LogTemp, Log, TEXT("Created Turn task for ped: %s"), 
               *OwnerPed->GetCharacterName());
    }
    
    return TurnTask;
}

UTask_Shimmy* UTaskFactory::CreateShimmyTask(APed* OwnerPed, FVector ShimmyDirection, float ShimmyDistance)
{
    if (!OwnerPed)
    {
        UE_LOG(LogTemp, Error, TEXT("CreateShimmyTask: Invalid OwnerPed"));
        return nullptr;
    }

    UTask_Shimmy* ShimmyTask = NewObject<UTask_Shimmy>(OwnerPed);
    if (ShimmyTask)
    {
        SetCommonTaskProperties(ShimmyTask, OwnerPed, nullptr);
        ShimmyTask->SetShimmyDirection(ShimmyDirection);
        ShimmyTask->SetShimmyDistance(ShimmyDistance);
        
        UE_LOG(LogTemp, Log, TEXT("Created Shimmy task for ped: %s"), 
               *OwnerPed->GetCharacterName());
    }
    
    return ShimmyTask;
}

UTask_DropDown* UTaskFactory::CreateDropDownTask(APed* OwnerPed, AActor* LedgeActor)
{
    if (!OwnerPed)
    {
        UE_LOG(LogTemp, Error, TEXT("CreateDropDownTask: Invalid OwnerPed"));
        return nullptr;
    }

    UTask_DropDown* DropDownTask = NewObject<UTask_DropDown>(OwnerPed);
    if (DropDownTask)
    {
        SetCommonTaskProperties(DropDownTask, OwnerPed, LedgeActor);
        
        UE_LOG(LogTemp, Log, TEXT("Created DropDown task for ped: %s"), 
               *OwnerPed->GetCharacterName());
    }
    
    return DropDownTask;
}

UTask_Jump* UTaskFactory::CreateJumpTask(APed* OwnerPed, const FVector& JumpTarget, float JumpHeight)
{
    if (!OwnerPed)
    {
        UE_LOG(LogTemp, Error, TEXT("CreateJumpTask: Invalid OwnerPed"));
        return nullptr;
    }

    UTask_Jump* JumpTask = NewObject<UTask_Jump>(OwnerPed);
    if (JumpTask)
    {
        SetCommonTaskProperties(JumpTask, OwnerPed, nullptr);
        JumpTask->SetJumpTarget(JumpTarget);
        JumpTask->SetJumpHeight(JumpHeight);
        
        UE_LOG(LogTemp, Log, TEXT("Created Jump task for ped: %s to target: %s"), 
               *OwnerPed->GetCharacterName(), *JumpTarget.ToString());
    }
    
    return JumpTask;
}

UTask_MoveTowards* UTaskFactory::CreateMoveTowardsTask(APed* OwnerPed, const FVector& TargetLocation, float MovementSpeed)
{
    if (!OwnerPed)
    {
        UE_LOG(LogTemp, Error, TEXT("CreateMoveTowardsTask: Invalid OwnerPed"));
        return nullptr;
    }

    UTask_MoveTowards* MoveTask = NewObject<UTask_MoveTowards>(OwnerPed);
    if (MoveTask)
    {
        SetCommonTaskProperties(MoveTask, OwnerPed, nullptr);
        MoveTask->SetTargetLocation(TargetLocation);
        MoveTask->SetMovementSpeed(MovementSpeed);
        
        UE_LOG(LogTemp, Log, TEXT("Created MoveTowards task for ped: %s to location: %s"), 
               *OwnerPed->GetCharacterName(), *TargetLocation.ToString());
    }
    
    return MoveTask;
}

UTask_MoveTowards* UTaskFactory::CreateMoveTowardsActorTask(APed* OwnerPed, AActor* TargetActor, float MovementSpeed)
{
    if (!OwnerPed || !TargetActor)
    {
        UE_LOG(LogTemp, Error, TEXT("CreateMoveTowardsActorTask: Invalid OwnerPed or TargetActor"));
        return nullptr;
    }

    UTask_MoveTowards* MoveTask = NewObject<UTask_MoveTowards>(OwnerPed);
    if (MoveTask)
    {
        SetCommonTaskProperties(MoveTask, OwnerPed, TargetActor);
        MoveTask->SetTargetActor(TargetActor);
        MoveTask->SetMovementSpeed(MovementSpeed);
        
        UE_LOG(LogTemp, Log, TEXT("Created MoveTowardsActor task for ped: %s targeting: %s"), 
               *OwnerPed->GetCharacterName(), *TargetActor->GetName());
    }
    
    return MoveTask;
}

// === Complex Task Creation ===

UTask_Climb* UTaskFactory::CreateClimbTask(APed* OwnerPed, AActor* ClimbTarget, float ClimbHeight)
{
    if (!OwnerPed || !ClimbTarget)
    {
        UE_LOG(LogTemp, Error, TEXT("CreateClimbTask: Invalid OwnerPed or ClimbTarget"));
        return nullptr;
    }

    UTask_Climb* ClimbTask = NewObject<UTask_Climb>(OwnerPed);
    if (ClimbTask)
    {
        SetCommonTaskProperties(ClimbTask, OwnerPed, ClimbTarget);
        ClimbTask->SetClimbHeight(ClimbHeight);
        
        UE_LOG(LogTemp, Log, TEXT("Created Climb task for ped: %s targeting: %s"), 
               *OwnerPed->GetCharacterName(), *ClimbTarget->GetName());
    }
    
    return ClimbTask;
}

UTask_EnterVehicle* UTaskFactory::CreateEnterVehicleTask(APed* OwnerPed, AActor* Vehicle, int32 SeatIndex)
{
    if (!OwnerPed || !Vehicle)
    {
        UE_LOG(LogTemp, Error, TEXT("CreateEnterVehicleTask: Invalid OwnerPed or Vehicle"));
        return nullptr;
    }

    UTask_EnterVehicle* EnterVehicleTask = NewObject<UTask_EnterVehicle>(OwnerPed);
    if (EnterVehicleTask)
    {
        SetCommonTaskProperties(EnterVehicleTask, OwnerPed, Vehicle);
        EnterVehicleTask->SetSeatIndex(SeatIndex);
        
        UE_LOG(LogTemp, Log, TEXT("Created EnterVehicle task for ped: %s targeting vehicle: %s"), 
               *OwnerPed->GetCharacterName(), *Vehicle->GetName());
    }
    
    return EnterVehicleTask;
}

UTask_GrabLedgeAndHold* UTaskFactory::CreateGrabLedgeTask(APed* OwnerPed, AActor* LedgeActor, float HoldDuration)
{
    if (!OwnerPed || !LedgeActor)
    {
        UE_LOG(LogTemp, Error, TEXT("CreateGrabLedgeTask: Invalid OwnerPed or LedgeActor"));
        return nullptr;
    }

    UTask_GrabLedgeAndHold* GrabLedgeTask = NewObject<UTask_GrabLedgeAndHold>(OwnerPed);
    if (GrabLedgeTask)
    {
        SetCommonTaskProperties(GrabLedgeTask, OwnerPed, LedgeActor);
        GrabLedgeTask->SetHoldDuration(HoldDuration);
        
        UE_LOG(LogTemp, Log, TEXT("Created GrabLedge task for ped: %s targeting: %s"), 
               *OwnerPed->GetCharacterName(), *LedgeActor->GetName());
    }
    
    return GrabLedgeTask;
}

UTask_ClimbLadder* UTaskFactory::CreateClimbLadderTask(APed* OwnerPed, AActor* LadderActor, bool bClimbUp)
{
    if (!OwnerPed || !LadderActor)
    {
        UE_LOG(LogTemp, Error, TEXT("CreateClimbLadderTask: Invalid OwnerPed or LadderActor"));
        return nullptr;
    }

    UTask_ClimbLadder* ClimbLadderTask = NewObject<UTask_ClimbLadder>(OwnerPed);
    if (ClimbLadderTask)
    {
        SetCommonTaskProperties(ClimbLadderTask, OwnerPed, LadderActor);
        ClimbLadderTask->SetClimbDirection(bClimbUp);
        
        UE_LOG(LogTemp, Log, TEXT("Created ClimbLadder task for ped: %s targeting: %s"), 
               *OwnerPed->GetCharacterName(), *LadderActor->GetName());
    }
    
    return ClimbLadderTask;
}

// === WildComplex Task Creation ===

UTask_FightAgainst* UTaskFactory::CreateFightAgainstTask(APed* OwnerPed, APed* Enemy, float FightDuration)
{
    if (!OwnerPed || !Enemy)
    {
        UE_LOG(LogTemp, Error, TEXT("CreateFightAgainstTask: Invalid OwnerPed or Enemy"));
        return nullptr;
    }

    UTask_FightAgainst* FightTask = NewObject<UTask_FightAgainst>(OwnerPed);
    if (FightTask)
    {
        SetCommonTaskProperties(FightTask, OwnerPed, Enemy);
        FightTask->SetFightDuration(FightDuration);
        
        UE_LOG(LogTemp, Log, TEXT("Created FightAgainst task for ped: %s targeting: %s"), 
               *OwnerPed->GetCharacterName(), *Enemy->GetCharacterName());
    }
    
    return FightTask;
}

UTask_CombatTargets* UTaskFactory::CreateCombatTargetsTask(APed* OwnerPed, const TArray<APed*>& Targets, float CombatDuration)
{
    if (!OwnerPed || Targets.Num() == 0)
    {
        UE_LOG(LogTemp, Error, TEXT("CreateCombatTargetsTask: Invalid OwnerPed or empty Targets array"));
        return nullptr;
    }

    UTask_CombatTargets* CombatTask = NewObject<UTask_CombatTargets>(OwnerPed);
    if (CombatTask)
    {
        SetCommonTaskProperties(CombatTask, OwnerPed, Targets[0]); // Set first target as primary
        CombatTask->SetTargets(Targets);
        CombatTask->SetCombatDuration(CombatDuration);
        
        UE_LOG(LogTemp, Log, TEXT("Created CombatTargets task for ped: %s with %d targets"), 
               *OwnerPed->GetCharacterName(), Targets.Num());
    }
    
    return CombatTask;
}

// === Utility Functions ===

UBaseTask* UTaskFactory::CreateTaskOfClass(TSubclassOf<UBaseTask> TaskClass, APed* OwnerPed, AActor* Target)
{
    if (!TaskClass || !OwnerPed)
    {
        UE_LOG(LogTemp, Error, TEXT("CreateTaskOfClass: Invalid TaskClass or OwnerPed"));
        return nullptr;
    }

    UBaseTask* NewTask = NewObject<UBaseTask>(OwnerPed, TaskClass);
    if (NewTask)
    {
        SetCommonTaskProperties(NewTask, OwnerPed, Target);
        
        UE_LOG(LogTemp, Log, TEXT("Created task of class: %s for ped: %s"), 
               *TaskClass->GetName(), *OwnerPed->GetCharacterName());
    }
    
    return NewTask;
}

bool UTaskFactory::CanPedExecuteTask(APed* OwnerPed, TSubclassOf<UBaseTask> TaskClass, AActor* Target)
{
    if (!OwnerPed || !TaskClass)
    {
        return false;
    }

    // Basic validation - ped should be valid and active
    if (!IsValid(OwnerPed))
    {
        return false;
    }

    // Check if task requires target
    UBaseTask* DefaultTask = TaskClass->GetDefaultObject<UBaseTask>();
    if (DefaultTask && DefaultTask->RequiresTarget() && !Target)
    {
        return false;
    }

    // Additional task-specific validation can be added here
    return true;
}

TArray<TSubclassOf<UBaseTask>> UTaskFactory::GetAvailableTaskClasses(APed* OwnerPed)
{
    TArray<TSubclassOf<UBaseTask>> AvailableClasses;
    
    if (!OwnerPed)
    {
        return AvailableClasses;
    }

    // Add task classes based on ped's current state and capabilities
    for (auto& ClassPair : CachedTaskClasses)
    {
        if (CanPedExecuteTask(OwnerPed, ClassPair.Value, nullptr))
        {
            AvailableClasses.Add(ClassPair.Value);
        }
    }
    
    return AvailableClasses;
}

TArray<UBaseTask*> UTaskFactory::CreateTaskSequence(APed* OwnerPed, const TArray<TSubclassOf<UBaseTask>>& TaskClasses, const TArray<AActor*>& Targets)
{
    TArray<UBaseTask*> TaskSequence;
    
    if (!OwnerPed || TaskClasses.Num() == 0)
    {
        return TaskSequence;
    }

    for (int32 i = 0; i < TaskClasses.Num(); i++)
    {
        AActor* Target = (i < Targets.Num()) ? Targets[i] : nullptr;
        UBaseTask* NewTask = CreateTaskOfClass(TaskClasses[i], OwnerPed, Target);
        
        if (NewTask)
        {
            TaskSequence.Add(NewTask);
        }
    }
    
    UE_LOG(LogTemp, Log, TEXT("Created task sequence with %d tasks for ped: %s"), 
           TaskSequence.Num(), *OwnerPed->GetCharacterName());
    
    return TaskSequence;
}

// === Batch Task Creation ===

TArray<UTask_Aim*> UTaskFactory::CreateMultiPedAimTasks(const TArray<APed*>& Peds, AActor* Target, float AimDuration)
{
    TArray<UTask_Aim*> AimTasks;
    
    if (!Target || Peds.Num() == 0)
    {
        return AimTasks;
    }

    for (APed* Ped : Peds)
    {
        if (UTask_Aim* AimTask = CreateAimTask(Ped, Target, AimDuration))
        {
            AimTasks.Add(AimTask);
        }
    }
    
    UE_LOG(LogTemp, Log, TEXT("Created %d aim tasks targeting: %s"), 
           AimTasks.Num(), *Target->GetName());
    
    return AimTasks;
}

TArray<UTask_CombatTargets*> UTaskFactory::CreateMultiPedCombatTasks(const TArray<APed*>& Peds, const TArray<APed*>& Targets, float CombatDuration)
{
    TArray<UTask_CombatTargets*> CombatTasks;
    
    if (Peds.Num() == 0 || Targets.Num() == 0)
    {
        return CombatTasks;
    }

    for (APed* Ped : Peds)
    {
        if (UTask_CombatTargets* CombatTask = CreateCombatTargetsTask(Ped, Targets, CombatDuration))
        {
            CombatTasks.Add(CombatTask);
        }
    }
    
    UE_LOG(LogTemp, Log, TEXT("Created %d combat tasks for %d peds against %d targets"), 
           CombatTasks.Num(), Peds.Num(), Targets.Num());
    
    return CombatTasks;
}

// === Task Creation with Validation ===

UBaseTask* UTaskFactory::CreateValidatedTask(TSubclassOf<UBaseTask> TaskClass, APed* OwnerPed, AActor* Target, bool& bOutSuccess, FString& OutErrorMessage)
{
    bOutSuccess = false;
    OutErrorMessage = TEXT("");

    if (!ValidateTaskCreationParams(OwnerPed, TaskClass, OutErrorMessage))
    {
        return nullptr;
    }

    if (!CanPedExecuteTask(OwnerPed, TaskClass, Target))
    {
        OutErrorMessage = TEXT("Ped cannot execute this task type in current state");
        return nullptr;
    }

    UBaseTask* NewTask = CreateTaskOfClass(TaskClass, OwnerPed, Target);
    if (NewTask)
    {
        bOutSuccess = true;
        OutErrorMessage = TEXT("Task created successfully");
    }
    else
    {
        OutErrorMessage = TEXT("Failed to create task object");
    }

    return NewTask;
}

// === Protected Helper Functions ===

bool UTaskFactory::ValidateTaskCreationParams(APed* OwnerPed, TSubclassOf<UBaseTask> TaskClass, FString& OutErrorMessage)
{
    if (!OwnerPed)
    {
        OutErrorMessage = TEXT("Invalid OwnerPed - cannot be null");
        return false;
    }

    if (!TaskClass)
    {
        OutErrorMessage = TEXT("Invalid TaskClass - cannot be null");
        return false;
    }

    if (!IsValid(OwnerPed))
    {
        OutErrorMessage = TEXT("OwnerPed is not a valid UObject");
        return false;
    }

    return true;
}

void UTaskFactory::SetCommonTaskProperties(UBaseTask* Task, APed* OwnerPed, AActor* Target)
{
    if (!Task)
    {
        return;
    }

    Task->Initialize(OwnerPed, Target);
    Task->SetPriority(ETaskPriority::Normal);
    Task->SetTimeout(30.0f); // Default 30 second timeout
}

void UTaskFactory::ConfigureTaskSpecificProperties(UBaseTask* Task, const TMap<FString, FString>& Properties)
{
    if (!Task || Properties.Num() == 0)
    {
        return;
    }

    // This can be extended to set task-specific properties based on the Properties map
    for (const auto& PropertyPair : Properties)
    {
        UE_LOG(LogTemp, Log, TEXT("Setting property %s = %s for task %s"), 
               *PropertyPair.Key, *PropertyPair.Value, *Task->GetTaskName());
    }
}

// === Private Helper Functions ===

void UTaskFactory::InitializeCachedTaskClasses()
{
    // Cache commonly used task classes for performance
    CachedTaskClasses.Add(TEXT("Aim"), UTask_Aim::StaticClass());
    CachedTaskClasses.Add(TEXT("LookAt"), UTask_LookAt::StaticClass());
    CachedTaskClasses.Add(TEXT("Turn"), UTask_Turn::StaticClass());
    CachedTaskClasses.Add(TEXT("Shimmy"), UTask_Shimmy::StaticClass());
    CachedTaskClasses.Add(TEXT("DropDown"), UTask_DropDown::StaticClass());
    
    CachedTaskClasses.Add(TEXT("Climb"), UTask_Climb::StaticClass());
    CachedTaskClasses.Add(TEXT("EnterVehicle"), UTask_EnterVehicle::StaticClass());
    CachedTaskClasses.Add(TEXT("GrabLedge"), UTask_GrabLedgeAndHold::StaticClass());
    CachedTaskClasses.Add(TEXT("ClimbLadder"), UTask_ClimbLadder::StaticClass());
    
    CachedTaskClasses.Add(TEXT("FightAgainst"), UTask_FightAgainst::StaticClass());
    CachedTaskClasses.Add(TEXT("CombatTargets"), UTask_CombatTargets::StaticClass());

    UE_LOG(LogTemp, Log, TEXT("TaskFactory: Cached %d task classes"), CachedTaskClasses.Num());
}

TSubclassOf<UBaseTask> UTaskFactory::GetCachedTaskClass(const FString& TaskName)
{
    if (CachedTaskClasses.Contains(TaskName))
    {
        return CachedTaskClasses[TaskName];
    }
    
    UE_LOG(LogTemp, Warning, TEXT("TaskFactory: Task class '%s' not found in cache"), *TaskName);
    return nullptr;
}
