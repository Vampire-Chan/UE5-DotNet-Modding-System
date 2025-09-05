#pragma once

#include "CoreMinimal.h"
#include "Animation/AnimInstance.h"
#include "Engine/Engine.h"
#include "Animation/AnimSequence.h"
#include "Animation/AnimMontage.h"
#include "Animation/BlendSpace.h"
#include "Components/SkeletalMeshComponent.h"
#include "GameFramework/Character.h"
#include "../Core/Enums/GameWorldEnums.h"
#include "PedAnimationController.generated.h"

USTRUCT(BlueprintType)
struct FAnimationData
{
    GENERATED_BODY()

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    UAnimSequence* AnimSequence;

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    float BlendInTime;

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    float BlendOutTime;

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    float PlayRate;

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    bool bLooping;

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    bool bCanBeCancelled;

    FAnimationData()
    {
        AnimSequence = nullptr;
        BlendInTime = 0.15f;
        BlendOutTime = 0.15f;
        PlayRate = 1.0f;
        bLooping = false;
        bCanBeCancelled = true;
    }
};

USTRUCT(BlueprintType)
struct FMovementAnimSet
{
    GENERATED_BODY()

    // Basic Movement
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Basic Movement")
    FAnimationData Idle;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Basic Movement")
    FAnimationData Walk;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Basic Movement")
    FAnimationData Run;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Basic Movement")
    FAnimationData Sprint;

    // Directional Movement
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Directional")
    FAnimationData WalkBackward;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Directional")
    FAnimationData WalkStrafeLeft;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Directional")
    FAnimationData WalkStrafeRight;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Directional")
    FAnimationData RunStrafeLeft;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Directional")
    FAnimationData RunStrafeRight;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Directional")
    FAnimationData RunStrafeBackward;

    // Start/Stop Animations
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Transitions")
    TArray<FAnimationData> WalkStarts; // wstart, wstart_turn_l90, etc.

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Transitions")
    TArray<FAnimationData> WalkStops; // wstop_l, wstop_r

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Transitions")
    TArray<FAnimationData> RunStarts; // runstart_fwd, runstart_l_90, etc.

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Transitions")
    TArray<FAnimationData> RunStops; // rstop_l, rstop_r

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Transitions")
    TArray<FAnimationData> SprintStops; // sstop_l, sstop_r

    // Turn Animations
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Turning")
    TArray<FAnimationData> WalkTurns; // walk_turn_l, walk_turn_r, walk_turn_180_l, etc.

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Turning")
    TArray<FAnimationData> RunTurns; // run_turn_l, run_turn_r, run_turn_180, etc.

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Turning")
    TArray<FAnimationData> SprintTurns; // sprint_turn_l, sprint_turn_r, sprint_turn_180_l, etc.

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Turning")
    TArray<FAnimationData> Full360Turns; // turn_360_l, turn_360_r
};

USTRUCT(BlueprintType)
struct FCrouchAnimSet
{
    GENERATED_BODY()

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Crouch")
    FAnimationData CrouchIdle;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Crouch")
    FAnimationData CrouchWalk;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Crouch")
    FAnimationData CrouchRun;

    // Transitions
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Crouch Transitions")
    FAnimationData IdleToCrouch; // idle2crouchidle

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Crouch Transitions")
    FAnimationData CrouchToIdle; // crouchidle2idle

    // Crouch Turns
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Crouch Turning")
    TArray<FAnimationData> CrouchTurns; // walk_turn_l, walk_turn_r, run_turn_l, etc.

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Crouch Turning")
    TArray<FAnimationData> CrouchStops; // wstop_l, wstop_r, rstop_l, rstop_r

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Crouch Turning")
    TArray<FAnimationData> Crouch360Turns; // turn_360_l, turn_360_r
};

USTRUCT(BlueprintType)
struct FJumpAnimSet
{
    GENERATED_BODY()

    // Jump Takeoff Arrays (for animation selection)
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Jump Arrays")
    TArray<FAnimationData> JumpTakeoffs; // For selection algorithm

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Jump Arrays")
    TArray<FAnimationData> JumpInAir; // For selection algorithm

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Jump Arrays")
    TArray<FAnimationData> JumpLandings; // For selection algorithm

    // Individual Jump Animations (from jump_std folder)
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Jump Takeoff")
    FAnimationData JumpTakeoffLeft; // jump_takeoff_l.onim

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Jump Takeoff")
    FAnimationData JumpTakeoffRight; // jump_takeoff_r.onim

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Jump")
    FAnimationData JumpOnSpot; // jump_on_spot.onim

