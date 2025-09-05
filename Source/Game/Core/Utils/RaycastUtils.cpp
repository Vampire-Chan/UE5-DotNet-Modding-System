#include "Core/Utils/RaycastUtils.h"
#include "Engine/World.h"
#include "DrawDebugHelpers.h"
#include "Components/SkeletalMeshComponent.h"
#include "GameFramework/Actor.h"
#include "Engine/Engine.h"

URaycastUtils::URaycastUtils()
{
    // Nothing to initialize for UObject
}

bool URaycastUtils::PerformRaycast(UWorld* World, const FRaycastParams& Params, FHitResult& OutHit)
{
    if (!World)
    {
        return false;
    }

    FCollisionQueryParams QueryParams;
    QueryParams.bTraceComplex = Params.bTraceComplex;
    QueryParams.bReturnPhysicalMaterial = true;
    
    for (AActor* ActorToIgnore : Params.ActorsToIgnore)
    {
        if (ActorToIgnore)
        {
            QueryParams.AddIgnoredActor(ActorToIgnore);
        }
    }

    switch (Params.TraceType)
    {
        case ERaycastType::LineTrace:
            return World->LineTraceSingleByChannel(
                OutHit,
                Params.StartLocation,
                Params.EndLocation,
                ECC_Visibility,
                QueryParams
            );
        case ERaycastType::SphereTrace:
            return World->SweepSingleByChannel(
                OutHit,
                Params.StartLocation,
                Params.EndLocation,
                FQuat::Identity,
                ECC_Visibility,
                FCollisionShape::MakeSphere(Params.TraceRadius),
                QueryParams
            );
        case ERaycastType::BoxTrace:
            return World->SweepSingleByChannel(
                OutHit,
                Params.StartLocation,
                Params.EndLocation,
                FQuat::Identity,
                ECC_Visibility,
                FCollisionShape::MakeBox(Params.TraceBoxExtent),
                QueryParams
            );
        case ERaycastType::CapsuleTrace:
            return World->SweepSingleByChannel(
                OutHit,
                Params.StartLocation,
                Params.EndLocation,
                FQuat::Identity,
                ECC_Visibility,
                FCollisionShape::MakeCapsule(Params.TraceCapsuleRadius, Params.TraceCapsuleHeight),
                QueryParams
            );
        default:
            return false;
    }
}

bool URaycastUtils::PerformMultiRaycast(UWorld* World, const FRaycastParams& Params, TArray<FHitResult>& OutHits)
{
    if (!World)
    {
        return false;
    }

    FCollisionQueryParams QueryParams;
    QueryParams.bTraceComplex = Params.bTraceComplex;
    QueryParams.bReturnPhysicalMaterial = true;
    
    for (AActor* ActorToIgnore : Params.ActorsToIgnore)
    {
        if (ActorToIgnore)
        {
            QueryParams.AddIgnoredActor(ActorToIgnore);
        }
    }

    return World->LineTraceMultiByChannel(
        OutHits,
        Params.StartLocation,
        Params.EndLocation,
        ECC_Visibility,
        QueryParams
    );
}

FCoverDetectionResult URaycastUtils::DetectCover(UWorld* World, const FVector& CharacterLocation, const FVector& LookDirection, float MaxCoverDistance)
{
    FCoverDetectionResult Result;
    Result.bIsValidCover = false;
    Result.CoverHeight = 0.0f;
    Result.CoverType = ECoverType::None;
    
    if (!World)
    {
        return Result;
    }

    FVector TargetLocation = CharacterLocation + (LookDirection * MaxCoverDistance);

    // Perform raycast from character to target
    FHitResult HitResult;
    FCollisionQueryParams QueryParams;
    QueryParams.bTraceComplex = true;
    
    bool bHit = World->LineTraceSingleByChannel(
        HitResult,
        CharacterLocation,
        TargetLocation,
        ECC_Visibility,
        QueryParams
    );
    
    if (bHit)
    {
        Result.bIsValidCover = true;
        Result.CoverLocation = HitResult.Location;
        Result.CoverNormal = HitResult.Normal;
        Result.CoverHeight = FMath::Abs(HitResult.Location.Z - CharacterLocation.Z);
        
        // Determine cover type based on height
        if (Result.CoverHeight < MaxCoverDistance * 0.3f)
        {
            Result.CoverType = ECoverType::LowCover;
        }
        else if (Result.CoverHeight < MaxCoverDistance * 0.7f)
        {
            Result.CoverType = ECoverType::HighCover;
        }
        else
        {
            Result.CoverType = ECoverType::FullCover;
        }
        
        Result.CoverActor = HitResult.GetActor();
    }
    
    return Result;
}

