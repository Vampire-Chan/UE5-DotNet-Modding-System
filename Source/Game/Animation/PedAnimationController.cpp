#include "PedAnimationController.h"
#include "GameFramework/Character.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Components/SkeletalMeshComponent.h"
#include "Animation/AnimSequence.h"
#include "Engine/Engine.h"
#include "Kismet/KismetMathLibrary.h"

UPedAnimationController::UPedAnimationController()
{
    // Initialize default values
    CurrentMovementState = EPedMovementState::Idle;
    CurrentStanceState = EPedStanceState::Standing;
    CurrentTurnDirection = ETurnDirection::None;
    
    Speed = 0.0f;
    Direction = 0.0f;
    Velocity = FVector::ZeroVector;
    bIsInAir = false;
    bIsCrouching = false;
    bIsAiming = false;
    TurnRate = 0.0f;
    LeanAngle = 0.0f;
    
    bCanCancelCurrentAnimation = true;
    CurrentAnimationBlendWeight = 1.0f;
    CurrentAnimationName = TEXT("None");
    
    BlendTimer = 0.0f;
    BlendDuration = 0.15f;
    bIsBlending = false;
    
    PreviousMovementState = EPedMovementState::Idle;
    PreviousStanceState = EPedStanceState::Standing;
    
    PreviousYaw = 0.0f;
    YawDelta = 0.0f;
    TurnThreshold = 15.0f; // Degrees threshold for turn detection
}

void UPedAnimationController::NativeInitializeAnimation()
{
    Super::NativeInitializeAnimation();
    
    // Get character reference
    OwnerCharacter = Cast<ACharacter>(GetOwningActor());
    
    if (OwnerCharacter)
    {
        // Initialize animation system
        LoadAllAnimations();
        InitializeAnimationData();
        
        UE_LOG(LogTemp, Log, TEXT("PedAnimationController: Initialized for character %s"), 
               *OwnerCharacter->GetName());
    }
}

void UPedAnimationController::NativeUpdateAnimation(float DeltaTimeX)
{
    Super::NativeUpdateAnimation(DeltaTimeX);
    
    if (!OwnerCharacter)
        return;
    
    // Update all movement variables
    UpdateMovementVariables();
    
    // Handle turn detection
    HandleTurnDetection();
    
    // Update animation states based on movement
    UpdateAnimationStates();
    
    // Handle state transitions
    HandleStateTransitions();
    
    // Update blending
    UpdateBlending(DeltaTimeX);
}

void UPedAnimationController::UpdateMovementVariables()
{
    if (!OwnerCharacter || !OwnerCharacter->GetCharacterMovement())
        return;
    
    UCharacterMovementComponent* Movement = OwnerCharacter->GetCharacterMovement();
    
    // Get velocity and speed
    Velocity = Movement->Velocity;
    Speed = Velocity.Size();
    
    // Calculate movement direction relative to character
    if (Speed > 1.0f)
    {
        FVector ForwardVector = OwnerCharacter->GetActorForwardVector();
        FVector VelocityNormalized = Velocity.GetSafeNormal();
        Direction = FMath::Atan2(
            FVector::CrossProduct(ForwardVector, VelocityNormalized).Z,
            FVector::DotProduct(ForwardVector, VelocityNormalized)
        ) * 180.0f / PI;
    }
    else
    {
        Direction = 0.0f;
    }
    
    // Update movement flags
    bIsInAir = Movement->IsFalling();
    bIsCrouching = Movement->IsCrouching();
    
    // Calculate lean angle for turns
    if (Speed > 100.0f) // Only lean when moving at decent speed
    {
        LeanAngle = FMath::Clamp(TurnRate * 0.5f, -30.0f, 30.0f);
    }
    else
    {
        LeanAngle = FMath::FInterpTo(LeanAngle, 0.0f, GetWorld()->GetDeltaSeconds(), 5.0f);
    }
}

