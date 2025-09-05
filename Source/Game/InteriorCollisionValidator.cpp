#include "InteriorCollisionValidator.h"
#include "Components/SphereComponent.h"
#include "Components/StaticMeshComponent.h"
#include "GameFramework/Pawn.h"
#include "GameFramework/Character.h"
#include "Engine/Engine.h"
#include "Engine/World.h"
#include "TimerManager.h"
#include "Kismet/GameplayStatics.h"
#include "Components/CapsuleComponent.h"

AInteriorCollisionValidator::AInteriorCollisionValidator()
{
    PrimaryActorTick.bCanEverTick = true;

    // Create validation sphere component
    ValidationSphere = CreateDefaultSubobject<USphereComponent>(TEXT("ValidationSphere"));
    RootComponent = ValidationSphere;
    ValidationSphere->SetSphereRadius(100.0f);
    ValidationSphere->SetCollisionEnabled(ECollisionEnabled::QueryOnly);
    ValidationSphere->SetCollisionResponseToAllChannels(ECR_Ignore);
    ValidationSphere->SetCollisionResponseToChannel(ECC_Pawn, ECR_Overlap);

    // Create interior mesh component
    InteriorMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("InteriorMesh"));
    InteriorMesh->SetupAttachment(RootComponent);
    InteriorMesh->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);
    InteriorMesh->SetCollisionResponseToAllChannels(ECR_Block);

    // Initialize default settings
    bEnforceDoorEntry = true;
    bLogViolations = true;
    MaxViolationsBeforeKick = 3;
    
    // Mission/Admin settings
    bAllowMissionTeleports = true;
    bAllowAdminOverrides = true;
    MissionTeleportCooldown = 5.0f;
    bLogMissionTeleports = true;
    
    // Set fallback positions
    MainEntranceFallback = FVector(0.0f, -45.0f, 2.0f);
    ExteriorFallback = FVector(148.0f, 195.0f, 2.0f);
    
    LastValidationTime = 0.0f;
}

void AInteriorCollisionValidator::BeginPlay()
{
    Super::BeginPlay();
    
    // Start continuous validation timer
    if (CollisionSettings.bContinuousValidation)
    {
        GetWorldTimerManager().SetTimer(ValidationTimerHandle, 
            this, &AInteriorCollisionValidator::ValidateAllPlayersInRange, 
            CollisionSettings.ValidationFrequency, true);
    }
    
    UE_LOG(LogTemp, Warning, TEXT("InteriorCollisionValidator: Started validation system"));
}

void AInteriorCollisionValidator::Tick(float DeltaTime)
{
    Super::Tick(DeltaTime);
    
    // Update player positions for teleport detection
    TArray<AActor*> FoundPawns;
    UGameplayStatics::GetAllActorsOfClass(GetWorld(), APawn::StaticClass(), FoundPawns);
    
    for (AActor* Actor : FoundPawns)
    {
        APawn* Pawn = Cast<APawn>(Actor);
        if (Pawn && IsValid(Pawn))
        {
            FVector CurrentPosition = Pawn->GetActorLocation();
            
            // Store previous position for teleport detection
            if (PlayerPreviousPositions.Contains(Pawn))
            {
                FVector PreviousPosition = PlayerPreviousPositions[Pawn];
                float Distance = FVector::Dist(CurrentPosition, PreviousPosition);
                float TimeElapsed = GetWorld()->GetTimeSeconds() - PlayerLastValidationTime.FindOrAdd(Pawn);
                
                // Check for suspicious movement (teleport detection)
                if (TimeElapsed > 0.0f)
                {
                    float Speed = Distance / TimeElapsed;
                    if (Speed > 50.0f) // Suspicious speed threshold
                    {
                        EEntryMethod Method = DetectEntryMethod(Pawn, CurrentPosition, PreviousPosition);
                        
                        // Check if this is an authorized teleport
                        if (Method == EEntryMethod::TELEPORT || Method == EEntryMethod::NOCLIP)
                        {
                            if (!CanPlayerTeleport(Pawn, Method))
                            {
                                HandleInvalidPlayer(Pawn, Method, CurrentPosition);
                            }
                            else
                            {
                                // Log authorized teleport
                                if (bLogMissionTeleports)
                                {
                                    UE_LOG(LogTemp, Log, TEXT("Authorized teleport for player %s"), *Pawn->GetName());
                                }
                            }
                        }
                    }
                }
            }
            
            PlayerPreviousPositions.Add(Pawn, CurrentPosition);
            PlayerLastValidationTime.Add(Pawn, GetWorld()->GetTimeSeconds());
        }
    }
}

