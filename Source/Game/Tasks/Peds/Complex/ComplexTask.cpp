#include "ComplexTask.h"
#include "../../../Peds/Ped.h"
#include "../../../Core/Utils/RaycastUtils.h"
#include "Engine/World.h"
#include "Components/SkeletalMeshComponent.h"
#include "GameFramework/CharacterMovementComponent.h"

// =====================================================
// UComplexTask Implementation
// =====================================================

UComplexTask::UComplexTask()
{
    TaskType = ETaskType::Complex;
    TaskName = TEXT("ComplexTask");
    TimeoutDuration = 30.0f;
    bRequiresPreparation = true;
    PhaseTimeout = 10.0f;
    CurrentPhase = EComplexTaskPhase::Preparation;
    PhaseTime = 0.0f;
    bPhaseChanged = false;
}

bool UComplexTask::ExecuteTask()
{
    if (bRequiresPreparation)
    {
        SetCurrentPhase(EComplexTaskPhase::Preparation);
        return PrepareTask();
    }
    else
    {
        SetCurrentPhase(EComplexTaskPhase::Execution);
        return true;
    }
}

void UComplexTask::UpdateTask(float DeltaTime)
{
    Super::UpdateTask(DeltaTime);

    PhaseTime += DeltaTime;

    // Check phase timeout
    if (PhaseTimeout > 0.0f && PhaseTime >= PhaseTimeout)
    {
        UE_LOG(LogTemp, Warning, TEXT("ComplexTask %s: Phase %d timed out"), *TaskName, (int32)CurrentPhase);
        CompleteTask(false, TEXT("Phase timed out"));
        return;
    }

    // Execute current phase
    bool bPhaseSuccess = ExecutePhase(CurrentPhase, DeltaTime);
    if (!bPhaseSuccess)
    {
        CompleteTask(false, TEXT("Phase execution failed"));
        return;
    }

    // Check if we can advance to next phase
    if (CanAdvanceToNextPhase(CurrentPhase))
    {
        AdvanceToNextPhase();
    }
}

void UComplexTask::CleanupTask()
{
    Super::CleanupTask();
    SetCurrentPhase(EComplexTaskPhase::Cleanup);
}

void UComplexTask::AdvanceToNextPhase()
{
    EComplexTaskPhase OldPhase = CurrentPhase;
    EComplexTaskPhase NewPhase = CurrentPhase;

    switch (CurrentPhase)
    {
        case EComplexTaskPhase::Preparation:
            NewPhase = EComplexTaskPhase::Execution;
            break;
        case EComplexTaskPhase::Execution:
            NewPhase = EComplexTaskPhase::Completion;
            break;
        case EComplexTaskPhase::Completion:
            CompleteTask(true, TEXT("Complex task completed successfully"));
            return;
        case EComplexTaskPhase::Cleanup:
            // Stay in cleanup
            return;
    }

    SetCurrentPhase(NewPhase);
    OnPhaseChanged(OldPhase, NewPhase);
}

void UComplexTask::SetCurrentPhase(EComplexTaskPhase NewPhase)
{
    if (CurrentPhase != NewPhase)
    {
        CurrentPhase = NewPhase;
        PhaseTime = 0.0f;
        bPhaseChanged = true;

        UE_LOG(LogTemp, Log, TEXT("ComplexTask %s: Phase changed to %d"), *TaskName, (int32)NewPhase);
    }
}

// =====================================================
// UTask_Climb Implementation
// =====================================================

UTask_Climb::UTask_Climb()
{
    TaskName = TEXT("Climb");
    TaskDescription = TEXT("Climb over obstacles or walls");
    bRequiresTarget = false;
    TimeoutDuration = 15.0f;
    ClimbHeight = 200.0f;
    ClimbSpeed = 100.0f;
    bAutoDetectClimbHeight = true;
    bHasValidClimbPath = false;
    ClimbProgress = 0.0f;
}