void UPedAnimationController::HandleTurnDetection()
{
    if (!OwnerCharacter)
        return;
    
    float CurrentYaw = OwnerCharacter->GetActorRotation().Yaw;
    YawDelta = FMath::FindDeltaAngleDegrees(PreviousYaw, CurrentYaw);
    
    // Calculate turn rate (degrees per second)
    float DeltaTime = GetWorld()->GetDeltaSeconds();
    if (DeltaTime > 0.0f)
    {
        TurnRate = YawDelta / DeltaTime;
    }
    
    // Determine turn direction
    if (FMath::Abs(YawDelta) > TurnThreshold * DeltaTime)
    {
        if (YawDelta > 0.0f)
        {
            CurrentTurnDirection = ETurnDirection::Right;
        }
        else
        {
            CurrentTurnDirection = ETurnDirection::Left;
        }
        
        // Check for 180+ degree turns
        if (FMath::Abs(YawDelta) > 90.0f * DeltaTime)
        {
            CurrentTurnDirection = ETurnDirection::Around180;
        }
    }
    else
    {
        CurrentTurnDirection = ETurnDirection::None;
    }
    
    PreviousYaw = CurrentYaw;
}

void UPedAnimationController::UpdateAnimationStates()
{
    // Store previous states
    PreviousMovementState = CurrentMovementState;
    PreviousStanceState = CurrentStanceState;
    
    // Update stance state
    if (bIsCrouching)
    {
        CurrentStanceState = EPedStanceState::Crouched;
    }
    else if (bIsAiming)
    {
        CurrentStanceState = EPedStanceState::Combat;
    }
    else
    {
        CurrentStanceState = EPedStanceState::Standing;
    }
    
    // Update movement state based on speed and conditions
    if (bIsInAir)
    {
        CurrentMovementState = EPedMovementState::Jumping;
    }
    else if (Speed < 1.0f)
    {
        CurrentMovementState = EPedMovementState::Idle;
    }
    else if (Speed < 150.0f) // Walk threshold
    {
        CurrentMovementState = EPedMovementState::Walking;
    }
    else if (Speed < 400.0f) // Run threshold
    {
        CurrentMovementState = EPedMovementState::Running;
    }
    else // Sprint
    {
        CurrentMovementState = EPedMovementState::Sprinting;
    }
    
    // Handle turning state
    if (CurrentTurnDirection != ETurnDirection::None && Speed > 50.0f)
    {
        // Only show turning state if we're moving and turning significantly
        if (FMath::Abs(TurnRate) > 60.0f) // Degrees per second threshold
        {
            CurrentMovementState = EPedMovementState::Turning;
        }
    }
}

