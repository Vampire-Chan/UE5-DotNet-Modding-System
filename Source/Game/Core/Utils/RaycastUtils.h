#pragma once

#include "CoreMinimal.h"
#include "UObject/NoExportTypes.h"
#include "Engine/World.h"
#include "Engine/HitResult.h"
#include "Components/SkeletalMeshComponent.h"
#include "Components/SceneComponent.h"
#include "../Enums/GameWorldEnums.h"
#include "RaycastUtils.generated.h"

UENUM(BlueprintType)
enum class ERaycastType : uint8
{
    LineTrace           UMETA(DisplayName = "Line Trace"),
    SphereTrace         UMETA(DisplayName = "Sphere Trace"),
    BoxTrace            UMETA(DisplayName = "Box Trace"),
    CapsuleTrace        UMETA(DisplayName = "Capsule Trace")
};

UENUM(BlueprintType)
enum class ECoverType : uint8
{
    None                UMETA(DisplayName = "No Cover"),
    LowCover            UMETA(DisplayName = "Low Cover"),
    HighCover           UMETA(DisplayName = "High Cover"),
    LeftCover           UMETA(DisplayName = "Left Cover"),
    RightCover          UMETA(DisplayName = "Right Cover"),
    FullCover           UMETA(DisplayName = "Full Cover")
};

UENUM(BlueprintType)
enum class EIKAdjustmentType : uint8
{
    None                UMETA(DisplayName = "No Adjustment"),
    RaiseLeftHand       UMETA(DisplayName = "Raise Left Hand"),
    LowerLeftHand       UMETA(DisplayName = "Lower Left Hand"),
    RaiseRightHand      UMETA(DisplayName = "Raise Right Hand"),
    LowerRightHand      UMETA(DisplayName = "Lower Right Hand"),
    RaiseBothHands      UMETA(DisplayName = "Raise Both Hands"),
    LowerBothHands      UMETA(DisplayName = "Lower Both Hands")
};

USTRUCT(BlueprintType)
struct FRaycastParams
{
    GENERATED_BODY()

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Raycast")
    FVector StartLocation;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Raycast")
    FVector EndLocation;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Raycast")
    ERaycastType TraceType;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Raycast")
    float TraceRadius;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Raycast")
    FVector TraceBoxExtent;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Raycast")
    float TraceCapsuleRadius;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Raycast")
    float TraceCapsuleHeight;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Raycast")
    TArray<TEnumAsByte<EObjectTypeQuery>> ObjectTypes;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Raycast")
    TArray<AActor*> ActorsToIgnore;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Raycast")
    bool bTraceComplex;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Raycast")
    bool bIgnoreSelf;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Raycast")
    bool bDrawDebugTrace;

    FRaycastParams()
    {
        StartLocation = FVector::ZeroVector;
        EndLocation = FVector::ZeroVector;
        TraceType = ERaycastType::LineTrace;
        TraceRadius = 5.0f;
        TraceBoxExtent = FVector(10.0f, 10.0f, 10.0f);
        TraceCapsuleRadius = 5.0f;
        TraceCapsuleHeight = 10.0f;
        bTraceComplex = false;
        bIgnoreSelf = true;
        bDrawDebugTrace = false;
    }
};

USTRUCT(BlueprintType)
struct FCoverDetectionResult
{
    GENERATED_BODY()

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Cover")
    ECoverType CoverType;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Cover")
    FVector CoverLocation;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Cover")
    FVector CoverNormal;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Cover")
    float CoverHeight;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Cover")
    float CoverWidth;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Cover")
    AActor* CoverActor;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Cover")
    bool bIsValidCover;

    FCoverDetectionResult()
    {
        CoverType = ECoverType::None;
        CoverLocation = FVector::ZeroVector;
        CoverNormal = FVector::ZeroVector;
        CoverHeight = 0.0f;
        CoverWidth = 0.0f;
        CoverActor = nullptr;
        bIsValidCover = false;
    }
};

