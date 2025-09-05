#include "PedInputComponent.h"
#include "GameFramework/Character.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Components/SkeletalMeshComponent.h"
#include "Engine/World.h"

UPedInputComponent::UPedInputComponent()
{
    PrimaryComponentTick.bCanEverTick = true;
    PrimaryComponentTick.bStartWithTickEnabled = true;

    // Movement speed defaults
    WalkSpeed = 150.0f;
    RunSpeed = 300.0f;
    SprintSpeed = 600.0f;
    CrouchSpeed = 100.0f;

    // Input state defaults
    MovementInput = FVector2D::ZeroVector;
    LookInput = FVector2D::ZeroVector;
    bIsMoving = false;
    bIsSprinting = false;
    bIsWalking = false;
    bIsCrouching = false;
    bIsAiming = false;
    bIsFiring = false;

    // Animation controller reference
    AnimationController = nullptr;

    // Internal state
    LastMovementState = EPedMovementState::Idle;
    LastStanceState = EPedStanceState::Standing;
    MovementMagnitude = 0.0f;
    LastMovementDirection = FVector::ZeroVector;

    // Timing
    SprintHoldTime = 0.0f;
    CrouchHoldTime = 0.0f;
    IdleTime = 0.0f;

    // Thresholds
    MovementDeadzone = 0.1f;
    RunThreshold = 0.5f;
    SprintThreshold = 0.8f;
    IdleThreshold = 1.0f; // Time to wait before transitioning to idle
}

void UPedInputComponent::BeginPlay()
{
    Super::BeginPlay();

    // Try to find animation controller on the owner
    if (AActor* Owner = GetOwner())
    {
        if (USkeletalMeshComponent* SkeletalMesh = Owner->FindComponentByClass<USkeletalMeshComponent>())
        {
            AnimationController = Cast<UPedAnimationController>(SkeletalMesh->GetAnimInstance());
        }
    }

    // Initialize input mapping context
    if (APlayerController* PlayerController = GetWorld()->GetFirstPlayerController())
    {
        if (UEnhancedInputLocalPlayerSubsystem* Subsystem = ULocalPlayer::GetSubsystem<UEnhancedInputLocalPlayerSubsystem>(PlayerController->GetLocalPlayer()))
        {
            if (DefaultMappingContext)
            {
                Subsystem->AddMappingContext(DefaultMappingContext, 0);
            }
        }
    }

    UE_LOG(LogTemp, Log, TEXT("PedInputComponent: Initialized for %s"), 
           GetOwner() ? *GetOwner()->GetName() : TEXT("Unknown"));
}

void UPedInputComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
    Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

    // Update movement magnitude
    MovementMagnitude = MovementInput.Length();
    bIsMoving = MovementMagnitude > MovementDeadzone;

    // Update timing
    if (bIsSprinting)
        SprintHoldTime += DeltaTime;
    else
        SprintHoldTime = 0.0f;

    if (bIsCrouching)
        CrouchHoldTime += DeltaTime;
    else
        CrouchHoldTime = 0.0f;

    if (!bIsMoving)
        IdleTime += DeltaTime;
    else
        IdleTime = 0.0f;

    // Update states
    UpdateMovementState();
    UpdateStanceState();

    // Send changes to animation controller
    SendStateChangesToAnimationController();
}

