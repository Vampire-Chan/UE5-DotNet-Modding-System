#include "OneShotTask.h"
#include "../../../Peds/Ped.h"
#include "../../../Core/Utils/RaycastUtils.h"
#include "Engine/World.h"
#include "Components/SkeletalMeshComponent.h"
#include "GameFramework/CharacterMovementComponent.h"

// =====================================================
// UOneShotTask Implementation
// =====================================================

UOneShotTask::UOneShotTask()
{
    TaskType = ETaskType::OneShot;
    TaskName = TEXT("OneShotTask");
    TimeoutDuration = 5.0f;
    bInstantComplete = false;
    OneShotDuration = 1.0f;
}

bool UOneShotTask::ExecuteTask()
{
    if (bInstantComplete)
    {
        bool bSuccess = PerformOneShotAction();
        CompleteTask(bSuccess, bSuccess ? TEXT("OneShot action completed instantly") : TEXT("OneShot action failed"));
        return bSuccess;
    }

    return PerformOneShotAction();
}

void UOneShotTask::UpdateTask(float DeltaTime)
{
    Super::UpdateTask(DeltaTime);

    if (!bInstantComplete && ExecutionTime >= OneShotDuration)
    {
        CompleteTask(true, TEXT("OneShot task duration completed"));
    }
}

// =====================================================
// UTask_Aim Implementation
// =====================================================

UTask_Aim::UTask_Aim()
{
    TaskName = TEXT("Aim");
    TaskDescription = TEXT("Aim weapon at target");
    bRequiresTarget = true;
    OneShotDuration = 0.5f;
    AimSpeed = 180.0f; // degrees per second
    AimAccuracy = 0.95f;
    bUsePredictiveAiming = false;
    AimProgress = 0.0f;
}

bool UTask_Aim::PerformOneShotAction()
{
    if (!OwnerPed || !TaskTarget)
    {
        return false;
    }

    // Calculate aim direction
    FVector TargetLocation = TaskTarget->GetActorLocation();
    FVector PedLocation = OwnerPed->GetActorLocation();
    AimDirection = (TargetLocation - PedLocation).GetSafeNormal();

    CurrentAimDirection = OwnerPed->GetActorForwardVector();

    UE_LOG(LogTemp, Log, TEXT("Task_Aim: Starting aim at target %s"), *TaskTarget->GetName());
    return true;
}

void UTask_Aim::UpdateTask(float DeltaTime)
{
    if (!OwnerPed || !TaskTarget)
    {
        CompleteTask(false, TEXT("Invalid ped or target"));
        return;
    }

    // Update target location for moving targets
    FVector TargetLocation = TaskTarget->GetActorLocation();
    FVector PedLocation = OwnerPed->GetActorLocation();
    AimDirection = (TargetLocation - PedLocation).GetSafeNormal();

    // Interpolate aim direction
    float AimDelta = AimSpeed * DeltaTime;
    CurrentAimDirection = FMath::VInterpTo(CurrentAimDirection, AimDirection, DeltaTime, AimDelta);

    // Calculate aim progress
    float DotProduct = FVector::DotProduct(CurrentAimDirection, AimDirection);
    AimProgress = FMath::Clamp((DotProduct + 1.0f) * 0.5f, 0.0f, 1.0f);

    // Check if aim is accurate enough
    if (AimProgress >= AimAccuracy)
    {
        CompleteTask(true, TEXT("Target acquired"));
        return;
    }

    Super::UpdateTask(DeltaTime);
}

bool UTask_Aim::ValidateTaskConditions() const
{
    if (!Super::ValidateTaskConditions())
    {
        return false;
    }

    if (!TaskTarget || !IsValid(TaskTarget))
    {
        return false;
    }

    // Check if target is still in range and visible
    if (OwnerPed)
    {
        float Distance = FVector::Dist(OwnerPed->GetActorLocation(), TaskTarget->GetActorLocation());
        if (Distance > 1000.0f) // Max aim range
        {
            return false;
        }
    }

    return true;
}

// =====================================================
// UTask_LookAt Implementation
// =====================================================