bool UTask_Climb::PrepareTask()
{
    if (!OwnerPed)
    {
        return false;
    }

    StartLocation = OwnerPed->GetActorLocation();
    
    // If no climb target specified, use forward direction
    if (ClimbTarget.IsZero())
    {
        ClimbTarget = StartLocation + (OwnerPed->GetActorForwardVector() * 100.0f);
    }

    // Auto-detect climb height if enabled
    if (bAutoDetectClimbHeight)
    {
        FHitResult HitResult;
        FVector TraceStart = ClimbTarget + FVector(0, 0, 300.0f);
        FVector TraceEnd = ClimbTarget - FVector(0, 0, 50.0f);
        
        bool bHit = GetWorld()->LineTraceSingleByChannel(
            HitResult,
            TraceStart,
            TraceEnd,
            ECC_WorldStatic
        );

        if (bHit)
        {
            ClimbHeight = HitResult.Location.Z - StartLocation.Z + 50.0f;
            TopLocation = HitResult.Location + FVector(0, 0, 50.0f);
        }
        else
        {
            TopLocation = ClimbTarget + FVector(0, 0, ClimbHeight);
        }
    }
    else
    {
        TopLocation = ClimbTarget + FVector(0, 0, ClimbHeight);
    }

    GrabLocation = ClimbTarget + FVector(0, 0, ClimbHeight * 0.7f);
    EndLocation = TopLocation + (OwnerPed->GetActorForwardVector() * 50.0f);

    bHasValidClimbPath = true;

    UE_LOG(LogTemp, Log, TEXT("Task_Climb: Prepared climb path - Height: %f"), ClimbHeight);
    return true;
}

bool UTask_Climb::ExecutePhase(EComplexTaskPhase Phase, float DeltaTime)
{
    if (!OwnerPed || !bHasValidClimbPath)
    {
        return false;
    }

    switch (Phase)
    {
        case EComplexTaskPhase::Preparation:
            // Already handled in PrepareTask
            return true;

        case EComplexTaskPhase::Execution:
        {
            FVector CurrentLocation = OwnerPed->GetActorLocation();
            FVector TargetLocation;

            // Determine current climb stage based on progress
            if (ClimbProgress < 0.3f)
            {
                // Move to grab position
                TargetLocation = GrabLocation;
            }
            else if (ClimbProgress < 0.8f)
            {
                // Climb to top
                TargetLocation = TopLocation;
            }
            else
            {
                // Move to end position
                TargetLocation = EndLocation;
            }

            // Move towards target
            FVector NewLocation = FMath::VInterpTo(CurrentLocation, TargetLocation, DeltaTime, ClimbSpeed);
            OwnerPed->SetActorLocation(NewLocation);

            // Update progress
            float TotalDistance = FVector::Dist(StartLocation, EndLocation);
            float CurrentDistance = FVector::Dist(StartLocation, CurrentLocation);
            ClimbProgress = FMath::Clamp(CurrentDistance / TotalDistance, 0.0f, 1.0f);

            return true;
        }

        case EComplexTaskPhase::Completion:
            // Ensure final position
            OwnerPed->SetActorLocation(EndLocation);
            return true;

        default:
            return true;
    }
}

bool UTask_Climb::CanAdvanceToNextPhase(EComplexTaskPhase InPhase) const
{
    switch (CurrentPhase)
    {
        case EComplexTaskPhase::Preparation:
            return bHasValidClimbPath;

        case EComplexTaskPhase::Execution:
            return ClimbProgress >= 0.95f;

        case EComplexTaskPhase::Completion:
            return true;

        default:
            return false;
    }
}

bool UTask_Climb::ValidateTaskConditions() const
{
    if (!Super::ValidateTaskConditions())
    {
        return false;
    }

    // Check if climb path is still valid
    return bHasValidClimbPath;
}

// =====================================================
// UTask_EnterVehicle Implementation
// =====================================================

UTask_EnterVehicle::UTask_EnterVehicle()
{
    TaskName = TEXT("EnterVehicle");
    TaskDescription = TEXT("Enter a vehicle");
    bRequiresTarget = true;
    TimeoutDuration = 10.0f;
    SeatIndex = 0; // Driver seat
    bOpenDoor = true;
    ApproachSpeed = 200.0f;
    bDoorOpened = false;
    bReachedDoor = false;
    ApproachProgress = 0.0f;
}