    // In Air
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Jump Air")
    FAnimationData JumpInAirLeft; // jump_inair_l.onim

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Jump Air")
    FAnimationData JumpInAirRight; // jump_inair_r.onim

    // Landing
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Jump Landing")
    FAnimationData JumpLandLeft; // jump_land_l.onim

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Jump Landing")
    FAnimationData JumpLandRight; // jump_land_r.onim

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Jump Landing")
    FAnimationData JumpLandRoll; // jump_land_roll.onim

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Jump Landing")
    FAnimationData JumpLandSquat; // jump_land_squat.onim
};

USTRUCT(BlueprintType)
struct FCombatAnimSet
{
    GENERATED_BODY()

    // Combat Basic Movement (from move_combat_strafe folder)
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Combat Basic")
    FAnimationData Idle; // idle.onim

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Combat Basic")
    FAnimationData Walk; // walk.onim

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Combat Basic")
    FAnimationData Run; // run.onim

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Combat Basic")
    FAnimationData WalkStart; // wstart.onim

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Combat Basic")
    FAnimationData ShuffleStop; // shuffle_stop.onim

    // Combat Strafe Movement
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Combat Strafe")
    FAnimationData WalkStrafeLeft; // walk_strafe_l.onim

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Combat Strafe")
    FAnimationData WalkStrafeRight; // walk_strafe_r.onim

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Combat Strafe")
    FAnimationData WalkStrafeBackward; // walk_strafe_b.onim

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Combat Strafe")
    FAnimationData WalkStrafeLeft45; // walk_strafe_l45.onim

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Combat Strafe")
    FAnimationData WalkStrafeRight45; // walk_strafe_r45.onim

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Combat Strafe")
    FAnimationData WalkStrafeBackLeft45; // walk_strafe_bl45.onim

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Combat Strafe")
    FAnimationData WalkStrafeBackRight45; // walk_strafe_br45.onim

    // Combat Run Strafe
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Combat Run Strafe")
    FAnimationData RunStrafeLeft; // run_strafe_l.onim

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Combat Run Strafe")
    FAnimationData RunStrafeRight; // run_strafe_r.onim

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Combat Run Strafe")
    FAnimationData RunStrafeBackward; // run_strafe_b.onim

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Combat Run Strafe")
    FAnimationData RunStrafeLeft45; // run_strafe_l45.onim

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Combat Run Strafe")
    FAnimationData RunStrafeRight45; // run_strafe_r45.onim

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Combat Run Strafe")
    FAnimationData RunStrafeBackLeft45; // run_strafe_bl45.onim

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Combat Run Strafe")
    FAnimationData RunStrafeBackRight45; // run_strafe_br45.onim

    // Combat Turns
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Combat Turns")
    FAnimationData Turn360Left; // turn_360_l.onim

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Combat Turns")
    FAnimationData Turn360Right; // turn_360_r.onim
};

USTRUCT(BlueprintType)
struct FVehicleAnimSet
{
    GENERATED_BODY()

    // Basic Vehicle Positions (from veh@std folder)
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Vehicle Basic")
    FAnimationData SitDrive; // sit_drive.onim

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Vehicle Basic")
    FAnimationData SitPass; // sit_pass.onim

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Vehicle Basic")
    FAnimationData SitPassBackRight; // sit_pass_back_right.onim

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Vehicle Basic")
    FAnimationData RelaxedIdleA; // relaxed_idle_a.onim

    // Steering
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Vehicle Steering")
    FAnimationData SteerLeft; // steer_l.onim

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Vehicle Steering")
    FAnimationData SteerRight; // steer_r.onim

    // Vehicle Entry/Exit
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Vehicle Entry")
    FAnimationData GetInDriverSide; // get_in_ds.onim

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Vehicle Entry")
    FAnimationData GetInPassengerSide; // get_in_ps.onim

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Vehicle Entry")
    FAnimationData GetOutDriverSide; // get_out_ds.onim

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Vehicle Entry")
    FAnimationData GetOutPassengerSide; // get_out_ps.onim

    // Vehicle Actions
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Vehicle Actions")
    FAnimationData Keystart; // keystart.onim

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Vehicle Actions")
    FAnimationData Hotwire; // hotwire.onim

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Vehicle Actions")
    FAnimationData Horn; // horn.onim

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Vehicle Actions")
    FAnimationData HornNormal; // horn_normal.onim

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Vehicle Actions")
    FAnimationData Reverse; // reverse.onim