UTask_LookAt::UTask_LookAt()
{
    TaskName = TEXT("LookAt");
    TaskDescription = TEXT("Look at target or direction");
    bRequiresTarget = false;
    OneShotDuration = 0.3f;
    LookSpeed = 270.0f; // degrees per second
    bLookAtActor = true;
    bOnlyHorizontal = false;
    LookProgress = 0.0f;
}

bool UTask_LookAt::PerformOneShotAction()
{
    if (!OwnerPed)
    {
        return false;
    }

    StartRotation = OwnerPed->GetActorRotation();

    if (bLookAtActor && TaskTarget)
    {
        FVector TargetLocation = TaskTarget->GetActorLocation();
        FVector PedLocation = OwnerPed->GetActorLocation();
        LookDirection = (TargetLocation - PedLocation).GetSafeNormal();
    }

    TargetRotation = LookDirection.Rotation();
    
    if (bOnlyHorizontal)
    {
        TargetRotation.Pitch = StartRotation.Pitch;
        TargetRotation.Roll = StartRotation.Roll;
    }

    UE_LOG(LogTemp, Log, TEXT("Task_LookAt: Starting look from %s to %s"), 
           *StartRotation.ToString(), *TargetRotation.ToString());
    return true;
}

void UTask_LookAt::UpdateTask(float DeltaTime)
{
    if (!OwnerPed)
    {
        CompleteTask(false, TEXT("Invalid ped"));
        return;
    }

    // Interpolate rotation
    FRotator CurrentRotation = OwnerPed->GetActorRotation();
    FRotator NewRotation = FMath::RInterpTo(CurrentRotation, TargetRotation, DeltaTime, LookSpeed);
    OwnerPed->SetActorRotation(NewRotation);

    // Calculate progress
    float RotationDifference = FMath::Abs((TargetRotation - CurrentRotation).Yaw);
    LookProgress = 1.0f - FMath::Clamp(RotationDifference / 180.0f, 0.0f, 1.0f);

    // Check if look is complete
    if (LookProgress >= 0.95f)
    {
        OwnerPed->SetActorRotation(TargetRotation);
        CompleteTask(true, TEXT("Look completed"));
        return;
    }

    Super::UpdateTask(DeltaTime);
}

bool UTask_LookAt::ValidateTaskConditions() const
{
    if (!Super::ValidateTaskConditions())
    {
        return false;
    }

    if (bLookAtActor && (!TaskTarget || !IsValid(TaskTarget)))
    {
        return false;
    }

    return true;
}

// =====================================================
// UTask_Turn Implementation
// =====================================================

UTask_Turn::UTask_Turn()
{
    TaskName = TEXT("Turn");
    TaskDescription = TEXT("Turn to face direction or actor");
    bRequiresTarget = false;
    OneShotDuration = 1.0f;
    TurnSpeed = 180.0f; // degrees per second
    bTurnToFaceActor = false;
    TurnTolerance = 5.0f; // degrees
    TurnProgress = 0.0f;
    TotalTurnAngle = 0.0f;
}

bool UTask_Turn::PerformOneShotAction()
{
    if (!OwnerPed)
    {
        return false;
    }

    StartRotation = OwnerPed->GetActorRotation();

    if (bTurnToFaceActor && TaskTarget)
    {
        FVector TargetLocation = TaskTarget->GetActorLocation();
        FVector PedLocation = OwnerPed->GetActorLocation();
        FVector Direction = (TargetLocation - PedLocation).GetSafeNormal();
        TargetRotation = Direction.Rotation();
    }

    // Calculate total turn angle needed
    TotalTurnAngle = FMath::Abs(FMath::FindDeltaAngleDegrees(StartRotation.Yaw, TargetRotation.Yaw));

    UE_LOG(LogTemp, Log, TEXT("Task_Turn: Starting turn of %f degrees"), TotalTurnAngle);
    return true;
}