bool UTask_EnterVehicle::PrepareTask()
{
    if (!OwnerPed)
    {
        return false;
    }

    // Cast target to vehicle (this would be a proper vehicle class in production)
    TargetVehicle = TaskTarget;
    if (!TargetVehicle)
    {
        UE_LOG(LogTemp, Error, TEXT("Task_EnterVehicle: No target specified"));
        return false;
    }

    // Calculate door location (simplified - would use actual door bones/sockets)
    FVector VehicleLocation = TargetVehicle->GetActorLocation();
    FVector VehicleForward = TargetVehicle->GetActorForwardVector();
    FVector VehicleRight = TargetVehicle->GetActorRightVector();

    // Driver side door location (left side for most vehicles)
    DoorLocation = VehicleLocation + (VehicleRight * -100.0f) + (VehicleForward * 50.0f);
    SeatLocation = VehicleLocation + (VehicleRight * -50.0f);

    UE_LOG(LogTemp, Log, TEXT("Task_EnterVehicle: Prepared vehicle entry"));
    return true;
}

bool UTask_EnterVehicle::ExecutePhase(EComplexTaskPhase Phase, float DeltaTime)
{
    if (!OwnerPed || !TargetVehicle)
    {
        return false;
    }

    switch (Phase)
    {
        case EComplexTaskPhase::Preparation:
            return true;

        case EComplexTaskPhase::Execution:
        {
            FVector CurrentLocation = OwnerPed->GetActorLocation();

            if (!bReachedDoor)
            {
                // Approach the door
                FVector NewLocation = FMath::VInterpTo(CurrentLocation, DoorLocation, DeltaTime, ApproachSpeed);
                OwnerPed->SetActorLocation(NewLocation);

                float DistanceToDoor = FVector::Dist(CurrentLocation, DoorLocation);
                ApproachProgress = 1.0f - FMath::Clamp(DistanceToDoor / 200.0f, 0.0f, 1.0f);

                if (DistanceToDoor <= 30.0f)
                {
                    bReachedDoor = true;
                    UE_LOG(LogTemp, Log, TEXT("Task_EnterVehicle: Reached door"));
                }
            }
            else if (bOpenDoor && !bDoorOpened)
            {
                // Open door (simplified - would trigger door animation)
                bDoorOpened = true;
                UE_LOG(LogTemp, Log, TEXT("Task_EnterVehicle: Door opened"));
            }
            else
            {
                // Enter vehicle
                FVector NewLocation = FMath::VInterpTo(CurrentLocation, SeatLocation, DeltaTime, ApproachSpeed * 0.5f);
                OwnerPed->SetActorLocation(NewLocation);

                float DistanceToSeat = FVector::Dist(CurrentLocation, SeatLocation);
                if (DistanceToSeat <= 20.0f)
                {
                    // Ped is now in vehicle
                    UE_LOG(LogTemp, Log, TEXT("Task_EnterVehicle: Entered vehicle"));
                    return true;
                }
            }

            return true;
        }

        case EComplexTaskPhase::Completion:
            // Attach ped to vehicle seat
            OwnerPed->SetActorLocation(SeatLocation);
            return true;

        default:
            return true;
    }
}

bool UTask_EnterVehicle::CanAdvanceToNextPhase(EComplexTaskPhase InPhase) const
{
    switch (CurrentPhase)
    {
        case EComplexTaskPhase::Preparation:
            return TargetVehicle != nullptr;

        case EComplexTaskPhase::Execution:
        {
            if (!OwnerPed)
                return false;
            
            float DistanceToSeat = FVector::Dist(OwnerPed->GetActorLocation(), SeatLocation);
            return DistanceToSeat <= 20.0f;
        }

        case EComplexTaskPhase::Completion:
            return true;

        default:
            return false;
    }
}

bool UTask_EnterVehicle::ValidateTaskConditions() const
{
    if (!Super::ValidateTaskConditions())
    {
        return false;
    }

    return TargetVehicle && IsValid(TargetVehicle);
}

// =====================================================
// UTask_GrabLedgeAndHold Implementation
// =====================================================

UTask_GrabLedgeAndHold::UTask_GrabLedgeAndHold()
{
    TaskName = TEXT("GrabLedgeAndHold");
    TaskDescription = TEXT("Grab a ledge and hold position");
    bRequiresTarget = false;
    TimeoutDuration = 30.0f;
    GrabRange = 100.0f;
    HoldDuration = 5.0f;
    bAutoDetectLedge = true;
    StaminaDrainRate = 10.0f; // per second
    bIsHanging = false;
    HoldTime = 0.0f;
    CurrentStamina = 100.0f;
    bHasValidLedge = false;
}