void UPedAnimationController::HandleStateTransitions()
{
    // Handle transitions between movement states
    if (PreviousMovementState != CurrentMovementState)
    {
        switch (CurrentMovementState)
        {
            case EPedMovementState::Walking:
                if (PreviousMovementState == EPedMovementState::Idle)
                {
                    // Start walking animation
                    FAnimationData StartAnim = GetBestStartAnimation(CurrentMovementState, YawDelta);
                    PlayAnimation(StartAnim);
                }
                break;
                
            case EPedMovementState::Running:
                if (PreviousMovementState == EPedMovementState::Walking)
                {
                    // Transition from walk to run
                    FAnimationData StartAnim = GetBestStartAnimation(CurrentMovementState, YawDelta);
                    PlayAnimation(StartAnim);
                }
                else if (PreviousMovementState == EPedMovementState::Idle)
                {
                    // Start running from idle
                    FAnimationData StartAnim = GetBestStartAnimation(CurrentMovementState, YawDelta);
                    PlayAnimation(StartAnim);
                }
                break;
                
            case EPedMovementState::Sprinting:
            {
                // Always play sprint start animation
                FAnimationData StartAnim = GetBestStartAnimation(CurrentMovementState, YawDelta);
                PlayAnimation(StartAnim);
                break;
            }
                
            case EPedMovementState::Idle:
            {
                // Play stop animation based on previous movement
                if (PreviousMovementState == EPedMovementState::Walking)
                {
                    FAnimationData StopAnim = GetBestStopAnimation(PreviousMovementState, ShouldUseLeftFootVariant());
                    PlayAnimation(StopAnim);
                }
                else if (PreviousMovementState == EPedMovementState::Running)
                {
                    FAnimationData StopAnim = GetBestStopAnimation(PreviousMovementState, ShouldUseLeftFootVariant());
                    PlayAnimation(StopAnim);
                }
                else if (PreviousMovementState == EPedMovementState::Sprinting)
                {
                    FAnimationData StopAnim = GetBestStopAnimation(PreviousMovementState, ShouldUseLeftFootVariant());
                    PlayAnimation(StopAnim);
                }
                break;
            }
                
            case EPedMovementState::Turning:
            {
                // Play turn animation
                FAnimationData TurnAnim = GetBestTurnAnimation(PreviousMovementState, YawDelta);
                PlayAnimation(TurnAnim);
                break;
            }
                
            case EPedMovementState::Jumping:
            {
                // Play jump takeoff
                FAnimationData JumpAnim = GetBestJumpAnimation(true, false, false, ShouldUseLeftFootVariant());
                PlayAnimation(JumpAnim);
                break;
            }
        }
    }
    
    // Handle stance transitions
    if (PreviousStanceState != CurrentStanceState)
    {
        if (CurrentStanceState == EPedStanceState::Crouched && PreviousStanceState == EPedStanceState::Standing)
        {
            // Transition to crouch
            PlayAnimation(CrouchAnims.IdleToCrouch);
        }
        else if (CurrentStanceState == EPedStanceState::Standing && PreviousStanceState == EPedStanceState::Crouched)
        {
            // Transition from crouch
            PlayAnimation(CrouchAnims.CrouchToIdle);
        }
    }
}

void UPedAnimationController::UpdateBlending(float DeltaTime)
{
    if (bIsBlending)
    {
        BlendTimer += DeltaTime;
        
        if (BlendTimer >= BlendDuration)
        {
            bIsBlending = false;
            BlendTimer = 0.0f;
            CurrentAnimationBlendWeight = 1.0f;
        }
        else
        {
            CurrentAnimationBlendWeight = BlendTimer / BlendDuration;
        }
    }
}

void UPedAnimationController::PlayAnimation(const FAnimationData& AnimData, bool bForcePlay)
{
    if (!IsAnimationValid(AnimData))
        return;
    
    if (!bForcePlay && !CanPlayAnimation())
        return;
    
    // Start blending
    bIsBlending = true;
    BlendTimer = 0.0f;
    BlendDuration = AnimData.BlendInTime;
    CurrentAnimationBlendWeight = 0.0f;
    
    // Update animation info
    if (AnimData.AnimSequence)
    {
        CurrentAnimationName = AnimData.AnimSequence->GetName();
        bCanCancelCurrentAnimation = AnimData.bCanBeCancelled;
        
        UE_LOG(LogTemp, Log, TEXT("PedAnimationController: Playing animation %s"), 
               *CurrentAnimationName);
    }
}

void UPedAnimationController::CancelCurrentAnimation()
{
    if (bCanCancelCurrentAnimation)
    {
        bIsBlending = false;
        BlendTimer = 0.0f;
        CurrentAnimationBlendWeight = 1.0f;
        CurrentAnimationName = TEXT("None");
        bCanCancelCurrentAnimation = true;
        
        UE_LOG(LogTemp, Log, TEXT("PedAnimationController: Animation cancelled"));
    }
}

bool UPedAnimationController::CanPlayAnimation() const
{
    return bCanCancelCurrentAnimation || !bIsBlending;
}

void UPedAnimationController::SetMovementState(EPedMovementState NewState)
{
    if (CurrentMovementState != NewState)
    {
        UE_LOG(LogTemp, Log, TEXT("PedAnimationController: Movement state changed from %d to %d"), 
               (int32)CurrentMovementState, (int32)NewState);
        CurrentMovementState = NewState;
    }
}

