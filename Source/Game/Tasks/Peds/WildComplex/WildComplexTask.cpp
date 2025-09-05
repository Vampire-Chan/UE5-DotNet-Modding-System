#include "WildComplexTask.h"
#include "../../../Peds/Ped.h"
#include "../../../Core/Utils/RaycastUtils.h"
#include "Engine/World.h"
#include "Components/SkeletalMeshComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Kismet/KismetMathLibrary.h"

// =====================================================
// UWildComplexTask Implementation
// =====================================================

UWildComplexTask::UWildComplexTask()
{
    TaskType = ETaskType::WildComplex;
    TaskName = TEXT("WildComplexTask");
    TimeoutDuration = 120.0f; // Longer timeout for complex tasks
    AnalysisUpdateRate = 0.1f; // 10 times per second
    PlanningUpdateRate = 0.5f; // 2 times per second
    AdaptationThreshold = 0.3f;
    bEnableDynamicAdaptation = true;
    CurrentWildState = EWildComplexTaskState::Initializing;
    StateTime = 0.0f;
    AdaptationCount = 0;
    LastAnalysisTime = 0.0f;
    LastPlanningTime = 0.0f;
    bSystemsInitialized = false;
}

bool UWildComplexTask::ExecuteTask()
{
    SetWildComplexState(EWildComplexTaskState::Initializing);
    return InitializeComplexSystems();
}

void UWildComplexTask::UpdateTask(float DeltaTime)
{
    Super::UpdateTask(DeltaTime);

    StateTime += DeltaTime;

    // Execute current state logic
    bool bStateSuccess = false;
    
    switch (CurrentWildState)
    {
        case EWildComplexTaskState::Initializing:
            if (bSystemsInitialized)
            {
                SetWildComplexState(EWildComplexTaskState::Analyzing);
            }
            bStateSuccess = true;
            break;

        case EWildComplexTaskState::Analyzing:
            if (ExecutionTime - LastAnalysisTime >= AnalysisUpdateRate)
            {
                bStateSuccess = AnalyzeSituation(DeltaTime);
                LastAnalysisTime = ExecutionTime;
            }
            else
            {
                bStateSuccess = true;
            }
            
            if (StateTime >= 1.0f) // Move to planning after 1 second of analysis
            {
                SetWildComplexState(EWildComplexTaskState::Planning);
            }
            break;

        case EWildComplexTaskState::Planning:
            if (ExecutionTime - LastPlanningTime >= PlanningUpdateRate)
            {
                bStateSuccess = PlanActions(DeltaTime);
                LastPlanningTime = ExecutionTime;
            }
            else
            {
                bStateSuccess = true;
            }
            
            if (StateTime >= 2.0f) // Move to execution after 2 seconds of planning
            {
                SetWildComplexState(EWildComplexTaskState::Executing);
            }
            break;

        case EWildComplexTaskState::Executing:
            bStateSuccess = ExecuteComplexActions(DeltaTime);
            
            // Continuous analysis during execution
            if (ExecutionTime - LastAnalysisTime >= AnalysisUpdateRate)
            {
                AnalyzeSituation(DeltaTime);
                LastAnalysisTime = ExecutionTime;
            }
            
            // Check if adaptation is needed
            if (bEnableDynamicAdaptation && ShouldTransitionState())
            {
                SetWildComplexState(EWildComplexTaskState::Adapting);
            }
            break;

        case EWildComplexTaskState::Adapting:
            bStateSuccess = AdaptToChanges(DeltaTime);
            AdaptationCount++;
            
            if (StateTime >= 1.0f) // Return to execution after adaptation
            {
                SetWildComplexState(EWildComplexTaskState::Executing);
            }
            break;

        case EWildComplexTaskState::Finalizing:
            bStateSuccess = FinalizeExecution();
            if (bStateSuccess)
            {
                CompleteTask(true, TEXT("WildComplex task completed successfully"));
            }
            break;
    }

    if (!bStateSuccess)
    {
        CompleteTask(false, FString::Printf(TEXT("Failed in state: %d"), (int32)CurrentWildState));
    }
}

void UWildComplexTask::CleanupTask()
{
    Super::CleanupTask();
    SetWildComplexState(EWildComplexTaskState::Finalizing);
}