bool UTask_GrabLedgeAndHold::PrepareTask()
{
    if (!OwnerPed)
    {
        return false;
    }

    FVector PedLocation = OwnerPed->GetActorLocation();

    if (bAutoDetectLedge)
    {
        // Trace forward and up to find ledge
        FVector TraceStart = PedLocation + (OwnerPed->GetActorForwardVector() * 50.0f);
        FVector TraceEnd = TraceStart + FVector(0, 0, GrabRange);

        FHitResult HitResult;
        bool bHit = GetWorld()->LineTraceSingleByChannel(
            HitResult,
            TraceStart,
            TraceEnd,
            ECC_WorldStatic
        );

        if (bHit)
        {
            LedgeLocation = HitResult.Location;
            bHasValidLedge = true;
        }
        else
        {
            UE_LOG(LogTemp, Warning, TEXT("Task_GrabLedgeAndHold: No ledge found"));
            return false;
        }
    }
    else if (!LedgeLocation.IsZero())
    {
        bHasValidLedge = true;
    }

    if (bHasValidLedge)
    {
        GrabPosition = LedgeLocation + FVector(0, 0, -20.0f);
        HangPosition = LedgeLocation + FVector(0, 0, -100.0f);
        
        UE_LOG(LogTemp, Log, TEXT("Task_GrabLedgeAndHold: Found valid ledge"));
    }

    return bHasValidLedge;
}

bool UTask_GrabLedgeAndHold::ExecutePhase(EComplexTaskPhase Phase, float DeltaTime)
{
    if (!OwnerPed || !bHasValidLedge)
    {
        return false;
    }

    switch (Phase)
    {
        case EComplexTaskPhase::Preparation:
            return true;

        case EComplexTaskPhase::Execution:
        {
            if (!bIsHanging)
            {
                // Move to grab position
                FVector CurrentLocation = OwnerPed->GetActorLocation();
                FVector NewLocation = FMath::VInterpTo(CurrentLocation, GrabPosition, DeltaTime, 200.0f);
                OwnerPed->SetActorLocation(NewLocation);

                float DistanceToGrab = FVector::Dist(CurrentLocation, GrabPosition);
                if (DistanceToGrab <= 20.0f)
                {
                    bIsHanging = true;
                    UE_LOG(LogTemp, Log, TEXT("Task_GrabLedgeAndHold: Started hanging"));
                }
            }
            else
            {
                // Hold position and drain stamina
                OwnerPed->SetActorLocation(HangPosition);
                HoldTime += DeltaTime;
                CurrentStamina -= StaminaDrainRate * DeltaTime;

                if (CurrentStamina <= 0.0f)
                {
                    UE_LOG(LogTemp, Warning, TEXT("Task_GrabLedgeAndHold: Ran out of stamina"));
                    return false;
                }
            }

            return true;
        }

        case EComplexTaskPhase::Completion:
            return true;

        default:
            return true;
    }
}

bool UTask_GrabLedgeAndHold::CanAdvanceToNextPhase(EComplexTaskPhase InPhase) const
{
    switch (CurrentPhase)
    {
        case EComplexTaskPhase::Preparation:
            return bHasValidLedge;

        case EComplexTaskPhase::Execution:
            return bIsHanging && (HoldTime >= HoldDuration || CurrentStamina <= 0.0f);

        case EComplexTaskPhase::Completion:
            return true;

        default:
            return false;
    }
}

bool UTask_GrabLedgeAndHold::ValidateTaskConditions() const
{
    if (!Super::ValidateTaskConditions())
    {
        return false;
    }

    return bHasValidLedge && CurrentStamina > 0.0f;
}

// =====================================================
// UTask_ClimbLadder Implementation
// =====================================================

UTask_ClimbLadder::UTask_ClimbLadder()
{
    TaskName = TEXT("ClimbLadder");
    TaskDescription = TEXT("Climb up or down a ladder");
    bRequiresTarget = true;
    TimeoutDuration = 20.0f;
    bClimbUp = true;
    ClimbSpeed = 150.0f;
    ApproachDistance = 50.0f;
    bReachedLadder = false;
    bIsClimbing = false;
    ClimbProgress = 0.0f;
    LadderHeight = 0.0f;
}