void UPedInputComponent::SetupInputComponent(UEnhancedInputComponent* EnhancedInputComponent)
{
    if (!EnhancedInputComponent)
    {
        UE_LOG(LogTemp, Error, TEXT("PedInputComponent: Enhanced Input Component is null"));
        return;
    }

    // Bind movement actions
    if (MoveAction)
    {
        EnhancedInputComponent->BindAction(MoveAction, ETriggerEvent::Triggered, this, &UPedInputComponent::HandleMoveInput);
        EnhancedInputComponent->BindAction(MoveAction, ETriggerEvent::Completed, this, &UPedInputComponent::HandleMoveInput);
    }

    if (LookAction)
    {
        EnhancedInputComponent->BindAction(LookAction, ETriggerEvent::Triggered, this, &UPedInputComponent::HandleLookInput);
    }

    // Bind action buttons
    if (JumpAction)
    {
        EnhancedInputComponent->BindAction(JumpAction, ETriggerEvent::Started, this, &UPedInputComponent::HandleJumpPressed);
        EnhancedInputComponent->BindAction(JumpAction, ETriggerEvent::Completed, this, &UPedInputComponent::HandleJumpReleased);
    }

    if (CrouchAction)
    {
        EnhancedInputComponent->BindAction(CrouchAction, ETriggerEvent::Started, this, &UPedInputComponent::HandleCrouchPressed);
        EnhancedInputComponent->BindAction(CrouchAction, ETriggerEvent::Completed, this, &UPedInputComponent::HandleCrouchReleased);
    }

    if (SprintAction)
    {
        EnhancedInputComponent->BindAction(SprintAction, ETriggerEvent::Started, this, &UPedInputComponent::HandleSprintPressed);
        EnhancedInputComponent->BindAction(SprintAction, ETriggerEvent::Completed, this, &UPedInputComponent::HandleSprintReleased);
    }

    if (WalkAction)
    {
        EnhancedInputComponent->BindAction(WalkAction, ETriggerEvent::Started, this, &UPedInputComponent::HandleWalkPressed);
        EnhancedInputComponent->BindAction(WalkAction, ETriggerEvent::Completed, this, &UPedInputComponent::HandleWalkReleased);
    }

    if (AimAction)
    {
        EnhancedInputComponent->BindAction(AimAction, ETriggerEvent::Started, this, &UPedInputComponent::HandleAimPressed);
        EnhancedInputComponent->BindAction(AimAction, ETriggerEvent::Completed, this, &UPedInputComponent::HandleAimReleased);
    }

    if (FireAction)
    {
        EnhancedInputComponent->BindAction(FireAction, ETriggerEvent::Started, this, &UPedInputComponent::HandleFirePressed);
        EnhancedInputComponent->BindAction(FireAction, ETriggerEvent::Completed, this, &UPedInputComponent::HandleFireReleased);
    }

    UE_LOG(LogTemp, Log, TEXT("PedInputComponent: Input actions bound successfully"));
}

void UPedInputComponent::SetAnimationController(UPedAnimationController* InAnimationController)
{
    AnimationController = InAnimationController;
    UE_LOG(LogTemp, Log, TEXT("PedInputComponent: Animation controller set"));
}

// Input Handlers
void UPedInputComponent::HandleMoveInput(const FInputActionValue& Value)
{
    FVector2D InputVector = Value.Get<FVector2D>();
    MovementInput = InputVector;

    // Apply movement to character if we have one
    if (ACharacter* Character = Cast<ACharacter>(GetOwner()))
    {
        if (InputVector != FVector2D::ZeroVector)
        {
            // Get forward and right vectors
            const FRotator Rotation = Character->GetControlRotation();
            const FRotator YawRotation(0, Rotation.Yaw, 0);
            
            const FVector ForwardDirection = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::X);
            const FVector RightDirection = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::Y);

            // Apply movement
            Character->AddMovementInput(ForwardDirection, InputVector.Y);
            Character->AddMovementInput(RightDirection, InputVector.X);

            LastMovementDirection = (ForwardDirection * InputVector.Y + RightDirection * InputVector.X).GetSafeNormal();
        }
    }
}

void UPedInputComponent::HandleLookInput(const FInputActionValue& Value)
{
    FVector2D LookAxisVector = Value.Get<FVector2D>();
    LookInput = LookAxisVector;

    if (APawn* Pawn = Cast<APawn>(GetOwner()))
    {
        // Add yaw and pitch input to the pawn
        Pawn->AddControllerYawInput(LookAxisVector.X);
        Pawn->AddControllerPitchInput(LookAxisVector.Y);
    }
}

void UPedInputComponent::HandleJumpPressed()
{
    if (ACharacter* Character = Cast<ACharacter>(GetOwner()))
    {
        Character->Jump();
    }
}

void UPedInputComponent::HandleJumpReleased()
{
    if (ACharacter* Character = Cast<ACharacter>(GetOwner()))
    {
        Character->StopJumping();
    }
}

void UPedInputComponent::HandleCrouchPressed()
{
    bIsCrouching = true;
    
    if (ACharacter* Character = Cast<ACharacter>(GetOwner()))
    {
        Character->Crouch();
    }
}

void UPedInputComponent::HandleCrouchReleased()
{
    bIsCrouching = false;
    
    if (ACharacter* Character = Cast<ACharacter>(GetOwner()))
    {
        Character->UnCrouch();
    }
}

void UPedInputComponent::HandleSprintPressed()
{
    bIsSprinting = true;
}

