#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Components/SphereComponent.h"
#include "Components/StaticMeshComponent.h"
#include "Engine/TriggerVolume.h"
#include "Core/Enums/GameWorldEnums.h"
#include "InteriorCollisionValidator.generated.h"
USTRUCT(BlueprintType)
struct FEntryPointValidation
{
    GENERATED_BODY()

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Validation")
    FVector Position;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Validation")
    float Radius;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Validation")
    bool bRequiresDoorInteraction;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Validation")
    float MaxEntrySpeed;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Validation")
    TArray<EEntryMethod> AllowedEntryMethods;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Validation")
    TArray<EEntryMethod> MissionAllowedMethods;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Validation")
    TArray<EEntryMethod> AdminAllowedMethods;

    FEntryPointValidation()
    {
        Position = FVector::ZeroVector;
        Radius = 3.0f;
        bRequiresDoorInteraction = true;
        MaxEntrySpeed = 10.0f;
        AllowedEntryMethods = { EEntryMethod::DOOR_TRIGGER };
        MissionAllowedMethods = { EEntryMethod::DOOR_TRIGGER, EEntryMethod::MISSION_TELEPORT };
        AdminAllowedMethods = { EEntryMethod::DOOR_TRIGGER, EEntryMethod::ADMIN_TELEPORT, EEntryMethod::ADMIN_NOCLIP };
    }
};

USTRUCT(BlueprintType)
struct FCollisionValidationSettings
{
    GENERATED_BODY()

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Collision")
    bool bEnableWallPenetrationCheck;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Collision")
    bool bEnableFloorPenetrationCheck;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Collision")
    bool bEnableCeilingPenetrationCheck;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Collision")
    float MaxAllowedPenetration;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Collision")
    float ValidationFrequency;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Collision")
    bool bContinuousValidation;

    FCollisionValidationSettings()
    {
        bEnableWallPenetrationCheck = true;
        bEnableFloorPenetrationCheck = true;
        bEnableCeilingPenetrationCheck = true;
        MaxAllowedPenetration = 0.5f;
        ValidationFrequency = 0.1f;
        bContinuousValidation = true;
    }
};

DECLARE_DYNAMIC_MULTICAST_DELEGATE_ThreeParams(FOnInvalidEntry, class APawn*, Player, EEntryMethod, Method, FVector, Position);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FOnCollisionViolation, class APawn*, Player, FVector, ViolationPosition);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnPlayerCorrected, class APawn*, Player);

UCLASS()
class GAME_API AInteriorCollisionValidator : public AActor
{
    GENERATED_BODY()
    
public:    
    AInteriorCollisionValidator();

protected:
    virtual void BeginPlay() override;
    virtual void Tick(float DeltaTime) override;

    // ========== COMPONENTS ==========
    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
    class USphereComponent* ValidationSphere;

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
    class UStaticMeshComponent* InteriorMesh;

    // ========== VALIDATION SETTINGS ==========
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Validation")
    FCollisionValidationSettings CollisionSettings;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Validation")
    TArray<FEntryPointValidation> ValidEntryPoints;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Validation")
    bool bEnforceDoorEntry;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Validation")
    bool bLogViolations;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Validation")
    int32 MaxViolationsBeforeKick;

    // ========== MISSION/ADMIN SETTINGS ==========
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Mission")
    bool bAllowMissionTeleports;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Mission")
    bool bAllowAdminOverrides;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Mission")
    TArray<FString> AuthorizedMissionTeleporters;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Mission")
    TArray<FString> AdminPlayerNames;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Mission")
    float MissionTeleportCooldown;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Mission")
    bool bLogMissionTeleports;

    // ========== FALLBACK POSITIONS ==========
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Fallback")
    FVector MainEntranceFallback;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Fallback")
    FVector ExteriorFallback;

    // ========== EVENTS ==========
    UPROPERTY(BlueprintAssignable, Category = "Events")
    FOnInvalidEntry OnInvalidEntry;

    UPROPERTY(BlueprintAssignable, Category = "Events")
    FOnCollisionViolation OnCollisionViolation;