void UPedAnimationController::SetStanceState(EPedStanceState NewState)
{
    if (CurrentStanceState != NewState)
    {
        UE_LOG(LogTemp, Log, TEXT("PedAnimationController: Stance state changed from %d to %d"), 
               (int32)CurrentStanceState, (int32)NewState);
        CurrentStanceState = NewState;
    }
}

FAnimationData UPedAnimationController::GetBestStartAnimation(EPedMovementState MovementState, float TurnAngle)
{
    TArray<FAnimationData> StartAnims;
    
    switch (MovementState)
    {
        case EPedMovementState::Walking:
            StartAnims = MovementAnims.WalkStarts;
            break;
        case EPedMovementState::Running:
            StartAnims = MovementAnims.RunStarts;
            break;
        case EPedMovementState::Sprinting:
            // Use run starts for sprint (no specific sprint starts in our data)
            StartAnims = MovementAnims.RunStarts;
            break;
        default:
            // Return basic walk start
            if (MovementAnims.WalkStarts.Num() > 0)
                return MovementAnims.WalkStarts[0];
            break;
    }
    
    // Select best animation based on turn angle
    if (StartAnims.Num() > 0)
    {
        if (FMath::Abs(TurnAngle) < 45.0f)
        {
            // Forward start (index 0 should be basic start)
            return StartAnims[0];
        }
        else if (TurnAngle > 45.0f && TurnAngle < 135.0f)
        {
            // Right turn start
            for (const FAnimationData& Anim : StartAnims)
            {
                if (Anim.AnimSequence && Anim.AnimSequence->GetName().Contains(TEXT("r_90")))
                    return Anim;
            }
        }
        else if (TurnAngle < -45.0f && TurnAngle > -135.0f)
        {
            // Left turn start
            for (const FAnimationData& Anim : StartAnims)
            {
                if (Anim.AnimSequence && Anim.AnimSequence->GetName().Contains(TEXT("l_90")))
                    return Anim;
            }
        }
        else
        {
            // 180 degree turn
            for (const FAnimationData& Anim : StartAnims)
            {
                if (Anim.AnimSequence && Anim.AnimSequence->GetName().Contains(TEXT("180")))
                    return Anim;
            }
        }
        
        // Fallback to first animation
        return StartAnims[0];
    }
    
    return FAnimationData();
}

FAnimationData UPedAnimationController::GetBestStopAnimation(EPedMovementState MovementState, bool bLeftFoot)
{
    TArray<FAnimationData> StopAnims;
    
    switch (MovementState)
    {
        case EPedMovementState::Walking:
            StopAnims = MovementAnims.WalkStops;
            break;
        case EPedMovementState::Running:
            StopAnims = MovementAnims.RunStops;
            break;
        case EPedMovementState::Sprinting:
            StopAnims = MovementAnims.SprintStops;
            break;
        default:
            break;
    }
    
    // Select left or right foot variant
    for (const FAnimationData& Anim : StopAnims)
    {
        if (Anim.AnimSequence)
        {
            FString AnimName = Anim.AnimSequence->GetName();
            if (bLeftFoot && AnimName.Contains(TEXT("_l")))
                return Anim;
            else if (!bLeftFoot && AnimName.Contains(TEXT("_r")))
                return Anim;
        }
    }
    
    // Fallback to first available
    if (StopAnims.Num() > 0)
        return StopAnims[0];
    
    return FAnimationData();
}