bool AInteriorCollisionValidator::ValidatePlayerEntry(APawn* Player, const FVector& EntryPosition)
{
    if (!IsValid(Player))
        return false;

    // Check if entry point is valid
    bool bNearValidEntry = false;
    float DistanceToEntry = 0.0f;
    
    for (const FEntryPointValidation& EntryPoint : ValidEntryPoints)
    {
        float Distance = FVector::Dist(EntryPosition, EntryPoint.Position);
        if (Distance <= EntryPoint.Radius)
        {
            bNearValidEntry = true;
            DistanceToEntry = Distance;
            
            // Check if door interaction is required
            if (EntryPoint.bRequiresDoorInteraction && bEnforceDoorEntry)
            {
                if (!HasValidDoorInteraction(Player))
                {
                    LogViolation(Player, TEXT("No Door Interaction"), EntryPosition);
                    return false;
                }
            }
            
            // Check entry speed
            if (PlayerPreviousPositions.Contains(Player))
            {
                FVector PrevPos = PlayerPreviousPositions[Player];
                float MovementDistance = FVector::Dist(EntryPosition, PrevPos);
                float TimeElapsed = GetWorld()->GetTimeSeconds() - PlayerLastValidationTime.FindOrAdd(Player);
                
                if (TimeElapsed > 0.0f)
                {
                    float Speed = MovementDistance / TimeElapsed;
                    if (Speed > EntryPoint.MaxEntrySpeed)
                    {
                        LogViolation(Player, TEXT("Excessive Entry Speed"), EntryPosition);
                        return false;
                    }
                }
            }
            
            break;
        }
    }
    
    if (!bNearValidEntry)
    {
        LogViolation(Player, TEXT("Invalid Entry Point"), EntryPosition);
        HandleInvalidPlayer(Player, EEntryMethod::INVALID, EntryPosition);
        return false;
    }

    // Mark player as having valid entry
    PlayerHasValidEntry.Add(Player, true);
    PlayerEntryPositions.Add(Player, EntryPosition);
    
    UE_LOG(LogTemp, Log, TEXT("Player %s validated entry at %s"), 
        *Player->GetName(), *EntryPosition.ToString());
    
    return true;
}

bool AInteriorCollisionValidator::IsPositionValid(const FVector& Position, APawn* Player)
{
    // Check mesh penetration
    if (CollisionSettings.bEnableWallPenetrationCheck || 
        CollisionSettings.bEnableFloorPenetrationCheck || 
        CollisionSettings.bEnableCeilingPenetrationCheck)
    {
        FHitResult HitResult;
        if (SphereTraceForCollision(Position, 50.0f, HitResult))
        {
            float PenetrationDepth = GetPenetrationDepth(Player, Position);
            if (PenetrationDepth > CollisionSettings.MaxAllowedPenetration)
            {
                return false;
            }
        }
    }
    
    // Check if position is inside valid interior bounds
    float DistanceToValidation = FVector::Dist(Position, GetActorLocation());
    if (DistanceToValidation > ValidationSphere->GetScaledSphereRadius())
    {
        return false;
    }
    
    return true;
}

