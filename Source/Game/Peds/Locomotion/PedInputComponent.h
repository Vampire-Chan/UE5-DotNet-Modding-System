#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "EnhancedInputComponent.h"
#include "EnhancedInputSubsystems.h"
#include "InputActionValue.h"
#include "Animation/PedAnimationController.h"
#include "PedInputComponent.generated.h"

class UInputMappingContext;
class UInputAction;

/**
 * Handles input detection and converts it to movement states for the animation system
 * This is the bridge between player input and animation states
 */
UCLASS(ClassGroup=(Custom), meta=(BlueprintSpawnableComponent))
class GAME_API UPedInputComponent : public UActorComponent
{
    GENERATED_BODY()

public:
    UPedInputComponent();

    // Input Actions (Set in Blueprint or Editor)
    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Input Actions")
    UInputAction* MoveAction;

    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Input Actions")
    UInputAction* LookAction;

    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Input Actions")
    UInputAction* JumpAction;

    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Input Actions")
    UInputAction* CrouchAction;

    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Input Actions")
    UInputAction* SprintAction;

    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Input Actions")
    UInputAction* WalkAction;

    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Input Actions")
    UInputAction* AimAction;

    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Input Actions")
    UInputAction* FireAction;

    // Input Mapping Context
    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Input Context")
    UInputMappingContext* DefaultMappingContext;

    // Movement Settings
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Movement Settings")
    float WalkSpeed;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Movement Settings")
    float RunSpeed;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Movement Settings")
    float SprintSpeed;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Movement Settings")
    float CrouchSpeed;

    // Input State
    UPROPERTY(BlueprintReadOnly, Category = "Input State")
    FVector2D MovementInput;

    UPROPERTY(BlueprintReadOnly, Category = "Input State")
    FVector2D LookInput;

    UPROPERTY(BlueprintReadOnly, Category = "Input State")
    bool bIsMoving;

    UPROPERTY(BlueprintReadOnly, Category = "Input State")
    bool bIsSprinting;

    UPROPERTY(BlueprintReadOnly, Category = "Input State")
    bool bIsWalking;

    UPROPERTY(BlueprintReadOnly, Category = "Input State")
    bool bIsCrouching;

    UPROPERTY(BlueprintReadOnly, Category = "Input State")
    bool bIsAiming;

    UPROPERTY(BlueprintReadOnly, Category = "Input State")
    bool bIsFiring;

    // Animation Controller Reference
    UPROPERTY(BlueprintReadOnly, Category = "Animation")
    UPedAnimationController* AnimationController;

    // Initialization
    UFUNCTION(BlueprintCallable, Category = "Input")
    void SetupInputComponent(UEnhancedInputComponent* EnhancedInputComponent);

    UFUNCTION(BlueprintCallable, Category = "Input")
    void SetAnimationController(UPedAnimationController* InAnimationController);

    // Input Handlers
    UFUNCTION()
    void HandleMoveInput(const FInputActionValue& Value);

    UFUNCTION()
    void HandleLookInput(const FInputActionValue& Value);

    UFUNCTION()
    void HandleJumpPressed();

    UFUNCTION()
    void HandleJumpReleased();

    UFUNCTION()
    void HandleCrouchPressed();

    UFUNCTION()
    void HandleCrouchReleased();

    UFUNCTION()
    void HandleSprintPressed();

    UFUNCTION()
    void HandleSprintReleased();

    UFUNCTION()
    void HandleWalkPressed();

    UFUNCTION()
    void HandleWalkReleased();

    UFUNCTION()
    void HandleAimPressed();

    UFUNCTION()
    void HandleAimReleased();

    UFUNCTION()
    void HandleFirePressed();

    UFUNCTION()
    void HandleFireReleased();

    // State Management
    UFUNCTION(BlueprintCallable, Category = "Input State")
    EPedMovementState DetermineMovementState() const;

    UFUNCTION(BlueprintCallable, Category = "Input State")
    EPedStanceState DetermineStanceState() const;

    UFUNCTION(BlueprintCallable, Category = "Input State")
    float GetCurrentMovementSpeed() const;

    UFUNCTION(BlueprintCallable, Category = "Input State")
    FVector GetMovementDirection() const;

protected:
    virtual void BeginPlay() override;
    virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

private:
    // Internal state tracking
    EPedMovementState LastMovementState;
    EPedStanceState LastStanceState;
    float MovementMagnitude;
    FVector LastMovementDirection;

    // Timing variables
    float SprintHoldTime;
    float CrouchHoldTime;
    float IdleTime;

    // State change thresholds
    UPROPERTY(EditAnywhere, Category = "Input Thresholds")
    float MovementDeadzone;

    UPROPERTY(EditAnywhere, Category = "Input Thresholds")
    float RunThreshold;

    UPROPERTY(EditAnywhere, Category = "Input Thresholds")
    float SprintThreshold;

    UPROPERTY(EditAnywhere, Category = "Input Thresholds")
    float IdleThreshold;

    // Helper functions
    void UpdateMovementState();
    void UpdateStanceState();
    void SendStateChangesToAnimationController();
    bool ShouldTransitionToSprint() const;
    bool ShouldTransitionToRun() const;
    bool ShouldTransitionToWalk() const;
};