    // Vehicle Braking
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Vehicle Braking")
    FAnimationData HeavyBrakeDriverSide; // heavy_brake_ds.onim

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Vehicle Braking")
    FAnimationData HeavyBrakePassengerSide; // heavy_brake_ps.onim

    // Vehicle Shock/Impact
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Vehicle Impact")
    FAnimationData ShockFront; // shock_front.onim

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Vehicle Impact")
    FAnimationData ShockBack; // shock_back.onim

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Vehicle Impact")
    FAnimationData ShockLeft; // shock_left.onim

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Vehicle Impact")
    FAnimationData ShockRight; // shock_right.onim
};

/**
 * Pure C++ Animation Controller for Ped/Player characters
 * Handles all movement animations with smooth blending and cancellation support
 */
UCLASS(BlueprintType, Blueprintable)
class GAME_API UPedAnimationController : public UAnimInstance
{
    GENERATED_BODY()

public:
    UPedAnimationController();

    virtual void NativeInitializeAnimation() override;
    virtual void NativeUpdateAnimation(float DeltaTimeX) override;

    // Current State Variables (Public for Manager Access)
    UPROPERTY(BlueprintReadOnly, Category = "Animation State")
    EPedMovementState CurrentMovementState;

    UPROPERTY(BlueprintReadOnly, Category = "Animation State")
    EPedAnimationRegion CurrentAnimationRegion;

    UPROPERTY(BlueprintReadOnly, Category = "Animation State")
    EPedStanceState CurrentStanceState;

    UPROPERTY(BlueprintReadOnly, Category = "Combat State")
    EPedWeaponType CurrentWeaponType;

    UPROPERTY(BlueprintReadOnly, Category = "Combat State")
    EPedCombatState CurrentCombatState;

    UPROPERTY(BlueprintReadOnly, Category = "Combat State")
    bool bIsInCombat;

    UPROPERTY(BlueprintReadOnly, Category = "Combat State")
    bool bIsAiming;

    UPROPERTY(BlueprintReadOnly, Category = "Animation Control")
    FString CurrentAnimationName;

protected:
    // Animation Sets
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Animation Sets")
    FMovementAnimSet MovementAnims;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Animation Sets")
    FCrouchAnimSet CrouchAnims;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Animation Sets")
    FJumpAnimSet JumpAnims;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Animation Sets")
    FCombatAnimSet CombatAnims;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Animation Sets")
    FVehicleAnimSet VehicleAnims;

    // Turn Direction (Protected)
    UPROPERTY(BlueprintReadOnly, Category = "Animation State")
    ETurnDirection CurrentTurnDirection;

    // Movement Variables
    UPROPERTY(BlueprintReadOnly, Category = "Movement")
    float Speed;

    UPROPERTY(BlueprintReadOnly, Category = "Movement")
    float Direction;

    UPROPERTY(BlueprintReadOnly, Category = "Movement")
    FVector Velocity;

    UPROPERTY(BlueprintReadOnly, Category = "Movement")
    bool bIsInAir;

    UPROPERTY(BlueprintReadOnly, Category = "Movement")
    bool bIsCrouching;

    UPROPERTY(BlueprintReadOnly, Category = "Movement")
    bool bIsInVehicle;

    UPROPERTY(BlueprintReadOnly, Category = "Movement")
    bool bIsInCover;

    UPROPERTY(BlueprintReadOnly, Category = "Movement")
    bool bIsInteracting;

    UPROPERTY(BlueprintReadOnly, Category = "Movement")
    float TurnRate;

    UPROPERTY(BlueprintReadOnly, Category = "Movement")
    float LeanAngle;

    // Animation Control
    UPROPERTY(BlueprintReadOnly, Category = "Animation Control")
    bool bCanCancelCurrentAnimation;

    UPROPERTY(BlueprintReadOnly, Category = "Animation Control")
    float CurrentAnimationBlendWeight;

private:
    // Character Reference
    UPROPERTY()
    ACharacter* OwnerCharacter;

    // Animation Blending
    float BlendTimer;
    float BlendDuration;
    bool bIsBlending;
    
    // Previous state for transitions
    EPedMovementState PreviousMovementState;
    EPedStanceState PreviousStanceState;
    
    // Turn detection
    float PreviousYaw;
    float YawDelta;
    float TurnThreshold;

public:
    // Animation Control Functions
    UFUNCTION(BlueprintCallable, Category = "Animation")
    void PlayAnimation(const FAnimationData& AnimData, bool bForcePlay = false);

    UFUNCTION(BlueprintCallable, Category = "Animation")
    void CancelCurrentAnimation();