EEntryMethod AInteriorCollisionValidator::DetectEntryMethod(APawn* Player, const FVector& CurrentPosition, const FVector& PreviousPosition)
{
    if (!IsValid(Player))
        return EEntryMethod::INVALID;

    float Distance = FVector::Dist(CurrentPosition, PreviousPosition);
    float TimeElapsed = GetWorld()->GetTimeSeconds() - PlayerLastValidationTime.FindOrAdd(Player);
    
    if (TimeElapsed > 0.0f)
    {
        float Speed = Distance / TimeElapsed;
        
        // Check for admin/mission authorization first
        if (IsPlayerAdmin(Player))
        {
            if (Distance > 20.0f && TimeElapsed < 1.0f)
            {
                return EEntryMethod::ADMIN_TELEPORT;
            }
            if (Speed > 50.0f)
            {
                FHitResult HitResult;
                if (LineTraceForCollision(PreviousPosition, CurrentPosition, HitResult))
                {
                    return EEntryMethod::ADMIN_NOCLIP;
                }
            }
        }
        else if (IsPlayerAuthorizedForMissionTeleport(Player))
        {
            if (Distance > 20.0f && TimeElapsed < 1.0f)
            {
                return EEntryMethod::MISSION_TELEPORT;
            }
        }
        
        // Regular teleport detection for unauthorized players
        if (Distance > 20.0f && TimeElapsed < 1.0f)
        {
            return EEntryMethod::TELEPORT;
        }
        
        // High speed movement (possible noclip)
        if (Speed > 50.0f)
        {
            // Check if player went through walls
            FHitResult HitResult;
            if (LineTraceForCollision(PreviousPosition, CurrentPosition, HitResult))
            {
                return EEntryMethod::NOCLIP;
            }
        }
    }
    
    // Check if near valid entry point
    float DistanceToEntry = 0.0f;
    if (IsNearValidEntryPoint(CurrentPosition, DistanceToEntry))
    {
        if (HasValidDoorInteraction(Player))
        {
            return EEntryMethod::DOOR_TRIGGER;
        }
    }
    
    return EEntryMethod::INVALID;
}

FVector AInteriorCollisionValidator::GetNearestValidPosition(const FVector& InvalidPosition)
{
    // Try to find nearest valid entry point
    FVector NearestEntry = MainEntranceFallback;
    float NearestDistance = FVector::Dist(InvalidPosition, MainEntranceFallback);
    
    for (const FEntryPointValidation& EntryPoint : ValidEntryPoints)
    {
        float Distance = FVector::Dist(InvalidPosition, EntryPoint.Position);
        if (Distance < NearestDistance)
        {
            NearestEntry = EntryPoint.Position;
            NearestDistance = Distance;
        }
    }
    
    return NearestEntry;
}

void AInteriorCollisionValidator::CorrectPlayerPosition(APawn* Player, EValidationAction Action)
{
    if (!IsValid(Player))
        return;

    FVector CurrentPosition = Player->GetActorLocation();
    
    switch (Action)
    {
        case EValidationAction::CORRECT_POSITION:
        {
            FVector ValidPosition = GetNearestValidPosition(CurrentPosition);
            Player->SetActorLocation(ValidPosition);
            OnPlayerCorrected.Broadcast(Player);
            UE_LOG(LogTemp, Warning, TEXT("Corrected player %s position to %s"), 
                *Player->GetName(), *ValidPosition.ToString());
            break;
        }
        
        case EValidationAction::RETURN_TO_ENTRY:
        {
            FVector EntryPosition = PlayerEntryPositions.FindOrAdd(Player, MainEntranceFallback);
            Player->SetActorLocation(EntryPosition);
            OnPlayerCorrected.Broadcast(Player);
            UE_LOG(LogTemp, Warning, TEXT("Returned player %s to entry point %s"), 
                *Player->GetName(), *EntryPosition.ToString());
            break;
        }
        
        case EValidationAction::KICK_PLAYER:
        {
            // Move to exterior fallback
            Player->SetActorLocation(ExteriorFallback);
            PlayerHasValidEntry.Remove(Player);
            UE_LOG(LogTemp, Error, TEXT("Kicked player %s to exterior at %s"), 
                *Player->GetName(), *ExteriorFallback.ToString());
            break;
        }
        
        case EValidationAction::LOG_VIOLATION:
        {
            LogViolation(Player, TEXT("Position Violation"), CurrentPosition);
            break;
        }
        
        default:
            break;
    }
}

bool AInteriorCollisionValidator::CheckMeshCollision(APawn* Player, const FVector& Position)
{
    if (!IsValid(Player))
        return false;

    ACharacter* Character = Cast<ACharacter>(Player);
    if (!Character)
        return false;

    UCapsuleComponent* CapsuleComp = Character->GetCapsuleComponent();
    if (!CapsuleComp)
        return false;

    // Check collision at the given position
    FCollisionQueryParams QueryParams;
    QueryParams.AddIgnoredActor(Player);
    QueryParams.bTraceComplex = true;

    FCollisionShape CapsuleShape = FCollisionShape::MakeCapsule(
        CapsuleComp->GetScaledCapsuleRadius(),
        CapsuleComp->GetScaledCapsuleHalfHeight()
    );

    bool bHasCollision = GetWorld()->OverlapBlockingTestByChannel(
        Position,
        FQuat::Identity,
        ECC_WorldStatic,
        CapsuleShape,
        QueryParams
    );

    return bHasCollision;
}