TArray<FCoverDetectionResult> URaycastUtils::FindNearbyCovers(UWorld* World, const FVector& CharacterLocation, float SearchRadius)
{
    TArray<FCoverDetectionResult> CoverResults;
    
    if (!World)
    {
        return CoverResults;
    }

    // Create a grid pattern for cover detection
    int32 NumRays = 16;
    float AngleStep = 360.0f / NumRays;
    
    for (int32 i = 0; i < NumRays; i++)
    {
        float Angle = i * AngleStep;
        FVector Direction = FVector(
            FMath::Cos(FMath::DegreesToRadians(Angle)),
            FMath::Sin(FMath::DegreesToRadians(Angle)),
            0.0f
        );
        
        FCoverDetectionResult CoverResult = DetectCover(World, CharacterLocation, Direction, SearchRadius);
        
        if (CoverResult.bIsValidCover)
        {
            CoverResults.Add(CoverResult);
        }
    }
    
    return CoverResults;
}

bool URaycastUtils::IsLocationBehindCover(UWorld* World, const FVector& CheckLocation, const FVector& ThreatLocation, float CoverThreshold)
{
    if (!World)
    {
        return false;
    }

    FHitResult HitResult;
    FCollisionQueryParams QueryParams;
    QueryParams.bTraceComplex = true;
    
    bool bHit = World->LineTraceSingleByChannel(
        HitResult,
        CheckLocation,
        ThreatLocation,
        ECC_Visibility,
        QueryParams
    );
    
    if (bHit)
    {
        float DistanceToHit = FVector::Dist(CheckLocation, HitResult.Location);
        float DistanceToThreat = FVector::Dist(CheckLocation, ThreatLocation);
        
        return DistanceToHit < DistanceToThreat && DistanceToHit > CoverThreshold;
    }
    
    return false;
}

FWeaponAimResult URaycastUtils::CalculateWeaponAim(UWorld* World, const FVector& WeaponMuzzleLocation, const FVector& TargetLocation, float MaxRange, AActor* ShooterActor)
{
    FWeaponAimResult Result;
    Result.bCanAim = false;
    Result.AimDirection = FVector::ZeroVector;
    Result.ImpactPoint = TargetLocation;
    Result.bIsObstructed = false;
    
    if (!World)
    {
        return Result;
    }

    // Calculate aim direction
    FVector AimDirection = (TargetLocation - WeaponMuzzleLocation).GetSafeNormal();
    Result.AimDirection = AimDirection;
    
    // Check if target is within weapon range
    float DistanceToTarget = FVector::Dist(WeaponMuzzleLocation, TargetLocation);
    Result.DistanceToTarget = DistanceToTarget;
    
    if (DistanceToTarget > MaxRange)
    {
        return Result;
    }
    
    // Perform raycast to check for obstructions
    FHitResult HitResult;
    FCollisionQueryParams QueryParams;
    QueryParams.bTraceComplex = true;
    
    if (ShooterActor)
    {
        QueryParams.AddIgnoredActor(ShooterActor);
    }
    
    bool bHit = World->LineTraceSingleByChannel(
        HitResult,
        WeaponMuzzleLocation,
        TargetLocation,
        ECC_Visibility,
        QueryParams
    );
    
    if (bHit)
    {
        Result.ImpactPoint = HitResult.Location;
        Result.bIsObstructed = true;
        Result.ObstructionPoint = HitResult.Location;
        Result.ObstructingActor = HitResult.GetActor();
        
        float ObstructionDistance = FVector::Dist(WeaponMuzzleLocation, HitResult.Location);
        Result.bCanAim = ObstructionDistance >= DistanceToTarget * 0.9f; // Allow for small margin
        Result.HitActor = HitResult.GetActor();
    }
    else
    {
        Result.bCanAim = true;
        Result.ImpactPoint = TargetLocation;
    }
    
    return Result;
}