    // Movement parameter setters for external components
    UFUNCTION(BlueprintCallable, Category = "Movement")
    void SetMovementSpeed(float NewSpeed) { Speed = NewSpeed; }

    UFUNCTION(BlueprintCallable, Category = "Movement")  
    void SetMovementDirection(float NewDirection) { Direction = NewDirection; }

    UFUNCTION(BlueprintCallable, Category = "Movement")
    void SetIsInAir(bool NewIsInAir) { bIsInAir = NewIsInAir; }

    UFUNCTION(BlueprintCallable, Category = "Movement")
    void SetIsCrouching(bool NewIsCrouching) { bIsCrouching = NewIsCrouching; }

    UFUNCTION(BlueprintCallable, Category = "Animation")
    bool CanPlayAnimation() const;

    // Animation Region Management
    UFUNCTION(BlueprintCallable, Category = "Animation Region")
    EPedAnimationRegion DetermineAnimationRegion() const;

    UFUNCTION(BlueprintCallable, Category = "Animation Region")
    void SetAnimationRegion(EPedAnimationRegion NewRegion);

    UFUNCTION(BlueprintCallable, Category = "Animation Region")
    FString GetRegionContextName(EPedAnimationRegion Region) const;

    // Vehicle Animation Sync
    UFUNCTION(BlueprintCallable, Category = "Vehicle Animation")
    void UpdateVehicleAnimationSync();

    UFUNCTION(BlueprintCallable, Category = "Vehicle Animation")
    void SetVehicleInputs(float SteeringInput, float ThrottleInput, float BrakeInput, int32 CurrentGear);

    // Vehicle Input Variables (for animation sync)
    UPROPERTY(BlueprintReadOnly, Category = "Vehicle Sync")
    float VehicleSteeringInput;

    UPROPERTY(BlueprintReadOnly, Category = "Vehicle Sync")
    float VehicleThrottleInput;

    UPROPERTY(BlueprintReadOnly, Category = "Vehicle Sync")
    float VehicleBrakeInput;

    UPROPERTY(BlueprintReadOnly, Category = "Vehicle Sync")
    int32 VehicleCurrentGear;

    UPROPERTY(BlueprintReadOnly, Category = "Vehicle Sync")
    float VehicleRPM;

    UPROPERTY(BlueprintReadOnly, Category = "Vehicle Sync")
    float VehicleSpeed;

    // Animation Selection Functions
    UFUNCTION(BlueprintCallable, Category = "Animation Selection")
    FAnimationData GetBestStartAnimation(EPedMovementState MovementState, float TurnAngle);

    UFUNCTION(BlueprintCallable, Category = "Animation Selection")
    FAnimationData GetBestStopAnimation(EPedMovementState MovementState, bool bLeftFoot);

    UFUNCTION(BlueprintCallable, Category = "Animation Selection")
    FAnimationData GetBestTurnAnimation(EPedMovementState MovementState, float TurnAngle);

    UFUNCTION(BlueprintCallable, Category = "Animation Selection")
    FAnimationData GetBestJumpAnimation(bool bTakeoff, bool bInAir, bool bLanding, bool bLeftFoot);

    // Utility Functions
    UFUNCTION(BlueprintCallable, Category = "Utility")
    ETurnDirection CalculateTurnDirection(float TurnAngle);

    UFUNCTION(BlueprintCallable, Category = "Utility")
    float GetMovementSpeedRatio() const;

    UFUNCTION(BlueprintCallable, Category = "Utility")
    bool ShouldUseLeftFootVariant() const;

    // Animation Loading Functions
    UFUNCTION(BlueprintCallable, Category = "Animation Loading")
    void LoadAllAnimations();

    UFUNCTION(BlueprintCallable, Category = "Animation Loading")
    UAnimSequence* LoadAnimationFromPath(const FString& AnimationPath);

    // State Management Functions
    UFUNCTION(BlueprintCallable, Category = "State Management")
    void SetMovementState(EPedMovementState NewState);

    UFUNCTION(BlueprintCallable, Category = "State Management")
    void SetStanceState(EPedStanceState NewState);

private:
    // Internal Update Functions
    void UpdateMovementVariables();
    void UpdateAnimationStates();
    void HandleStateTransitions();
    void HandleTurnDetection();
    void UpdateBlending(float DeltaTime);

    // Animation Helper Functions
    FAnimationData SelectRandomFromArray(const TArray<FAnimationData>& AnimArray) const;
    bool IsAnimationValid(const FAnimationData& AnimData) const;
    void InitializeAnimationData();
};