    UPROPERTY(BlueprintAssignable, Category = "Events")
    FOnPlayerCorrected OnPlayerCorrected;

public:
    // ========== VALIDATION FUNCTIONS ==========
    UFUNCTION(BlueprintCallable, Category = "Validation")
    bool ValidatePlayerEntry(class APawn* Player, const FVector& EntryPosition);

    UFUNCTION(BlueprintCallable, Category = "Validation")
    bool IsPositionValid(const FVector& Position, class APawn* Player = nullptr);

    UFUNCTION(BlueprintCallable, Category = "Validation")
    EEntryMethod DetectEntryMethod(class APawn* Player, const FVector& CurrentPosition, const FVector& PreviousPosition);

    UFUNCTION(BlueprintCallable, Category = "Validation")
    FVector GetNearestValidPosition(const FVector& InvalidPosition);

    UFUNCTION(BlueprintCallable, Category = "Validation")
    void CorrectPlayerPosition(class APawn* Player, EValidationAction Action);

    // ========== MISSION/ADMIN FUNCTIONS ==========
    UFUNCTION(BlueprintCallable, Category = "Mission")
    bool IsPlayerAuthorizedForMissionTeleport(class APawn* Player);

    UFUNCTION(BlueprintCallable, Category = "Mission")
    bool IsPlayerAdmin(class APawn* Player);

    UFUNCTION(BlueprintCallable, Category = "Mission")
    void AuthorizeMissionTeleport(class APawn* Player, const FString& MissionID);

    UFUNCTION(BlueprintCallable, Category = "Mission")
    void RevokeMissionTeleport(class APawn* Player);

    UFUNCTION(BlueprintCallable, Category = "Mission")
    bool CanPlayerTeleport(class APawn* Player, EEntryMethod Method);

    // ========== COLLISION CHECKS ==========
    UFUNCTION(BlueprintCallable, Category = "Collision")
    bool CheckMeshCollision(class APawn* Player, const FVector& Position);

    UFUNCTION(BlueprintCallable, Category = "Collision")
    bool IsPlayerPenetratingMesh(class APawn* Player);

    UFUNCTION(BlueprintCallable, Category = "Collision")
    float GetPenetrationDepth(class APawn* Player, const FVector& Position);

    // ========== DOOR INTERACTION ==========
    UFUNCTION(BlueprintCallable, Category = "Door")
    bool IsNearValidEntryPoint(const FVector& Position, float& DistanceToEntry);

    UFUNCTION(BlueprintCallable, Category = "Door")
    void RegisterDoorInteraction(class APawn* Player, const FVector& DoorPosition);

    UFUNCTION(BlueprintCallable, Category = "Door")
    bool HasValidDoorInteraction(class APawn* Player);

protected:
    // ========== INTERNAL FUNCTIONS ==========
    void ValidateAllPlayersInRange();
    void HandleInvalidPlayer(class APawn* Player, EEntryMethod Method, const FVector& Position);
    void LogViolation(class APawn* Player, const FString& ViolationType, const FVector& Position);
    
    // ========== COLLISION DETECTION ==========
    bool LineTraceForCollision(const FVector& Start, const FVector& End, FHitResult& HitResult);
    bool SphereTraceForCollision(const FVector& Position, float Radius, FHitResult& HitResult);
    bool IsPositionInsideMesh(const FVector& Position);

private:
    // ========== TRACKING DATA ==========
    TMap<class APawn*, FVector> PlayerPreviousPositions;
    TMap<class APawn*, float> PlayerLastValidationTime;
    TMap<class APawn*, int32> PlayerViolationCount;
    TMap<class APawn*, bool> PlayerHasValidEntry;
    TMap<class APawn*, FVector> PlayerEntryPositions;

    // ========== MISSION/ADMIN TRACKING ==========
    TMap<class APawn*, bool> PlayerMissionTeleportAuth;
    TMap<class APawn*, FString> PlayerActiveMissions;
    TMap<class APawn*, float> PlayerLastMissionTeleport;
    TSet<class APawn*> AdminPlayers;

    // ========== TIMERS ==========
    FTimerHandle ValidationTimerHandle;
    float LastValidationTime;
};