void UTask_Turn::UpdateTask(float DeltaTime)
{
    if (!OwnerPed)
    {
        CompleteTask(false, TEXT("Invalid ped"));
        return;
    }

    // Interpolate rotation
    FRotator CurrentRotation = OwnerPed->GetActorRotation();
    FRotator NewRotation = FMath::RInterpTo(CurrentRotation, TargetRotation, DeltaTime, TurnSpeed);
    OwnerPed->SetActorRotation(NewRotation);

    // Calculate progress
    float CurrentAngleDiff = FMath::Abs(FMath::FindDeltaAngleDegrees(CurrentRotation.Yaw, TargetRotation.Yaw));
    TurnProgress = 1.0f - (CurrentAngleDiff / TotalTurnAngle);

    // Check if turn is complete
    if (CurrentAngleDiff <= TurnTolerance)
    {
        OwnerPed->SetActorRotation(TargetRotation);
        CompleteTask(true, TEXT("Turn completed"));
        return;
    }

    Super::UpdateTask(DeltaTime);
}

bool UTask_Turn::ValidateTaskConditions() const
{
    if (!Super::ValidateTaskConditions())
    {
        return false;
    }

    if (bTurnToFaceActor && (!TaskTarget || !IsValid(TaskTarget)))
    {
        return false;
    }

    return true;
}

// =====================================================
// UTask_Shimmy Implementation
// =====================================================

UTask_Shimmy::UTask_Shimmy()
{
    TaskName = TEXT("Shimmy");
    TaskDescription = TEXT("Side-step movement along cover");
    bRequiresTarget = false;
    OneShotDuration = 2.0f;
    ShimmyDistance = 100.0f;
    ShimmySpeed = 150.0f; // units per second
    bCheckForObstacles = true;
    ShimmyProgress = 0.0f;
}

bool UTask_Shimmy::PerformOneShotAction()
{
    if (!OwnerPed)
    {
        return false;
    }

    StartLocation = OwnerPed->GetActorLocation();
    
    // If no direction specified, use right vector
    if (ShimmyDirection.IsZero())
    {
        ShimmyDirection = OwnerPed->GetActorRightVector();
    }

    TargetLocation = StartLocation + (ShimmyDirection.GetSafeNormal() * ShimmyDistance);

    // Check for obstacles if required
    if (bCheckForObstacles)
    {
        FHitResult HitResult;
        bool bHit = GetWorld()->LineTraceSingleByChannel(
            HitResult,
            StartLocation,
            TargetLocation,
            ECC_WorldStatic
        );

        if (bHit)
        {
            // Adjust target location to stop before obstacle
            TargetLocation = HitResult.Location - (ShimmyDirection.GetSafeNormal() * 20.0f);
            ShimmyDistance = FVector::Dist(StartLocation, TargetLocation);
        }
    }

    UE_LOG(LogTemp, Log, TEXT("Task_Shimmy: Starting shimmy %f units"), ShimmyDistance);
    return true;
}

void UTask_Shimmy::UpdateTask(float DeltaTime)
{
    if (!OwnerPed)
    {
        CompleteTask(false, TEXT("Invalid ped"));
        return;
    }

    // Move towards target location
    FVector CurrentLocation = OwnerPed->GetActorLocation();
    FVector NewLocation = FMath::VInterpTo(CurrentLocation, TargetLocation, DeltaTime, ShimmySpeed);
    OwnerPed->SetActorLocation(NewLocation);

    // Calculate progress
    float TravelDistance = FVector::Dist(StartLocation, CurrentLocation);
    ShimmyProgress = FMath::Clamp(TravelDistance / ShimmyDistance, 0.0f, 1.0f);

    // Check if shimmy is complete
    float DistanceToTarget = FVector::Dist(CurrentLocation, TargetLocation);
    if (DistanceToTarget <= 5.0f)
    {
        OwnerPed->SetActorLocation(TargetLocation);
        CompleteTask(true, TEXT("Shimmy completed"));
        return;
    }

    Super::UpdateTask(DeltaTime);
}

bool UTask_Shimmy::ValidateTaskConditions() const
{
    if (!Super::ValidateTaskConditions())
    {
        return false;
    }

    // Check if ped is in cover (this would be expanded with actual cover detection)
    return true;
}

// =====================================================
// UTask_DropDown Implementation
// =====================================================

UTask_DropDown::UTask_DropDown()
{
    TaskName = TEXT("DropDown");
    TaskDescription = TEXT("Drop down from ledge grab state");
    bRequiresTarget = false;
    OneShotDuration = 1.0f;
    DropHeight = 200.0f;
    bCheckLandingSafety = true;
    SafeLandingDistance = 300.0f;
    bIsDropping = false;
    DropProgress = 0.0f;
}