bool UTask_ClimbLadder::PrepareTask()
{
    if (!OwnerPed)
    {
        return false;
    }

    // Cast target to ladder (this would be a proper ladder class in production)
    TargetLadder = TaskTarget;
    if (!TargetLadder)
    {
        UE_LOG(LogTemp, Error, TEXT("Task_ClimbLadder: No target specified"));
        return false;
    }

    // Get ladder positions (simplified - would use actual ladder data)
    FVector LadderLocation = TargetLadder->GetActorLocation();
    LadderBottomLocation = LadderLocation;
    LadderTopLocation = LadderLocation + FVector(0, 0, 400.0f); // Default ladder height
    LadderHeight = FVector::Dist(LadderBottomLocation, LadderTopLocation);

    if (bClimbUp)
    {
        StartClimbLocation = LadderBottomLocation;
        TargetLocation = LadderTopLocation;
    }
    else
    {
        StartClimbLocation = LadderTopLocation;
        TargetLocation = LadderBottomLocation;
    }

    UE_LOG(LogTemp, Log, TEXT("Task_ClimbLadder: Prepared ladder climb - Height: %f"), LadderHeight);
    return true;
}

bool UTask_ClimbLadder::ExecutePhase(EComplexTaskPhase Phase, float DeltaTime)
{
    if (!OwnerPed || !TargetLadder)
    {
        return false;
    }

    switch (Phase)
    {
        case EComplexTaskPhase::Preparation:
            return true;

        case EComplexTaskPhase::Execution:
        {
            FVector CurrentLocation = OwnerPed->GetActorLocation();

            if (!bReachedLadder)
            {
                // Approach the ladder
                FVector ApproachLocation = StartClimbLocation + (OwnerPed->GetActorForwardVector() * -ApproachDistance);
                FVector NewLocation = FMath::VInterpTo(CurrentLocation, ApproachLocation, DeltaTime, ClimbSpeed);
                OwnerPed->SetActorLocation(NewLocation);

                float DistanceToLadder = FVector::Dist(CurrentLocation, StartClimbLocation);
                if (DistanceToLadder <= ApproachDistance + 10.0f)
                {
                    bReachedLadder = true;
                    UE_LOG(LogTemp, Log, TEXT("Task_ClimbLadder: Reached ladder"));
                }
            }
            else if (!bIsClimbing)
            {
                // Position at ladder start
                OwnerPed->SetActorLocation(StartClimbLocation);
                bIsClimbing = true;
                UE_LOG(LogTemp, Log, TEXT("Task_ClimbLadder: Started climbing"));
            }
            else
            {
                // Climb the ladder
                FVector NewLocation = FMath::VInterpTo(CurrentLocation, TargetLocation, DeltaTime, ClimbSpeed);
                OwnerPed->SetActorLocation(NewLocation);

                // Update progress
                float TotalDistance = FVector::Dist(StartClimbLocation, TargetLocation);
                float CurrentDistance = FVector::Dist(StartClimbLocation, CurrentLocation);
                ClimbProgress = FMath::Clamp(CurrentDistance / TotalDistance, 0.0f, 1.0f);
            }

            return true;
        }

        case EComplexTaskPhase::Completion:
            // Ensure final position
            OwnerPed->SetActorLocation(TargetLocation);
            return true;

        default:
            return true;
    }
}

bool UTask_ClimbLadder::CanAdvanceToNextPhase(EComplexTaskPhase InPhase) const
{
    switch (CurrentPhase)
    {
        case EComplexTaskPhase::Preparation:
            return TargetLadder != nullptr;

        case EComplexTaskPhase::Execution:
            return bIsClimbing && ClimbProgress >= 0.95f;

        case EComplexTaskPhase::Completion:
            return true;

        default:
            return false;
    }
}

bool UTask_ClimbLadder::ValidateTaskConditions() const
{
    if (!Super::ValidateTaskConditions())
    {
        return false;
    }

    return TargetLadder && IsValid(TargetLadder);
}