FAnimationData UPedAnimationController::GetBestTurnAnimation(EPedMovementState MovementState, float TurnAngle)
{
    TArray<FAnimationData> TurnAnims;
    
    switch (MovementState)
    {
        case EPedMovementState::Walking:
            TurnAnims = MovementAnims.WalkTurns;
            break;
        case EPedMovementState::Running:
            TurnAnims = MovementAnims.RunTurns;
            break;
        case EPedMovementState::Sprinting:
            TurnAnims = MovementAnims.SprintTurns;
            break;
        default:
            TurnAnims = MovementAnims.WalkTurns;
            break;
    }
    
    // Check for 360 degree turn
    if (FMath::Abs(TurnAngle) > 270.0f)
    {
        for (const FAnimationData& Anim : MovementAnims.Full360Turns)
        {
            if (Anim.AnimSequence)
            {
                FString AnimName = Anim.AnimSequence->GetName();
                if (TurnAngle > 0.0f && AnimName.Contains(TEXT("_r")))
                    return Anim;
                else if (TurnAngle < 0.0f && AnimName.Contains(TEXT("_l")))
                    return Anim;
            }
        }
    }
    
    // Select appropriate turn animation
    for (const FAnimationData& Anim : TurnAnims)
    {
        if (Anim.AnimSequence)
        {
            FString AnimName = Anim.AnimSequence->GetName();
            
            if (FMath::Abs(TurnAngle) > 135.0f)
            {
                // 180 degree turn
                if (AnimName.Contains(TEXT("180")))
                {
                    if (TurnAngle > 0.0f && AnimName.Contains(TEXT("_r")))
                        return Anim;
                    else if (TurnAngle < 0.0f && AnimName.Contains(TEXT("_l")))
                        return Anim;
                }
            }
            else if (TurnAngle > 45.0f && AnimName.Contains(TEXT("_r")) && !AnimName.Contains(TEXT("180")))
            {
                // Right turn
                return Anim;
            }
            else if (TurnAngle < -45.0f && AnimName.Contains(TEXT("_l")) && !AnimName.Contains(TEXT("180")))
            {
                // Left turn
                return Anim;
            }
        }
    }
    
    // Fallback
    if (TurnAnims.Num() > 0)
        return TurnAnims[0];
    
    return FAnimationData();
}

FAnimationData UPedAnimationController::GetBestJumpAnimation(bool bTakeoff, bool bInAir, bool bLanding, bool bLeftFoot)
{
    if (bTakeoff)
    {
        for (const FAnimationData& Anim : JumpAnims.JumpTakeoffs)
        {
            if (Anim.AnimSequence)
            {
                FString AnimName = Anim.AnimSequence->GetName();
                if (bLeftFoot && AnimName.Contains(TEXT("_l")))
                    return Anim;
                else if (!bLeftFoot && AnimName.Contains(TEXT("_r")))
                    return Anim;
            }
        }
        
        return JumpAnims.JumpOnSpot;
    }
    else if (bInAir)
    {
        return SelectRandomFromArray(JumpAnims.JumpInAir);
    }
    else if (bLanding)
    {
        // Random chance for special landing
        if (FMath::RandRange(0, 100) < 20) // 20% chance
        {
            if (FMath::RandBool())
                return JumpAnims.JumpLandRoll;
            else
                return JumpAnims.JumpLandSquat;
        }
        
        // Normal landing
        for (const FAnimationData& Anim : JumpAnims.JumpLandings)
        {
            if (Anim.AnimSequence)
            {
                FString AnimName = Anim.AnimSequence->GetName();
                if (bLeftFoot && AnimName.Contains(TEXT("_l")))
                    return Anim;
                else if (!bLeftFoot && AnimName.Contains(TEXT("_r")))
                    return Anim;
            }
        }
    }
    
    return FAnimationData();
}

ETurnDirection UPedAnimationController::CalculateTurnDirection(float TurnAngle)
{
    float AbsAngle = FMath::Abs(TurnAngle);
    
    if (AbsAngle < TurnThreshold)
        return ETurnDirection::None;
    else if (AbsAngle > 270.0f)
        return ETurnDirection::Full360;
    else if (AbsAngle > 135.0f)
        return ETurnDirection::Around180;
    else if (TurnAngle > 0.0f)
        return ETurnDirection::Right;
    else
        return ETurnDirection::Left;
}

float UPedAnimationController::GetMovementSpeedRatio() const
{
    if (!OwnerCharacter || !OwnerCharacter->GetCharacterMovement())
        return 0.0f;
    
    float MaxSpeed = OwnerCharacter->GetCharacterMovement()->MaxWalkSpeed;
    return FMath::Clamp(Speed / MaxSpeed, 0.0f, 1.0f);
}