bool URaycastUtils::CanAimAtTarget(UWorld* World, const FVector& WeaponMuzzleLocation, const FVector& TargetLocation, AActor* ShooterActor)
{
    if (!World)
    {
        return false;
    }

    FHitResult HitResult;
    FCollisionQueryParams QueryParams;
    QueryParams.bTraceComplex = true;
    
    if (ShooterActor)
    {
        QueryParams.AddIgnoredActor(ShooterActor);
    }
    
    bool bHit = World->LineTraceSingleByChannel(
        HitResult,
        WeaponMuzzleLocation,
        TargetLocation,
        ECC_Visibility,
        QueryParams
    );
    
    // If we hit something before reaching the target, we can't aim directly
    if (bHit)
    {
        float DistanceToHit = FVector::Dist(WeaponMuzzleLocation, HitResult.Location);
        float DistanceToTarget = FVector::Dist(WeaponMuzzleLocation, TargetLocation);
        
        return DistanceToHit >= DistanceToTarget * 0.95f; // Allow for small margin of error
    }
    
    return true;
}

FVector URaycastUtils::CalculateBulletTrajectory(const FVector& StartLocation, const FVector& TargetLocation, float BulletSpeed, float Gravity)
{
    FVector Direction = (TargetLocation - StartLocation).GetSafeNormal();
    float Distance = FVector::Dist(StartLocation, TargetLocation);
    float FlightTime = Distance / BulletSpeed;
    
    // Calculate gravity drop
    float GravityDrop = 0.5f * Gravity * FlightTime * FlightTime;
    
    // Adjust trajectory to compensate for gravity
    FVector AdjustedDirection = Direction;
    AdjustedDirection.Z += GravityDrop / Distance;
    
    return AdjustedDirection.GetSafeNormal();
}

FIKAdjustmentResult URaycastUtils::CalculateHandIKAdjustments(UWorld* World, USkeletalMeshComponent* CharacterMesh, const FVector& TargetLocation, const FString& LeftHandBoneName, const FString& RightHandBoneName)
{
    FIKAdjustmentResult Result;
    Result.bNeedsAdjustment = false;
    Result.LeftHandTargetLocation = FVector::ZeroVector;
    Result.RightHandTargetLocation = FVector::ZeroVector;
    Result.LeftHandAlpha = 0.0f;
    Result.RightHandAlpha = 0.0f;
    
    if (!World || !CharacterMesh)
    {
        return Result;
    }

    // Get bone locations
    FVector LeftHandLocation = CharacterMesh->GetBoneLocation(*LeftHandBoneName);
    FVector RightHandLocation = CharacterMesh->GetBoneLocation(*RightHandBoneName);
    
    // Check for obstructions for each hand
    bool bLeftHandObstructed = !IsLocationVisible(World, LeftHandLocation, TargetLocation, CharacterMesh->GetOwner());
    bool bRightHandObstructed = !IsLocationVisible(World, RightHandLocation, TargetLocation, CharacterMesh->GetOwner());
    
    if (bLeftHandObstructed || bRightHandObstructed)
    {
        Result.bNeedsAdjustment = true;
        
        // Calculate adjustments to avoid obstructions
        if (bLeftHandObstructed)
        {
            Result.AdjustmentType = EIKAdjustmentType::RaiseLeftHand;
            Result.LeftHandTargetLocation = LeftHandLocation + FVector(0, 0, 50); // Raise left hand
            Result.LeftHandAlpha = 1.0f;
        }
        
        if (bRightHandObstructed)
        {
            if (Result.AdjustmentType == EIKAdjustmentType::RaiseLeftHand)
            {
                Result.AdjustmentType = EIKAdjustmentType::RaiseBothHands;
            }
            else
            {
                Result.AdjustmentType = EIKAdjustmentType::RaiseRightHand;
            }
            Result.RightHandTargetLocation = RightHandLocation + FVector(0, 0, 50); // Raise right hand
            Result.RightHandAlpha = 1.0f;
        }
    }
    
    return Result;
}