USTRUCT(BlueprintType)
struct FWeaponAimResult
{
    GENERATED_BODY()

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Weapon")
    bool bCanAim;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Weapon")
    FVector AimDirection;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Weapon")
    FVector ImpactPoint;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Weapon")
    AActor* HitActor;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Weapon")
    float DistanceToTarget;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Weapon")
    bool bIsObstructed;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Weapon")
    FVector ObstructionPoint;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Weapon")
    AActor* ObstructingActor;

    FWeaponAimResult()
    {
        bCanAim = false;
        AimDirection = FVector::ForwardVector;
        ImpactPoint = FVector::ZeroVector;
        HitActor = nullptr;
        DistanceToTarget = 0.0f;
        bIsObstructed = false;
        ObstructionPoint = FVector::ZeroVector;
        ObstructingActor = nullptr;
    }
};

USTRUCT(BlueprintType)
struct FIKAdjustmentResult
{
    GENERATED_BODY()

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "IK")
    EIKAdjustmentType AdjustmentType;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "IK")
    FVector LeftHandTargetLocation;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "IK")
    FVector RightHandTargetLocation;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "IK")
    FRotator LeftHandTargetRotation;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "IK")
    FRotator RightHandTargetRotation;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "IK")
    float LeftHandAlpha;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "IK")
    float RightHandAlpha;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "IK")
    bool bNeedsAdjustment;

    FIKAdjustmentResult()
    {
        AdjustmentType = EIKAdjustmentType::None;
        LeftHandTargetLocation = FVector::ZeroVector;
        RightHandTargetLocation = FVector::ZeroVector;
        LeftHandTargetRotation = FRotator::ZeroRotator;
        RightHandTargetRotation = FRotator::ZeroRotator;
        LeftHandAlpha = 0.0f;
        RightHandAlpha = 0.0f;
        bNeedsAdjustment = false;
    }
};

/**
 * RaycastUtils - Comprehensive raycasting utilities for cover detection, weapon aiming, and IK adjustments
 * Provides functionality for:
 * - Cover detection and analysis
 * - Weapon aim trajectory calculation
 * - Hand IK adjustments based on obstructions
 * - General purpose raycasting utilities
 */
UCLASS(BlueprintType, Blueprintable)
class GAME_API URaycastUtils : public UObject
{
    GENERATED_BODY()

public:
    URaycastUtils();

    // Core raycasting functions
    UFUNCTION(BlueprintCallable, Category = "Raycast Utils")
    static bool PerformRaycast(UWorld* World, const FRaycastParams& Params, FHitResult& OutHit);

    UFUNCTION(BlueprintCallable, Category = "Raycast Utils")
    static bool PerformMultiRaycast(UWorld* World, const FRaycastParams& Params, TArray<FHitResult>& OutHits);

    // Cover detection functions
    UFUNCTION(BlueprintCallable, Category = "Cover Detection")
    static FCoverDetectionResult DetectCover(UWorld* World, const FVector& CharacterLocation, const FVector& LookDirection, float MaxCoverDistance = 200.0f);

    UFUNCTION(BlueprintCallable, Category = "Cover Detection")
    static TArray<FCoverDetectionResult> FindNearbyCovers(UWorld* World, const FVector& CharacterLocation, float SearchRadius = 500.0f);

    UFUNCTION(BlueprintCallable, Category = "Cover Detection")
    static bool IsLocationBehindCover(UWorld* World, const FVector& CheckLocation, const FVector& ThreatLocation, float CoverThreshold = 50.0f);

    // Weapon aiming functions
    UFUNCTION(BlueprintCallable, Category = "Weapon Aiming")
    static FWeaponAimResult CalculateWeaponAim(UWorld* World, const FVector& WeaponMuzzleLocation, const FVector& TargetLocation, float MaxRange = 1000.0f, AActor* ShooterActor = nullptr);