void UWildComplexTask::SetWildComplexState(EWildComplexTaskState NewState)
{
    if (CurrentWildState != NewState)
    {
        EWildComplexTaskState OldState = CurrentWildState;
        CurrentWildState = NewState;
        StateTime = 0.0f;

        UE_LOG(LogTemp, Log, TEXT("WildComplexTask %s: State changed from %d to %d (Adaptations: %d)"), 
               *TaskName, (int32)OldState, (int32)NewState, AdaptationCount);
    }
}

bool UWildComplexTask::ShouldTransitionState() const
{
    // Implement adaptive logic here
    // For now, simple threshold-based adaptation
    return AdaptationCount < 5 && StateTime > 10.0f; // Adapt if state has been running for too long
}

// =====================================================
// UTask_FightAgainst Implementation
// =====================================================

UTask_FightAgainst::UTask_FightAgainst()
{
    TaskName = TEXT("FightAgainst");
    TaskDescription = TEXT("Engage in combat with another character");
    bRequiresTarget = true;
    TimeoutDuration = 60.0f;
    FightingStyle = EFightingStyle::Mixed;
    AggressionLevel = 0.7f;
    SkillLevel = 0.5f;
    StaminaConsumptionRate = 15.0f;
    bUseEnvironmentalObjects = true;
    
    // Initialize combat state
    OpponentDistance = 0.0f;
    OpponentHealth = 100.0f;
    CurrentStamina = 100.0f;
    CombatIntensity = 0.0f;
    ActionCooldown = 0.0f;
    
    // Initialize analysis data
    bOpponentIsAggressive = false;
    bOpponentIsBlocking = false;
    bOpponentIsTired = false;
    OpponentSpeed = 0.0f;
}

bool UTask_FightAgainst::InitializeComplexSystems()
{
    if (!OwnerPed)
    {
        return false;
    }

    Opponent = Cast<APed>(TaskTarget);
    if (!Opponent)
    {
        UE_LOG(LogTemp, Error, TEXT("Task_FightAgainst: Target is not a Ped"));
        return false;
    }

    // Initialize available actions based on fighting style
    AvailableAttacks.Empty();
    AvailableDefenses.Empty();

    switch (FightingStyle)
    {
        case EFightingStyle::Boxing:
            AvailableAttacks.AddUnique(TEXT("Jab"));
            AvailableAttacks.AddUnique(TEXT("Cross"));
            AvailableAttacks.AddUnique(TEXT("Hook"));
            AvailableDefenses.AddUnique(TEXT("Block"));
            AvailableDefenses.AddUnique(TEXT("Weave"));
            break;

        case EFightingStyle::MartialArts:
            AvailableAttacks.AddUnique(TEXT("Punch"));
            AvailableAttacks.AddUnique(TEXT("Kick"));
            AvailableAttacks.AddUnique(TEXT("Knee"));
            AvailableDefenses.AddUnique(TEXT("Block"));
            AvailableDefenses.AddUnique(TEXT("Parry"));
            AvailableDefenses.AddUnique(TEXT("Dodge"));
            break;

        case EFightingStyle::Mixed:
        default:
            AvailableAttacks.AddUnique(TEXT("Punch"));
            AvailableAttacks.AddUnique(TEXT("Kick"));
            AvailableAttacks.AddUnique(TEXT("Grapple"));
            AvailableDefenses.AddUnique(TEXT("Block"));
            AvailableDefenses.AddUnique(TEXT("Dodge"));
            break;
    }

    LastKnownOpponentLocation = Opponent->GetActorLocation();
    bSystemsInitialized = true;

    UE_LOG(LogTemp, Log, TEXT("Task_FightAgainst: Initialized combat against %s"), *Opponent->GetCharacterName());
    return true;
}

bool UTask_FightAgainst::AnalyzeSituation(float DeltaTime)
{
    if (!Opponent || !OwnerPed)
    {
        return false;
    }

    UpdateCombatAnalysis();
    
    // Update combat intensity based on various factors
    float DistanceFactor = FMath::Clamp(1.0f - (OpponentDistance / 300.0f), 0.0f, 1.0f);
    float HealthFactor = OpponentHealth / 100.0f;
    float StaminaFactor = CurrentStamina / 100.0f;
    
    CombatIntensity = (DistanceFactor + (1.0f - HealthFactor) + StaminaFactor) / 3.0f;

    return true;
}