bool URaycastUtils::ShouldRaiseHands(UWorld* World, const FVector& CharacterLocation, const FVector& HandLocation, const FVector& TargetDirection)
{
    if (!World)
    {
        return false;
    }

    FVector TargetLocation = CharacterLocation + (TargetDirection * 1000.0f);
    
    // Check if hand path is obstructed
    FHitResult HitResult;
    FCollisionQueryParams QueryParams;
    QueryParams.bTraceComplex = true;
    
    bool bHit = World->LineTraceSingleByChannel(
        HitResult,
        HandLocation,
        TargetLocation,
        ECC_Visibility,
        QueryParams
    );
    
    if (bHit)
    {
        // If obstacle is close to hand and in the target direction
        float DistanceToObstacle = FVector::Dist(HandLocation, HitResult.Location);
        return DistanceToObstacle < DEFAULT_IK_ADJUSTMENT_THRESHOLD;
    }
    
    return false;
}

bool URaycastUtils::ShouldLowerHands(UWorld* World, const FVector& CharacterLocation, const FVector& HandLocation, const FVector& TargetDirection)
{
    if (!World)
    {
        return false;
    }

    FVector TargetLocation = CharacterLocation + (TargetDirection * 1000.0f);
    
    // Check if path is clear
    return IsLocationVisible(World, HandLocation, TargetLocation, nullptr);
}

float URaycastUtils::CalculateObstructionPercentage(UWorld* World, const FVector& StartLocation, const FVector& EndLocation, int32 NumRays)
{
    if (!World)
    {
        return 1.0f;
    }

    TArray<FVector> RayDirections = GenerateRayPattern(StartLocation, EndLocation, NumRays, 50.0f);
    int32 BlockedRays = 0;
    
    for (const FVector& RayDirection : RayDirections)
    {
        FHitResult HitResult;
        FCollisionQueryParams QueryParams;
        QueryParams.bTraceComplex = true;
        
        bool bHit = World->LineTraceSingleByChannel(
            HitResult,
            StartLocation,
            RayDirection,
            ECC_Visibility,
            QueryParams
        );
        
        if (bHit)
        {
            float DistanceToHit = FVector::Dist(StartLocation, HitResult.Location);
            float DistanceToTarget = FVector::Dist(StartLocation, EndLocation);
            
            if (DistanceToHit < DistanceToTarget * 0.9f)
            {
                BlockedRays++;
            }
        }
    }
    
    return static_cast<float>(BlockedRays) / static_cast<float>(NumRays);
}

TArray<FVector> URaycastUtils::GenerateRayPattern(const FVector& CenterStart, const FVector& CenterEnd, int32 NumRays, float SpreadRadius)
{
    TArray<FVector> RayDirections;
    
    FVector CenterDirection = (CenterEnd - CenterStart).GetSafeNormal();
    FVector RightVector = FVector::CrossProduct(CenterDirection, FVector::UpVector).GetSafeNormal();
    FVector UpVector = FVector::CrossProduct(RightVector, CenterDirection).GetSafeNormal();
    
    // Add center ray
    RayDirections.Add(CenterEnd);
    
    // Add rays in a circular pattern around the center
    float Distance = FVector::Dist(CenterStart, CenterEnd);
    for (int32 i = 1; i < NumRays; i++)
    {
        float Angle = (2.0f * PI * i) / (NumRays - 1);
        float Radius = SpreadRadius / Distance;
        
        FVector Offset = (RightVector * FMath::Cos(Angle) + UpVector * FMath::Sin(Angle)) * SpreadRadius;
        FVector RayEnd = CenterEnd + Offset;
        
        RayDirections.Add(RayEnd);
    }
    
    return RayDirections;
}