bool AInteriorCollisionValidator::IsPlayerPenetratingMesh(APawn* Player)
{
    if (!IsValid(Player))
        return false;

    FVector PlayerPosition = Player->GetActorLocation();
    return CheckMeshCollision(Player, PlayerPosition);
}

float AInteriorCollisionValidator::GetPenetrationDepth(APawn* Player, const FVector& Position)
{
    if (!IsValid(Player))
        return 0.0f;

    // Simplified penetration depth calculation
    FHitResult HitResult;
    if (SphereTraceForCollision(Position, 50.0f, HitResult))
    {
        return FVector::Dist(Position, HitResult.Location);
    }
    
    return 0.0f;
}

bool AInteriorCollisionValidator::IsNearValidEntryPoint(const FVector& Position, float& DistanceToEntry)
{
    DistanceToEntry = FLT_MAX;
    
    for (const FEntryPointValidation& EntryPoint : ValidEntryPoints)
    {
        float Distance = FVector::Dist(Position, EntryPoint.Position);
        if (Distance <= EntryPoint.Radius)
        {
            DistanceToEntry = Distance;
            return true;
        }
        
        if (Distance < DistanceToEntry)
        {
            DistanceToEntry = Distance;
        }
    }
    
    return false;
}

void AInteriorCollisionValidator::RegisterDoorInteraction(APawn* Player, const FVector& DoorPosition)
{
    if (!IsValid(Player))
        return;

    PlayerHasValidEntry.Add(Player, true);
    PlayerEntryPositions.Add(Player, DoorPosition);
    
    UE_LOG(LogTemp, Log, TEXT("Registered door interaction for player %s at %s"), 
        *Player->GetName(), *DoorPosition.ToString());
}

bool AInteriorCollisionValidator::HasValidDoorInteraction(APawn* Player)
{
    if (!IsValid(Player))
        return false;

    return PlayerHasValidEntry.FindOrAdd(Player, false);
}

void AInteriorCollisionValidator::ValidateAllPlayersInRange()
{
    TArray<AActor*> OverlappingActors;
    ValidationSphere->GetOverlappingActors(OverlappingActors, APawn::StaticClass());
    
    for (AActor* Actor : OverlappingActors)
    {
        APawn* Player = Cast<APawn>(Actor);
        if (IsValid(Player))
        {
            FVector PlayerPosition = Player->GetActorLocation();
            
            // Check if player has valid entry
            if (!PlayerHasValidEntry.FindOrAdd(Player, false))
            {
                // Player is inside without valid entry
                HandleInvalidPlayer(Player, EEntryMethod::INVALID, PlayerPosition);
                continue;
            }
            
            // Check for mesh penetration
            if (IsPlayerPenetratingMesh(Player))
            {
                OnCollisionViolation.Broadcast(Player, PlayerPosition);
                CorrectPlayerPosition(Player, EValidationAction::CORRECT_POSITION);
            }
        }
    }
}

void AInteriorCollisionValidator::HandleInvalidPlayer(APawn* Player, EEntryMethod Method, const FVector& Position)
{
    if (!IsValid(Player))
        return;

    // Increment violation count
    int32& ViolationCount = PlayerViolationCount.FindOrAdd(Player, 0);
    ViolationCount++;
    
    // Broadcast invalid entry event
    OnInvalidEntry.Broadcast(Player, Method, Position);
    
    // Take action based on violation count
    if (ViolationCount >= MaxViolationsBeforeKick)
    {
        CorrectPlayerPosition(Player, EValidationAction::KICK_PLAYER);
        PlayerViolationCount.Remove(Player);
    }
    else
    {
        CorrectPlayerPosition(Player, EValidationAction::RETURN_TO_ENTRY);
    }
    
    LogViolation(Player, TEXT("Invalid Entry Method"), Position);
}

