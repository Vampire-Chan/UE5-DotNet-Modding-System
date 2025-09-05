#pragma once

#include "CoreMinimal.h"
#include "../../BaseTask.h"
#include "OneShotTask.generated.h"

/**
 * OneShotTask - Base class for simple, immediate tasks
 * These tasks complete quickly and don't require complex state management
 */
UCLASS(BlueprintType, Blueprintable, Abstract)
class GAME_API UOneShotTask : public UBaseTask
{
    GENERATED_BODY()

public:
    UOneShotTask();

protected:
    virtual bool ExecuteTask() override;
    virtual void UpdateTask(float DeltaTime) override;

    // OneShot specific function - implement the immediate action
    virtual bool PerformOneShotAction() { return true; }

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "OneShot Config")
    bool bInstantComplete;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "OneShot Config")
    float OneShotDuration;
};

/**
 * Task_Aim - Aim at a target
 */
UCLASS()
class GAME_API UTask_Aim : public UOneShotTask
{
    GENERATED_BODY()

public:
    UTask_Aim();

    // Public setter methods for TaskManager access
    void SetAimDuration(float Duration) { AimDuration = Duration; }

protected:
    virtual bool PerformOneShotAction() override;
    virtual void UpdateTask(float DeltaTime) override;
    virtual bool ValidateTaskConditions() const override;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Aim Config")
    FVector AimDirection;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Aim Config")
    float AimSpeed;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Aim Config")
    float AimAccuracy;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Aim Config")
    bool bUsePredictiveAiming;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Aim Config")
    float AimDuration;

private:
    FVector CurrentAimDirection;
    float AimProgress;
};

/**
 * Task_LookAt - Look at a target or direction
 */
UCLASS()
class GAME_API UTask_LookAt : public UOneShotTask
{
    GENERATED_BODY()

public:
    UTask_LookAt();

    // Public setter methods for TaskManager access
    void SetLookDirection(const FVector& Direction) { LookDirection = Direction; }
    void SetLookAtActor(bool bLookActor) { bLookAtActor = bLookActor; }
    void SetLookSpeed(float Speed) { LookSpeed = Speed; }
    void SetOnlyHorizontal(bool bHorizontalOnly) { bOnlyHorizontal = bHorizontalOnly; }
    void SetLookDuration(float Duration) { LookDuration = Duration; }

protected:
    virtual bool PerformOneShotAction() override;
    virtual void UpdateTask(float DeltaTime) override;
    virtual bool ValidateTaskConditions() const override;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "LookAt Config")
    FVector LookDirection;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "LookAt Config")
    float LookSpeed;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "LookAt Config")
    bool bLookAtActor;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "LookAt Config")
    bool bOnlyHorizontal;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "LookAt Config")
    float LookDuration;

private:
    FRotator TargetRotation;
    FRotator StartRotation;
    float LookProgress;
};

/**
 * Task_Turn - Turn to face a direction
 */
UCLASS()
class GAME_API UTask_Turn : public UOneShotTask
{
    GENERATED_BODY()

public:
    UTask_Turn();

    // Public setter methods for TaskManager access
    void SetTargetRotation(const FRotator& Rotation) { TargetRotation = Rotation; }
    void SetTargetDirection(const FVector& Direction) { TargetRotation = Direction.Rotation(); }
    void SetTurnSpeed(float Speed) { TurnSpeed = Speed; }
    void SetTurnToFaceActor(bool bFaceActor) { bTurnToFaceActor = bFaceActor; }

protected:
    virtual bool PerformOneShotAction() override;
    virtual void UpdateTask(float DeltaTime) override;
    virtual bool ValidateTaskConditions() const override;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Turn Config")
    FRotator TargetRotation;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Turn Config")
    float TurnSpeed;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Turn Config")
    bool bTurnToFaceActor;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Turn Config")
    float TurnTolerance;

private:
    FRotator StartRotation;
    float TurnProgress;
    float TotalTurnAngle;
};

/**
 * Task_Shimmy - Side-step movement along cover
 */
UCLASS()
class GAME_API UTask_Shimmy : public UOneShotTask
{
    GENERATED_BODY()

public:
    UTask_Shimmy();

    // Public setter methods for TaskManager access
    void SetShimmyDirection(const FVector& Direction) { ShimmyDirection = Direction; }
    void SetShimmyDistance(float Distance) { ShimmyDistance = Distance; }

protected:
    virtual bool PerformOneShotAction() override;
    virtual void UpdateTask(float DeltaTime) override;
    virtual bool ValidateTaskConditions() const override;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Shimmy Config")
    FVector ShimmyDirection;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Shimmy Config")
    float ShimmyDistance;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Shimmy Config")
    float ShimmySpeed;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Shimmy Config")
    bool bCheckForObstacles;

private:
    FVector StartLocation;
    FVector TargetLocation;
    float ShimmyProgress;
};

/**
 * Task_DropDown - Drop down from ledge grab state
 */
UCLASS()
class GAME_API UTask_DropDown : public UOneShotTask
{
    GENERATED_BODY()

public:
    UTask_DropDown();

    // Public setter methods for TaskManager access  
    void SetDropHeight(float Height) { DropHeight = Height; }

protected:
    virtual bool PerformOneShotAction() override;
    virtual void UpdateTask(float DeltaTime) override;
    virtual bool ValidateTaskConditions() const override;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "DropDown Config")
    float DropHeight;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "DropDown Config")
    bool bCheckLandingSafety;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "DropDown Config")
    float SafeLandingDistance;

private:
    FVector StartLocation;
    FVector LandingLocation;
    bool bIsDropping;
    float DropProgress;
};

/**
 * Task_Jump - Jump over obstacles or to reach higher ground
 */
UCLASS()
class GAME_API UTask_Jump : public UOneShotTask
{
    GENERATED_BODY()

public:
    UTask_Jump();

    // Public setter methods for TaskManager access
    void SetJumpTarget(const FVector& Target) { JumpTarget = Target; }
    void SetJumpHeight(float Height) { JumpHeight = Height; }
    void SetJumpForce(float Force) { JumpForce = Force; }

protected:
    virtual bool PerformOneShotAction() override;
    virtual void UpdateTask(float DeltaTime) override;
    virtual bool ValidateTaskConditions() const override;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Jump Config")
    FVector JumpTarget;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Jump Config")
    float JumpHeight;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Jump Config")
    float JumpForce;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Jump Config")
    bool bCheckLandingSafety;

private:
    FVector StartLocation;
    bool bIsJumping;
    float JumpProgress;
    FVector JumpVelocity;
};

/**
 * Task_MoveTowards - Move towards a target location or actor
 */
UCLASS()
class GAME_API UTask_MoveTowards : public UOneShotTask
{
    GENERATED_BODY()

public:
    UTask_MoveTowards();

    // Public setter methods for TaskManager access
    void SetTargetLocation(const FVector& Location) { TargetLocation = Location; }
    void SetTargetActor(AActor* Actor) { TargetActor = Actor; }
    void SetMovementSpeed(float Speed) { MovementSpeed = Speed; }
    void SetStopDistance(float Distance) { StopDistance = Distance; }

protected:
    virtual bool PerformOneShotAction() override;
    virtual void UpdateTask(float DeltaTime) override;
    virtual bool ValidateTaskConditions() const override;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Movement Config")
    FVector TargetLocation;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Movement Config")
    AActor* TargetActor;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Movement Config")
    float MovementSpeed;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Movement Config")
    float StopDistance;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Movement Config")
    bool bUsePathfinding;

private:
    FVector StartLocation;
    bool bIsMoving;
    float MovementProgress;
    FVector CurrentVelocity;
};