bool UTask_DropDown::PerformOneShotAction()
{
    if (!OwnerPed)
    {
        return false;
    }

    StartLocation = OwnerPed->GetActorLocation();
    LandingLocation = StartLocation - FVector(0, 0, DropHeight);

    // Check landing safety if required
    if (bCheckLandingSafety)
    {
        FHitResult HitResult;
        bool bHit = GetWorld()->LineTraceSingleByChannel(
            HitResult,
            StartLocation,
            StartLocation - FVector(0, 0, SafeLandingDistance),
            ECC_WorldStatic
        );

        if (bHit)
        {
            LandingLocation = HitResult.Location + FVector(0, 0, 20.0f); // Slight offset above ground
            DropHeight = FVector::Dist(StartLocation, LandingLocation);
        }
        else
        {
            UE_LOG(LogTemp, Warning, TEXT("Task_DropDown: No safe landing found"));
            return false;
        }
    }

    bIsDropping = true;
    UE_LOG(LogTemp, Log, TEXT("Task_DropDown: Starting drop of %f units"), DropHeight);
    return true;
}

void UTask_DropDown::UpdateTask(float DeltaTime)
{
    if (!OwnerPed || !bIsDropping)
    {
        CompleteTask(false, TEXT("Invalid state"));
        return;
    }

    // Simulate dropping motion
    FVector CurrentLocation = OwnerPed->GetActorLocation();
    float DropSpeed = 500.0f; // units per second
    FVector NewLocation = FMath::VInterpTo(CurrentLocation, LandingLocation, DeltaTime, DropSpeed);
    OwnerPed->SetActorLocation(NewLocation);

    // Calculate progress
    float DropDistance = FVector::Dist(StartLocation, CurrentLocation);
    DropProgress = FMath::Clamp(DropDistance / DropHeight, 0.0f, 1.0f);

    // Check if drop is complete
    float DistanceToLanding = FVector::Dist(CurrentLocation, LandingLocation);
    if (DistanceToLanding <= 10.0f)
    {
        OwnerPed->SetActorLocation(LandingLocation);
        bIsDropping = false;
        CompleteTask(true, TEXT("Drop completed"));
        return;
    }

    Super::UpdateTask(DeltaTime);
}

bool UTask_DropDown::ValidateTaskConditions() const
{
    if (!Super::ValidateTaskConditions())
    {
        return false;
    }

    // Check if ped is in ledge grab state (this would be expanded with actual state checking)
    return true;
}

// =====================================================
// UTask_Jump Implementation
// =====================================================

UTask_Jump::UTask_Jump()
{
    TaskName = TEXT("Jump");
    TaskDescription = TEXT("Jump over obstacle or to target");
    OneShotDuration = 2.0f;
    JumpHeight = 200.0f; // 2 meters default
    JumpForce = 500.0f;
    bCheckLandingSafety = true;
    bIsJumping = false;
    JumpProgress = 0.0f;
}

bool UTask_Jump::PerformOneShotAction()
{
    if (!OwnerPed)
    {
        return false;
    }

    StartLocation = OwnerPed->GetActorLocation();
    
    // Calculate jump trajectory
    FVector Direction = (JumpTarget - StartLocation).GetSafeNormal();
    JumpVelocity = Direction * JumpForce + FVector(0, 0, JumpHeight);
    
    // Start jump
    bIsJumping = true;
    
    UE_LOG(LogTemp, Log, TEXT("Task_Jump: Starting jump from %s to %s"), 
           *StartLocation.ToString(), *JumpTarget.ToString());
    
    return true;
}

void UTask_Jump::UpdateTask(float DeltaTime)
{
    if (bIsJumping && OwnerPed)
    {
        // Simple parabolic movement
        JumpProgress += DeltaTime / OneShotDuration;
        
        if (JumpProgress >= 1.0f)
        {
            // Landing
            OwnerPed->SetActorLocation(JumpTarget);
            bIsJumping = false;
            CompleteTask(true, TEXT("Jump completed"));
            return;
        }
        
        // Calculate current position along jump arc
        FVector CurrentPos = FMath::Lerp(StartLocation, JumpTarget, JumpProgress);
        float ArcHeight = FMath::Sin(JumpProgress * PI) * JumpHeight;
        CurrentPos.Z += ArcHeight;
        
        OwnerPed->SetActorLocation(CurrentPos);
    }

    Super::UpdateTask(DeltaTime);
}