void AInteriorCollisionValidator::LogViolation(APawn* Player, const FString& ViolationType, const FVector& Position)
{
    if (!bLogViolations || !IsValid(Player))
        return;

    UE_LOG(LogTemp, Warning, TEXT("VIOLATION: Player %s - %s at position %s"), 
        *Player->GetName(), *ViolationType, *Position.ToString());
}

bool AInteriorCollisionValidator::LineTraceForCollision(const FVector& Start, const FVector& End, FHitResult& HitResult)
{
    FCollisionQueryParams QueryParams;
    QueryParams.bTraceComplex = true;
    QueryParams.bReturnPhysicalMaterial = false;

    return GetWorld()->LineTraceSingleByChannel(
        HitResult,
        Start,
        End,
        ECC_WorldStatic,
        QueryParams
    );
}

bool AInteriorCollisionValidator::SphereTraceForCollision(const FVector& Position, float Radius, FHitResult& HitResult)
{
    FCollisionQueryParams QueryParams;
    QueryParams.bTraceComplex = true;
    QueryParams.bReturnPhysicalMaterial = false;

    FCollisionShape SphereShape = FCollisionShape::MakeSphere(Radius);

    return GetWorld()->SweepSingleByChannel(
        HitResult,
        Position,
        Position,
        FQuat::Identity,
        ECC_WorldStatic,
        SphereShape,
        QueryParams
    );
}

bool AInteriorCollisionValidator::IsPositionInsideMesh(const FVector& Position)
{
    FHitResult HitResult;
    return SphereTraceForCollision(Position, 10.0f, HitResult);
}

bool AInteriorCollisionValidator::IsPlayerAuthorizedForMissionTeleport(APawn* Player)
{
    if (!IsValid(Player) || !bAllowMissionTeleports)
        return false;

    // Check if player has mission teleport authorization
    bool bHasAuth = PlayerMissionTeleportAuth.FindOrAdd(Player, false);
    
    // Check cooldown
    if (bHasAuth)
    {
        float LastTeleport = PlayerLastMissionTeleport.FindOrAdd(Player, 0.0f);
        float CurrentTime = GetWorld()->GetTimeSeconds();
        
        if (CurrentTime - LastTeleport < MissionTeleportCooldown)
        {
            return false; // Still on cooldown
        }
    }
    
    return bHasAuth;
}

bool AInteriorCollisionValidator::IsPlayerAdmin(APawn* Player)
{
    if (!IsValid(Player) || !bAllowAdminOverrides)
        return false;

    // Check if player is in admin list
    if (AdminPlayers.Contains(Player))
        return true;

    // Check by name
    FString PlayerName = Player->GetName();
    return AdminPlayerNames.Contains(PlayerName);
}

void AInteriorCollisionValidator::AuthorizeMissionTeleport(APawn* Player, const FString& MissionID)
{
    if (!IsValid(Player))
        return;

    PlayerMissionTeleportAuth.Add(Player, true);
    PlayerActiveMissions.Add(Player, MissionID);
    
    UE_LOG(LogTemp, Log, TEXT("Authorized mission teleport for player %s (Mission: %s)"), 
        *Player->GetName(), *MissionID);
}

void AInteriorCollisionValidator::RevokeMissionTeleport(APawn* Player)
{
    if (!IsValid(Player))
        return;

    PlayerMissionTeleportAuth.Remove(Player);
    PlayerActiveMissions.Remove(Player);
    
    UE_LOG(LogTemp, Log, TEXT("Revoked mission teleport for player %s"), *Player->GetName());
}

bool AInteriorCollisionValidator::CanPlayerTeleport(APawn* Player, EEntryMethod Method)
{
    if (!IsValid(Player))
        return false;

    switch (Method)
    {
        case EEntryMethod::MISSION_TELEPORT:
            return IsPlayerAuthorizedForMissionTeleport(Player);
            
        case EEntryMethod::ADMIN_TELEPORT:
        case EEntryMethod::ADMIN_NOCLIP:
            return IsPlayerAdmin(Player);
            
        case EEntryMethod::DOOR_TRIGGER:
        case EEntryMethod::WINDOW_BREAK:
        case EEntryMethod::ELEVATOR:
            return true; // Always allowed
            
        case EEntryMethod::TELEPORT:
        case EEntryMethod::NOCLIP:
        case EEntryMethod::WALL_PHASE:
        default:
            return false; // Not allowed for regular players
    }
}
