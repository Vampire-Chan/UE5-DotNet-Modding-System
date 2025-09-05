#pragma once

#include "CoreMinimal.h"
#include "../../BaseTask.h"
#include "ComplexTask.generated.h"

UENUM(BlueprintType)
enum class EComplexTaskPhase : uint8
{
    Preparation         UMETA(DisplayName = "Preparation"),
    Execution           UMETA(DisplayName = "Execution"),
    Completion          UMETA(DisplayName = "Completion"),
    Cleanup             UMETA(DisplayName = "Cleanup")
};

/**
 * ComplexTask - Base class for multi-phase tasks that require state management
 * These tasks have multiple phases and can take extended time to complete
 */
UCLASS(BlueprintType, Blueprintable, Abstract)
class GAME_API UComplexTask : public UBaseTask
{
    GENERATED_BODY()

public:
    UComplexTask();

protected:
    virtual bool ExecuteTask() override;
    virtual void UpdateTask(float DeltaTime) override;
    virtual void CleanupTask() override;

    // Complex task specific functions
    virtual bool PrepareTask() { return true; }
    virtual bool ExecutePhase(EComplexTaskPhase Phase, float DeltaTime) { return true; }
    /** Check if task can advance to the next phase */
    virtual bool CanAdvanceToNextPhase(EComplexTaskPhase InPhase) const { return true; }
    virtual void OnPhaseChanged(EComplexTaskPhase OldPhase, EComplexTaskPhase NewPhase) {}

    // Phase management
    void AdvanceToNextPhase();
    void SetCurrentPhase(EComplexTaskPhase NewPhase);

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Complex Config")
    bool bRequiresPreparation;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Complex Config")
    float PhaseTimeout;

    UPROPERTY(BlueprintReadOnly, Category = "Complex Runtime")
    EComplexTaskPhase CurrentPhase;

    UPROPERTY(BlueprintReadOnly, Category = "Complex Runtime")
    float PhaseTime;

private:
    bool bPhaseChanged;
};

/**
 * Task_Climb - Climb over obstacles or walls
 */
UCLASS()
class GAME_API UTask_Climb : public UComplexTask
{
    GENERATED_BODY()

public:
    UTask_Climb();

    // Public setter methods for TaskManager access
    void SetClimbTarget(const FVector& Target) { ClimbTarget = Target; }
    void SetClimbHeight(float Height) { ClimbHeight = Height; }
    void SetClimbSpeed(float Speed) { ClimbSpeed = Speed; }
    void SetAutoDetectClimbHeight(bool bAutoDetect) { bAutoDetectClimbHeight = bAutoDetect; }

protected:
    virtual bool PrepareTask() override;
    virtual bool ExecutePhase(EComplexTaskPhase Phase, float DeltaTime) override;
    virtual bool CanAdvanceToNextPhase(EComplexTaskPhase CurrentPhase) const override;
    virtual bool ValidateTaskConditions() const override;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Climb Config")
    FVector ClimbTarget;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Climb Config")
    float ClimbHeight;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Climb Config")
    float ClimbSpeed;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Climb Config")
    bool bAutoDetectClimbHeight;

private:
    FVector StartLocation;
    FVector GrabLocation;
    FVector TopLocation;
    FVector EndLocation;
    bool bHasValidClimbPath;
    float ClimbProgress;
};

/**
 * Task_EnterVehicle - Enter a vehicle
 */
UCLASS()
class GAME_API UTask_EnterVehicle : public UComplexTask
{
    GENERATED_BODY()

public:
    UTask_EnterVehicle();

    // Public setter methods for TaskManager access
    void SetSeatIndex(int32 Index) { SeatIndex = Index; }

protected:
    virtual bool PrepareTask() override;
    virtual bool ExecutePhase(EComplexTaskPhase Phase, float DeltaTime) override;
    virtual bool CanAdvanceToNextPhase(EComplexTaskPhase CurrentPhase) const override;
    virtual bool ValidateTaskConditions() const override;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Vehicle Config")
    AActor* TargetVehicle;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Vehicle Config")
    int32 SeatIndex;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Vehicle Config")
    bool bOpenDoor;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Vehicle Config")
    float ApproachSpeed;

private:
    FVector DoorLocation;
    FVector SeatLocation;
    bool bDoorOpened;
    bool bReachedDoor;
    float ApproachProgress;
};

/**
 * Task_GrabLedgeAndHold - Grab a ledge and hold position
 */
UCLASS()
class GAME_API UTask_GrabLedgeAndHold : public UComplexTask
{
    GENERATED_BODY()

public:
    UTask_GrabLedgeAndHold();

    // Public setter methods for TaskManager access
    void SetLedgeLocation(const FVector& Location) { LedgeLocation = Location; }
    void SetHoldDuration(float Duration) { HoldDuration = Duration; }
    void SetAutoDetectLedge(bool bAutoDetect) { bAutoDetectLedge = bAutoDetect; }

protected:
    virtual bool PrepareTask() override;
    virtual bool ExecutePhase(EComplexTaskPhase Phase, float DeltaTime) override;
    virtual bool CanAdvanceToNextPhase(EComplexTaskPhase CurrentPhase) const override;
    virtual bool ValidateTaskConditions() const override;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Ledge Config")
    FVector LedgeLocation;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Ledge Config")
    float GrabRange;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Ledge Config")
    float HoldDuration;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Ledge Config")
    bool bAutoDetectLedge;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Ledge Config")
    float StaminaDrainRate;

private:
    FVector GrabPosition;
    FVector HangPosition;
    bool bIsHanging;
    float HoldTime;
    float CurrentStamina;
    bool bHasValidLedge;
};

/**
 * Task_ClimbLadder - Climb up or down a ladder
 */
UCLASS()
class GAME_API UTask_ClimbLadder : public UComplexTask
{
    GENERATED_BODY()

public:
    UTask_ClimbLadder();

    // Public setter methods for TaskManager access
    void SetClimbDirection(bool bUp) { bClimbUp = bUp; }

protected:
    virtual bool PrepareTask() override;
    virtual bool ExecutePhase(EComplexTaskPhase Phase, float DeltaTime) override;
    virtual bool CanAdvanceToNextPhase(EComplexTaskPhase CurrentPhase) const override;
    virtual bool ValidateTaskConditions() const override;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Ladder Config")
    AActor* TargetLadder;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Ladder Config")
    bool bClimbUp;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Ladder Config")
    float ClimbSpeed;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Ladder Config")
    float ApproachDistance;

private:
    FVector LadderBottomLocation;
    FVector LadderTopLocation;
    FVector TargetLocation;
    FVector StartClimbLocation;
    bool bReachedLadder;
    bool bIsClimbing;
    float ClimbProgress;
    float LadderHeight;
};