bool UTask_Jump::ValidateTaskConditions() const
{
    if (!Super::ValidateTaskConditions())
    {
        return false;
    }

    // Check if jump distance is reasonable
    float JumpDistance = FVector::Dist(OwnerPed->GetActorLocation(), JumpTarget);
    if (JumpDistance > 500.0f) // Max 5 meter jump
    {
        return false;
    }

    // Check landing safety if enabled
    if (bCheckLandingSafety)
    {
        // Simple ground check
        FHitResult HitResult;
        FVector TraceStart = JumpTarget + FVector(0, 0, 100);
        FVector TraceEnd = JumpTarget - FVector(0, 0, 100);
        
        if (OwnerPed->GetWorld()->LineTraceSingleByChannel(HitResult, TraceStart, TraceEnd, ECC_WorldStatic))
        {
            return true; // Ground found
        }
        return false; // No safe landing
    }

    return true;
}

// =====================================================
// UTask_MoveTowards Implementation
// =====================================================

UTask_MoveTowards::UTask_MoveTowards()
{
    TaskName = TEXT("MoveTowards");
    TaskDescription = TEXT("Move towards target location or actor");
    OneShotDuration = 5.0f; // Allow up to 5 seconds for movement
    MovementSpeed = 300.0f; // 3 m/s default speed
    StopDistance = 100.0f; // Stop 1 meter from target
    bUsePathfinding = false;
    bIsMoving = false;
    MovementProgress = 0.0f;
    TargetActor = nullptr;
}

bool UTask_MoveTowards::PerformOneShotAction()
{
    if (!OwnerPed)
    {
        return false;
    }

    StartLocation = OwnerPed->GetActorLocation();
    
    // Update target location if following an actor
    if (TargetActor)
    {
        TargetLocation = TargetActor->GetActorLocation();
    }
    
    // Check if already at target
    float DistanceToTarget = FVector::Dist(StartLocation, TargetLocation);
    if (DistanceToTarget <= StopDistance)
    {
        CompleteTask(true, TEXT("Already at target location"));
        return true;
    }
    
    bIsMoving = true;
    
    UE_LOG(LogTemp, Log, TEXT("Task_MoveTowards: Moving from %s to %s"), 
           *StartLocation.ToString(), *TargetLocation.ToString());
    
    return true;
}

void UTask_MoveTowards::UpdateTask(float DeltaTime)
{
    if (bIsMoving && OwnerPed)
    {
        // Update target if following an actor
        if (TargetActor)
        {
            TargetLocation = TargetActor->GetActorLocation();
        }
        
        FVector CurrentLocation = OwnerPed->GetActorLocation();
        FVector Direction = (TargetLocation - CurrentLocation).GetSafeNormal();
        float DistanceToTarget = FVector::Dist(CurrentLocation, TargetLocation);
        
        // Check if reached target
        if (DistanceToTarget <= StopDistance)
        {
            bIsMoving = false;
            CompleteTask(true, TEXT("Reached target location"));
            return;
        }
        
        // Move towards target
        FVector MovementThisFrame = Direction * MovementSpeed * DeltaTime;
        FVector NewLocation = CurrentLocation + MovementThisFrame;
        
        // Simple movement (without pathfinding for now)
        OwnerPed->SetActorLocation(NewLocation);
        
        // Face movement direction
        if (!Direction.IsNearlyZero())
        {
            FRotator NewRotation = Direction.Rotation();
            OwnerPed->SetActorRotation(FMath::RInterpTo(OwnerPed->GetActorRotation(), NewRotation, DeltaTime, 5.0f));
        }
    }

    Super::UpdateTask(DeltaTime);
}

bool UTask_MoveTowards::ValidateTaskConditions() const
{
    if (!Super::ValidateTaskConditions())
    {
        return false;
    }

    // Validate target location
    if (TargetActor)
    {
        return IsValid(TargetActor);
    }
    
    // Check if target location is reasonable
    return !TargetLocation.IsZero();
}