bool UTask_FightAgainst::PlanActions(float DeltaTime)
{
    if (!Opponent || !OwnerPed)
    {
        return false;
    }

    SelectBestAction();
    return true;
}

bool UTask_FightAgainst::ExecuteComplexActions(float DeltaTime)
{
    if (!Opponent || !OwnerPed)
    {
        return false;
    }

    // Update cooldowns
    if (ActionCooldown > 0.0f)
    {
        ActionCooldown -= DeltaTime;
    }

    // Drain stamina
    CurrentStamina -= StaminaConsumptionRate * DeltaTime * CombatIntensity;
    CurrentStamina = FMath::Clamp(CurrentStamina, 0.0f, 100.0f);

    // Execute current action
    ExecuteCurrentAction(DeltaTime);

    // Check for combat end conditions
    if (CheckForCombatEnd())
    {
        SetWildComplexState(EWildComplexTaskState::Finalizing);
        return true;
    }

    return true;
}

bool UTask_FightAgainst::AdaptToChanges(float DeltaTime)
{
    if (!Opponent || !OwnerPed)
    {
        return false;
    }

    // Adapt fighting style based on opponent behavior
    if (bOpponentIsAggressive && FightingStyle != EFightingStyle::Boxing)
    {
        // Switch to more defensive style
        AggressionLevel = FMath::Max(AggressionLevel - 0.2f, 0.1f);
        UE_LOG(LogTemp, Log, TEXT("Task_FightAgainst: Adapting to aggressive opponent - reducing aggression"));
    }
    else if (bOpponentIsTired && AggressionLevel < 0.8f)
    {
        // Increase aggression against tired opponent
        AggressionLevel = FMath::Min(AggressionLevel + 0.3f, 1.0f);
        UE_LOG(LogTemp, Log, TEXT("Task_FightAgainst: Adapting to tired opponent - increasing aggression"));
    }

    return true;
}

bool UTask_FightAgainst::ValidateTaskConditions() const
{
    if (!Super::ValidateTaskConditions())
    {
        return false;
    }

    if (!Opponent || !IsValid(Opponent))
    {
        return false;
    }

    // Check if opponent is still alive and able to fight
    return OpponentHealth > 0.0f && CurrentStamina > 10.0f;
}

void UTask_FightAgainst::UpdateCombatAnalysis()
{
    if (!Opponent || !OwnerPed)
    {
        return;
    }

    FVector CurrentOpponentLocation = Opponent->GetActorLocation();
    FVector PedLocation = OwnerPed->GetActorLocation();

    // Update distance
    OpponentDistance = FVector::Dist(PedLocation, CurrentOpponentLocation);

    // Update movement analysis
    OpponentMovementDirection = (CurrentOpponentLocation - LastKnownOpponentLocation).GetSafeNormal();
    OpponentSpeed = FVector::Dist(CurrentOpponentLocation, LastKnownOpponentLocation) / GetWorld()->GetDeltaSeconds();
    LastKnownOpponentLocation = CurrentOpponentLocation;

    // Analyze opponent behavior (simplified)
    bOpponentIsAggressive = OpponentSpeed > 100.0f && OpponentDistance < 150.0f;
    bOpponentIsTired = OpponentSpeed < 50.0f;
    bOpponentIsBlocking = OpponentSpeed < 20.0f && OpponentDistance < 100.0f;
}

void UTask_FightAgainst::SelectBestAction()
{
    if (ActionCooldown > 0.0f)
    {
        return;
    }

    // Simple action selection based on distance and opponent state
    if (OpponentDistance > 200.0f)
    {
        CurrentAction = TEXT("Approach");
    }
    else if (OpponentDistance < 80.0f)
    {
        if (bOpponentIsBlocking)
        {
            CurrentAction = AvailableAttacks.Num() > 0 ? AvailableAttacks[FMath::RandRange(0, AvailableAttacks.Num() - 1)] : TEXT("Punch");
        }
        else if (bOpponentIsAggressive && FMath::RandRange(0.0f, 1.0f) < 0.6f)
        {
            CurrentAction = AvailableDefenses.Num() > 0 ? AvailableDefenses[FMath::RandRange(0, AvailableDefenses.Num() - 1)] : TEXT("Block");
        }
        else
        {
            CurrentAction = AvailableAttacks.Num() > 0 ? AvailableAttacks[FMath::RandRange(0, AvailableAttacks.Num() - 1)] : TEXT("Punch");
        }
    }
    else
    {
        CurrentAction = TEXT("Circle");
    }

    ActionCooldown = FMath::RandRange(0.5f, 2.0f) / FMath::Max(SkillLevel, 0.1f);
    UE_LOG(LogTemp, VeryVerbose, TEXT("Task_FightAgainst: Selected action: %s"), *CurrentAction);
}