    UFUNCTION(BlueprintCallable, Category = "Weapon Aiming")
    static bool CanAimAtTarget(UWorld* World, const FVector& WeaponMuzzleLocation, const FVector& TargetLocation, AActor* ShooterActor = nullptr);

    UFUNCTION(BlueprintCallable, Category = "Weapon Aiming")
    static FVector CalculateBulletTrajectory(const FVector& StartLocation, const FVector& TargetLocation, float BulletSpeed, float Gravity = 980.0f);

    // IK adjustment functions
    UFUNCTION(BlueprintCallable, Category = "IK Adjustment")
    static FIKAdjustmentResult CalculateHandIKAdjustments(UWorld* World, USkeletalMeshComponent* CharacterMesh, const FVector& TargetLocation, const FString& LeftHandBoneName = TEXT("hand_l"), const FString& RightHandBoneName = TEXT("hand_r"));

    UFUNCTION(BlueprintCallable, Category = "IK Adjustment")
    static bool ShouldRaiseHands(UWorld* World, const FVector& CharacterLocation, const FVector& HandLocation, const FVector& TargetDirection);

    UFUNCTION(BlueprintCallable, Category = "IK Adjustment")
    static bool ShouldLowerHands(UWorld* World, const FVector& CharacterLocation, const FVector& HandLocation, const FVector& TargetDirection);

    // Utility functions
    UFUNCTION(BlueprintCallable, Category = "Raycast Utils")
    static float CalculateObstructionPercentage(UWorld* World, const FVector& StartLocation, const FVector& EndLocation, int32 NumRays = 9);

    UFUNCTION(BlueprintCallable, Category = "Raycast Utils")
    static TArray<FVector> GenerateRayPattern(const FVector& CenterStart, const FVector& CenterEnd, int32 NumRays, float SpreadRadius);

    UFUNCTION(BlueprintCallable, Category = "Raycast Utils")
    static bool IsLocationVisible(UWorld* World, const FVector& FromLocation, const FVector& ToLocation, AActor* IgnoreActor = nullptr);

    UFUNCTION(BlueprintCallable, Category = "Raycast Utils")
    static FVector GetClosestPointOnSurface(UWorld* World, const FVector& StartLocation, const FVector& Direction, float MaxDistance = 1000.0f);

    // Debug functions
    UFUNCTION(BlueprintCallable, Category = "Debug")
    static void DrawDebugRaycast(UWorld* World, const FVector& Start, const FVector& End, bool bHit, const FHitResult& HitResult, float Duration = 5.0f);

    UFUNCTION(BlueprintCallable, Category = "Debug")
    static void DrawDebugCover(UWorld* World, const FCoverDetectionResult& CoverResult, float Duration = 5.0f);

    UFUNCTION(BlueprintCallable, Category = "Debug")
    static void DrawDebugWeaponAim(UWorld* World, const FWeaponAimResult& AimResult, const FVector& StartLocation, float Duration = 5.0f);

protected:
    // Internal helper functions
    static ECoverType DetermineCoverType(const TArray<FHitResult>& HitResults, const FVector& CharacterLocation);
    static float CalculateCoverHeight(const TArray<FHitResult>& HitResults);
    static float CalculateCoverWidth(UWorld* World, const FVector& CoverLocation, const FVector& CoverNormal);
    static bool ValidateCoverQuality(const FCoverDetectionResult& CoverResult, const FVector& CharacterLocation);

private:
    // Configuration constants
    static constexpr float DEFAULT_COVER_HEIGHT_THRESHOLD = 120.0f;
    static constexpr float DEFAULT_COVER_WIDTH_THRESHOLD = 80.0f;
    static constexpr float DEFAULT_IK_ADJUSTMENT_THRESHOLD = 30.0f;
    static constexpr float DEFAULT_WEAPON_CLEARANCE = 10.0f;
};