bool UPedAnimationController::ShouldUseLeftFootVariant() const
{
    // Simple alternating system or random selection
    static bool bLastWasLeft = false;
    bLastWasLeft = !bLastWasLeft;
    return bLastWasLeft;
}

void UPedAnimationController::LoadAllAnimations()
{
    UE_LOG(LogTemp, Log, TEXT("PedAnimationController: Loading all animations..."));
    
    // Load movement animations
    // Note: These paths will need to be updated when .onim files are converted to .uasset
    
    // Basic movement
    MovementAnims.Idle.AnimSequence = LoadAnimationFromPath(TEXT("/Game/Content/Animations/player_movement/idle"));
    MovementAnims.Walk.AnimSequence = LoadAnimationFromPath(TEXT("/Game/Content/Animations/player_movement/walk"));
    MovementAnims.Run.AnimSequence = LoadAnimationFromPath(TEXT("/Game/Content/Animations/player_movement/run"));
    MovementAnims.Sprint.AnimSequence = LoadAnimationFromPath(TEXT("/Game/Content/Animations/player_movement/sprint"));
    
    // Directional movement
    MovementAnims.WalkBackward.AnimSequence = LoadAnimationFromPath(TEXT("/Game/Content/Animations/player_movement/walk_b"));
    MovementAnims.WalkStrafeLeft.AnimSequence = LoadAnimationFromPath(TEXT("/Game/Content/Animations/player_movement/walk_strafe_l"));
    MovementAnims.WalkStrafeRight.AnimSequence = LoadAnimationFromPath(TEXT("/Game/Content/Animations/player_movement/walk_strafe_r"));
    MovementAnims.RunStrafeLeft.AnimSequence = LoadAnimationFromPath(TEXT("/Game/Content/Animations/player_movement/run_strafe_l"));
    MovementAnims.RunStrafeRight.AnimSequence = LoadAnimationFromPath(TEXT("/Game/Content/Animations/player_movement/run_strafe_r"));
    MovementAnims.RunStrafeBackward.AnimSequence = LoadAnimationFromPath(TEXT("/Game/Content/Animations/player_movement/run_strafe_b"));
    
    // Load start animations
    TArray<FString> WalkStartNames = {
        TEXT("wstart"), TEXT("wstart_turn_l90"), TEXT("wstart_turn_r90"), 
        TEXT("wstart_turn_l180"), TEXT("wstart_turn_r180")
    };
    
    for (const FString& AnimName : WalkStartNames)
    {
        FAnimationData StartAnim;
        StartAnim.AnimSequence = LoadAnimationFromPath(FString::Printf(TEXT("/Game/Content/Animations/player_movement/%s"), *AnimName));
        StartAnim.bLooping = false;
        StartAnim.bCanBeCancelled = true;
        MovementAnims.WalkStarts.Add(StartAnim);
    }
    
    // Load all other animation categories...
    // (Similar pattern for all animation types)
    
    UE_LOG(LogTemp, Log, TEXT("PedAnimationController: Animation loading complete"));
}

UAnimSequence* UPedAnimationController::LoadAnimationFromPath(const FString& AnimationPath)
{
    // This will load the animation asset from the given path
    // The path should point to the converted .uasset files
    return LoadObject<UAnimSequence>(nullptr, *AnimationPath);
}

FAnimationData UPedAnimationController::SelectRandomFromArray(const TArray<FAnimationData>& AnimArray) const
{
    if (AnimArray.Num() > 0)
    {
        int32 RandomIndex = FMath::RandRange(0, AnimArray.Num() - 1);
        return AnimArray[RandomIndex];
    }
    
    return FAnimationData();
}

bool UPedAnimationController::IsAnimationValid(const FAnimationData& AnimData) const
{
    return AnimData.AnimSequence != nullptr;
}