void UTask_FightAgainst::ExecuteCurrentAction(float DeltaTime)
{
    if (!OwnerPed || CurrentAction.IsEmpty())
    {
        return;
    }

    FVector PedLocation = OwnerPed->GetActorLocation();
    FVector OpponentLocation = Opponent->GetActorLocation();
    FVector Direction = (OpponentLocation - PedLocation).GetSafeNormal();

    if (CurrentAction == TEXT("Approach"))
    {
        FVector TargetLocation = OpponentLocation - (Direction * 100.0f);
        FVector NewLocation = FMath::VInterpTo(PedLocation, TargetLocation, DeltaTime, 200.0f);
        OwnerPed->SetActorLocation(NewLocation);
    }
    else if (CurrentAction == TEXT("Circle"))
    {
        FVector RightVector = OwnerPed->GetActorRightVector();
        FVector CircleDirection = AggressionLevel > 0.5f ? RightVector : -RightVector;
        FVector NewLocation = PedLocation + (CircleDirection * 100.0f * DeltaTime);
        OwnerPed->SetActorLocation(NewLocation);
    }
    else
    {
        // Execute attack or defense action
        UE_LOG(LogTemp, VeryVerbose, TEXT("Task_FightAgainst: Executing %s"), *CurrentAction);
        // Here you would trigger actual animations and combat mechanics
    }
}

bool UTask_FightAgainst::CheckForCombatEnd() const
{
    return OpponentHealth <= 0.0f || CurrentStamina <= 0.0f || OpponentDistance > 500.0f;
}

// =====================================================
// UTask_CombatTargets Implementation
// =====================================================

UTask_CombatTargets::UTask_CombatTargets()
{
    TaskName = TEXT("CombatTargets");
    TaskDescription = TEXT("Engage multiple targets in combat");
    bRequiresTarget = false;
    TimeoutDuration = 180.0f; // 3 minutes for multi-target combat
    CombatStrategy = ECombatStrategy::Tactical;
    EngagementRange = 300.0f;
    MaxSimultaneousTargets = 3;
    bUseCover = true;
    bUseWeapons = false;
    
    PrimaryTarget = nullptr;
    SecondaryTarget = nullptr;
    bInCover = false;
    SuppressiveFire = 0.0f;
    ActiveTargets = 0;
    StrategyTimer = 0.0f;
    bNeedStrategyChange = false;
    PreviousStrategy = ECombatStrategy::Adaptive;
    
    bHasWeapon = false;
    bIsReloading = false;
    AmmoCount = 30;
    WeaponRange = 500.0f;
}

bool UTask_CombatTargets::InitializeComplexSystems()
{
    if (!OwnerPed)
    {
        return false;
    }

    // Initialize target database
    TargetDatabase.Empty();
    for (AActor* Target : CombatTargets)
    {
        if (Target && IsValid(Target))
        {
            FTargetInfo Info;
            Info.Target = Target;
            Info.Distance = FVector::Dist(OwnerPed->GetActorLocation(), Target->GetActorLocation());
            Info.ThreatLevel = 0.5f; // Default threat level
            Info.LastSeenTime = GetWorld()->GetTimeSeconds();
            Info.LastKnownPosition = Target->GetActorLocation();
            Info.bIsAlive = true;
            Info.bIsAggressive = false;
            Info.Health = 100.0f;
            
            TargetDatabase.Add(Info);
        }
    }

    if (TargetDatabase.Num() == 0)
    {
        UE_LOG(LogTemp, Error, TEXT("Task_CombatTargets: No valid targets found"));
        return false;
    }

    bSystemsInitialized = true;
    UE_LOG(LogTemp, Log, TEXT("Task_CombatTargets: Initialized combat against %d targets"), TargetDatabase.Num());
    return true;
}