bool URaycastUtils::IsLocationVisible(UWorld* World, const FVector& FromLocation, const FVector& ToLocation, AActor* IgnoreActor)
{
    if (!World)
    {
        return false;
    }

    FHitResult HitResult;
    FCollisionQueryParams QueryParams;
    QueryParams.bTraceComplex = true;
    
    if (IgnoreActor)
    {
        QueryParams.AddIgnoredActor(IgnoreActor);
    }
    
    bool bHit = World->LineTraceSingleByChannel(
        HitResult,
        FromLocation,
        ToLocation,
        ECC_Visibility,
        QueryParams
    );
    
    return !bHit;
}

FVector URaycastUtils::GetClosestPointOnSurface(UWorld* World, const FVector& StartLocation, const FVector& Direction, float MaxDistance)
{
    if (!World)
    {
        return StartLocation;
    }

    FVector EndLocation = StartLocation + (Direction * MaxDistance);
    FHitResult HitResult;
    FCollisionQueryParams QueryParams;
    QueryParams.bTraceComplex = true;
    
    bool bHit = World->LineTraceSingleByChannel(
        HitResult,
        StartLocation,
        EndLocation,
        ECC_Visibility,
        QueryParams
    );
    
    if (bHit)
    {
        return HitResult.Location;
    }
    
    return EndLocation;
}

void URaycastUtils::DrawDebugRaycast(UWorld* World, const FVector& Start, const FVector& End, bool bHit, const FHitResult& HitResult, float Duration)
{
    if (!World)
    {
        return;
    }

    FColor LineColor = bHit ? FColor::Red : FColor::Green;
    
    DrawDebugLine(World, Start, End, LineColor, false, Duration, 0, 2.0f);
    
    if (bHit)
    {
        DrawDebugSphere(World, HitResult.Location, 10.0f, 8, FColor::Yellow, false, Duration);
        DrawDebugDirectionalArrow(World, HitResult.Location, HitResult.Location + (HitResult.Normal * 50.0f), 5.0f, FColor::Blue, false, Duration);
    }
}

void URaycastUtils::DrawDebugCover(UWorld* World, const FCoverDetectionResult& CoverResult, float Duration)
{
    if (!World || !CoverResult.bIsValidCover)
    {
        return;
    }

    FColor CoverColor = FColor::Orange;
    switch (CoverResult.CoverType)
    {
        case ECoverType::LowCover:
            CoverColor = FColor::Yellow;
            break;
        case ECoverType::HighCover:
            CoverColor = FColor::Orange;
            break;
        case ECoverType::FullCover:
            CoverColor = FColor::Red;
            break;
        default:
            CoverColor = FColor::White;
            break;
    }
    
    DrawDebugSphere(World, CoverResult.CoverLocation, 20.0f, 8, CoverColor, false, Duration);
    DrawDebugDirectionalArrow(World, CoverResult.CoverLocation, CoverResult.CoverLocation + (CoverResult.CoverNormal * 100.0f), 10.0f, FColor::Purple, false, Duration);
}

void URaycastUtils::DrawDebugWeaponAim(UWorld* World, const FWeaponAimResult& AimResult, const FVector& StartLocation, float Duration)
{
    if (!World)
    {
        return;
    }

    FColor AimColor = AimResult.bCanAim ? FColor::Green : FColor::Red;
    
    DrawDebugLine(World, StartLocation, AimResult.ImpactPoint, AimColor, false, Duration, 0, 3.0f);
    DrawDebugSphere(World, AimResult.ImpactPoint, 15.0f, 8, AimColor, false, Duration);
    
    // Draw obstruction info if obstructed
    if (AimResult.bIsObstructed)
    {
        FString ObstructionText = TEXT("Obstructed");
        DrawDebugString(World, AimResult.ImpactPoint + FVector(0, 0, 50), ObstructionText, nullptr, FColor::White, Duration);
    }
}