void UPedAnimationController::InitializeAnimationData()
{
    // Set up default animation properties for all loaded animations
    
    // Set looping for movement animations
    MovementAnims.Idle.bLooping = true;
    MovementAnims.Walk.bLooping = true;
    MovementAnims.Run.bLooping = true;
    MovementAnims.Sprint.bLooping = true;
    MovementAnims.WalkBackward.bLooping = true;
    MovementAnims.WalkStrafeLeft.bLooping = true;
    MovementAnims.WalkStrafeRight.bLooping = true;
    MovementAnims.RunStrafeLeft.bLooping = true;
    MovementAnims.RunStrafeRight.bLooping = true;
    MovementAnims.RunStrafeBackward.bLooping = true;
    
    // Crouch animations
    CrouchAnims.CrouchIdle.bLooping = true;
    CrouchAnims.CrouchWalk.bLooping = true;
    CrouchAnims.CrouchRun.bLooping = true;
    
    // Transition animations are non-looping by default
    CrouchAnims.IdleToCrouch.bLooping = false;
    CrouchAnims.CrouchToIdle.bLooping = false;
    
    UE_LOG(LogTemp, Log, TEXT("PedAnimationController: Animation data initialization complete"));
}

EPedAnimationRegion UPedAnimationController::DetermineAnimationRegion() const
{
    if (bIsInVehicle)
    {
        return EPedAnimationRegion::InVehicle;
    }
    else if (bIsInCombat)
    {
        return EPedAnimationRegion::Combat;
    }
    else if (bIsCrouching)
    {
        return EPedAnimationRegion::Crouch;
    }
    else if (bIsInAir)
    {
        return EPedAnimationRegion::Jump;
    }
    else if (bIsInCover)
    {
        return EPedAnimationRegion::Cover;
    }
    else if (bIsInteracting)
    {
        return EPedAnimationRegion::Interaction;
    }
    else
    {
        return EPedAnimationRegion::OnFoot;
    }
}

void UPedAnimationController::SetAnimationRegion(EPedAnimationRegion NewRegion)
{
    if (CurrentAnimationRegion != NewRegion)
    {
        EPedAnimationRegion PreviousRegion = CurrentAnimationRegion;
        CurrentAnimationRegion = NewRegion;
        
        UE_LOG(LogTemp, Log, TEXT("PedAnimationController: Animation region changed from %d to %d"), 
               (int32)PreviousRegion, (int32)NewRegion);
    }
}

FString UPedAnimationController::GetRegionContextName(EPedAnimationRegion Region) const
{
    switch (Region)
    {
        case EPedAnimationRegion::OnFoot:
            return TEXT("OnFoot");
        case EPedAnimationRegion::Jump:
            return TEXT("Jump");
        case EPedAnimationRegion::Crouch:
            return TEXT("Crouch");
        case EPedAnimationRegion::Combat:
            return TEXT("Combat");
        case EPedAnimationRegion::Cover:
            return TEXT("Cover");
        case EPedAnimationRegion::InVehicle:
            return TEXT("InVehicle");
        case EPedAnimationRegion::Interaction:
            return TEXT("Interaction");
        case EPedAnimationRegion::Emote:
            return TEXT("Emote");
        default:
            return TEXT("OnFoot");
    }
}

void UPedAnimationController::UpdateVehicleAnimationSync()
{
    if (!bIsInVehicle || !OwnerCharacter)
    {
        return;
    }
    
    // Update vehicle-specific animation variables based on character state
    // This would typically sync with vehicle input state
    UE_LOG(LogTemp, VeryVerbose, TEXT("PedAnimationController: Updating vehicle animation sync"));
}

void UPedAnimationController::SetVehicleInputs(float SteeringInput, float ThrottleInput, float BrakeInput, int32 CurrentGear)
{
    if (!bIsInVehicle)
    {
        return;
    }
    
    // Store vehicle input data for animation blending
    // These would be used in animation blueprints for steering wheel, pedal animations, etc.
    UE_LOG(LogTemp, VeryVerbose, TEXT("PedAnimationController: Vehicle inputs - Steering: %f, Throttle: %f, Brake: %f, Gear: %d"), 
           SteeringInput, ThrottleInput, BrakeInput, CurrentGear);
}