bool UTask_CombatTargets::AnalyzeSituation(float DeltaTime)
{
    if (!OwnerPed)
    {
        return false;
    }

    UpdateTargetDatabase();
    SelectPrimaryTarget();
    DetermineOptimalStrategy();
    
    return true;
}

bool UTask_CombatTargets::PlanActions(float DeltaTime)
{
    if (!OwnerPed)
    {
        return false;
    }

    // Plan based on current strategy and situation
    switch (CombatStrategy)
    {
        case ECombatStrategy::Aggressive:
            // Plan direct assault
            break;
        case ECombatStrategy::Defensive:
            // Plan defensive positions
            if (bUseCover)
            {
                // Find cover position
                // This would use the RaycastUtils to find cover
            }
            break;
        case ECombatStrategy::Tactical:
            // Plan tactical approach
            break;
        case ECombatStrategy::Stealth:
            // Plan stealth approach
            break;
        case ECombatStrategy::Retreat:
            // Plan retreat route
            break;
        default:
            break;
    }

    return true;
}

bool UTask_CombatTargets::ExecuteComplexActions(float DeltaTime)
{
    if (!OwnerPed)
    {
        return false;
    }

    ExecuteCurrentStrategy(DeltaTime);

    // Check for strategy change
    StrategyTimer += DeltaTime;
    if (bNeedStrategyChange || StrategyTimer > 10.0f)
    {
        SetWildComplexState(EWildComplexTaskState::Adapting);
        return true;
    }

    // Check for combat end
    if (ActiveTargets == 0)
    {
        SetWildComplexState(EWildComplexTaskState::Finalizing);
        return true;
    }

    return true;
}

bool UTask_CombatTargets::AdaptToChanges(float DeltaTime)
{
    if (!OwnerPed)
    {
        return false;
    }

    // Evaluate current strategy effectiveness
    bool bStrategyEffective = true; // Simplified evaluation

    if (!bStrategyEffective || bNeedStrategyChange)
    {
        // Change strategy based on situation
        ECombatStrategy NewStrategy = CombatStrategy;
        
        if (ActiveTargets > 2)
        {
            NewStrategy = bUseCover ? ECombatStrategy::Defensive : ECombatStrategy::Tactical;
        }
        else if (ActiveTargets == 1)
        {
            NewStrategy = ECombatStrategy::Aggressive;
        }

        if (NewStrategy != CombatStrategy)
        {
            PreviousStrategy = CombatStrategy;
            CombatStrategy = NewStrategy;
            StrategyTimer = 0.0f;
            bNeedStrategyChange = false;
            
            UE_LOG(LogTemp, Log, TEXT("Task_CombatTargets: Strategy changed to %d"), (int32)NewStrategy);
        }
    }

    return true;
}

bool UTask_CombatTargets::ValidateTaskConditions() const
{
    if (!Super::ValidateTaskConditions())
    {
        return false;
    }

    // Check if any targets are still valid
    for (const FTargetInfo& Info : TargetDatabase)
    {
        if (Info.bIsAlive && IsValid(Info.Target))
        {
            return true;
        }
    }

    return false;
}

void UTask_CombatTargets::UpdateTargetDatabase()
{
    if (!OwnerPed)
    {
        return;
    }

    ActiveTargets = 0;
    FVector PedLocation = OwnerPed->GetActorLocation();

    for (FTargetInfo& Info : TargetDatabase)
    {
        if (!Info.Target || !IsValid(Info.Target))
        {
            Info.bIsAlive = false;
            continue;
        }

        // Update distance and position
        Info.LastKnownPosition = Info.Target->GetActorLocation();
        Info.Distance = FVector::Dist(PedLocation, Info.LastKnownPosition);
        Info.LastSeenTime = GetWorld()->GetTimeSeconds();

        // Update threat level based on distance and behavior
        if (Info.Distance <= EngagementRange)
        {
            Info.ThreatLevel = 1.0f - (Info.Distance / EngagementRange);
            ActiveTargets++;
        }
        else
        {
            Info.ThreatLevel *= 0.9f; // Decay threat for distant targets
        }
    }
}