void UPedInputComponent::HandleSprintReleased()
{
    bIsSprinting = false;
}

void UPedInputComponent::HandleWalkPressed()
{
    bIsWalking = true;
}

void UPedInputComponent::HandleWalkReleased()
{
    bIsWalking = false;
}

void UPedInputComponent::HandleAimPressed()
{
    bIsAiming = true;
}

void UPedInputComponent::HandleAimReleased()
{
    bIsAiming = false;
}

void UPedInputComponent::HandleFirePressed()
{
    bIsFiring = true;
}

void UPedInputComponent::HandleFireReleased()
{
    bIsFiring = false;
}

// State Management
EPedMovementState UPedInputComponent::DetermineMovementState() const
{
    if (!bIsMoving && IdleTime > IdleThreshold)
    {
        return EPedMovementState::Idle;
    }

    if (bIsMoving)
    {
        if (bIsSprinting && ShouldTransitionToSprint())
        {
            return EPedMovementState::Sprinting;
        }
        else if (bIsWalking || ShouldTransitionToWalk())
        {
            return EPedMovementState::Walking;
        }
        else if (ShouldTransitionToRun())
        {
            return EPedMovementState::Running;
        }
        else
        {
            return EPedMovementState::Walking; // Default to walking
        }
    }

    return LastMovementState;
}

EPedStanceState UPedInputComponent::DetermineStanceState() const
{
    if (bIsCrouching)
    {
        return EPedStanceState::Crouched;
    }
    else if (bIsAiming || bIsFiring)
    {
        return EPedStanceState::Combat;
    }
    else
    {
        return EPedStanceState::Standing;
    }
}

float UPedInputComponent::GetCurrentMovementSpeed() const
{
    EPedMovementState CurrentState = DetermineMovementState();
    
    switch (CurrentState)
    {
        case EPedMovementState::Walking:
            return WalkSpeed;
        case EPedMovementState::Running:
            return RunSpeed;
        case EPedMovementState::Sprinting:
            return SprintSpeed;
        case EPedMovementState::Crouching:
            return CrouchSpeed;
        default:
            return 0.0f;
    }
}

FVector UPedInputComponent::GetMovementDirection() const
{
    return LastMovementDirection;
}

// Private helper functions
void UPedInputComponent::UpdateMovementState()
{
    EPedMovementState NewState = DetermineMovementState();
    
    if (NewState != LastMovementState)
    {
        LastMovementState = NewState;
        
        // Update character movement speed
        if (ACharacter* Character = Cast<ACharacter>(GetOwner()))
        {
            if (UCharacterMovementComponent* MovementComp = Character->GetCharacterMovement())
            {
                MovementComp->MaxWalkSpeed = GetCurrentMovementSpeed();
            }
        }
    }
}

void UPedInputComponent::UpdateStanceState()
{
    EPedStanceState NewState = DetermineStanceState();
    
    if (NewState != LastStanceState)
    {
        LastStanceState = NewState;
    }
}

void UPedInputComponent::SendStateChangesToAnimationController()
{
    if (!AnimationController)
    {
        return;
    }

    // Update movement state
    if (AnimationController->CurrentMovementState != LastMovementState)
    {
        AnimationController->SetMovementState(LastMovementState);
    }

    // Update stance state
    if (AnimationController->CurrentStanceState != LastStanceState)
    {
        AnimationController->SetStanceState(LastStanceState);
    }

    // Update movement variables
    AnimationController->SetMovementSpeed(MovementMagnitude * GetCurrentMovementSpeed());
    AnimationController->SetMovementDirection(FMath::Atan2(MovementInput.Y, MovementInput.X) * (180.0f / PI));
    AnimationController->SetIsInAir(false); // Will be set by character movement component
    AnimationController->SetIsCrouching(bIsCrouching);
    AnimationController->bIsInCombat = bIsAiming || bIsFiring;
    AnimationController->bIsAiming = bIsAiming;
}

bool UPedInputComponent::ShouldTransitionToSprint() const
{
    return MovementMagnitude >= SprintThreshold && bIsSprinting;
}

bool UPedInputComponent::ShouldTransitionToRun() const
{
    return MovementMagnitude >= RunThreshold && !bIsWalking;
}

bool UPedInputComponent::ShouldTransitionToWalk() const
{
    return MovementMagnitude >= MovementDeadzone && (bIsWalking || MovementMagnitude < RunThreshold);
}