void UTask_CombatTargets::SelectPrimaryTarget()
{
    AActor* BestTarget = nullptr;
    float HighestThreat = 0.0f;

    for (const FTargetInfo& Info : TargetDatabase)
    {
        if (Info.bIsAlive && Info.ThreatLevel > HighestThreat && Info.Distance <= EngagementRange)
        {
            HighestThreat = Info.ThreatLevel;
            BestTarget = Info.Target;
        }
    }

    if (BestTarget != PrimaryTarget)
    {
        SecondaryTarget = PrimaryTarget; // Demote current primary
        PrimaryTarget = BestTarget;
        
        if (PrimaryTarget)
        {
            UE_LOG(LogTemp, Log, TEXT("Task_CombatTargets: New primary target selected: %s"), *PrimaryTarget->GetName());
        }
    }
}

void UTask_CombatTargets::DetermineOptimalStrategy()
{
    // Simple strategy determination based on situation
    if (ActiveTargets > MaxSimultaneousTargets)
    {
        bNeedStrategyChange = (CombatStrategy != ECombatStrategy::Defensive && CombatStrategy != ECombatStrategy::Retreat);
    }
    else if (ActiveTargets == 1)
    {
        bNeedStrategyChange = (CombatStrategy != ECombatStrategy::Aggressive);
    }
    else
    {
        bNeedStrategyChange = (CombatStrategy != ECombatStrategy::Tactical);
    }
}

void UTask_CombatTargets::ExecuteCurrentStrategy(float DeltaTime)
{
    switch (CombatStrategy)
    {
        case ECombatStrategy::Aggressive:
            if (PrimaryTarget)
            {
                // Move towards primary target
                FVector TargetLocation = PrimaryTarget->GetActorLocation();
                FVector PedLocation = OwnerPed->GetActorLocation();
                FVector Direction = (TargetLocation - PedLocation).GetSafeNormal();
                FVector NewLocation = PedLocation + (Direction * 200.0f * DeltaTime);
                OwnerPed->SetActorLocation(NewLocation);
            }
            break;

        case ECombatStrategy::Defensive:
            HandleCoverMovement(DeltaTime);
            break;

        case ECombatStrategy::Tactical:
            // Tactical movement and positioning
            break;

        default:
            break;
    }

    if (bUseWeapons)
    {
        HandleWeaponCombat(DeltaTime);
    }
    else
    {
        HandleMeleeCombat(DeltaTime);
    }
}

void UTask_CombatTargets::HandleCoverMovement(float DeltaTime)
{
    // Simplified cover movement
    if (!bInCover && bUseCover)
    {
        // Find and move to cover
        // This would use RaycastUtils for cover detection
        bInCover = true;
    }
}

void UTask_CombatTargets::HandleWeaponCombat(float DeltaTime)
{
    if (!bHasWeapon || bIsReloading)
    {
        return;
    }

    if (PrimaryTarget && AmmoCount > 0)
    {
        // Simplified weapon firing
        AmmoCount--;
        UE_LOG(LogTemp, VeryVerbose, TEXT("Task_CombatTargets: Firing at primary target (Ammo: %d)"), AmmoCount);
    }

    if (AmmoCount <= 0)
    {
        bIsReloading = true;
        // Start reload timer
    }
}

void UTask_CombatTargets::HandleMeleeCombat(float DeltaTime)
{
    if (PrimaryTarget)
    {
        float Distance = FVector::Dist(OwnerPed->GetActorLocation(), PrimaryTarget->GetActorLocation());
        if (Distance <= 100.0f)
        {
            // In melee range - execute attack
            UE_LOG(LogTemp, VeryVerbose, TEXT("Task_CombatTargets: Melee attack on primary target"));
        }
    }
}

bool UTask_CombatTargets::ShouldRetreat() const
{
    return ActiveTargets > MaxSimultaneousTargets * 2;
}

bool UTask_CombatTargets::ShouldAdvance() const
{
    return ActiveTargets <= 1 && PrimaryTarget != nullptr;
}

void UTask_CombatTargets::SetTargets(const TArray<APed*>& Targets)
{
    CombatTargets.Reset();
    for (APed* Target : Targets)
    {
        if (Target)
        {
            CombatTargets.Add(Target);
        }
    }
}
