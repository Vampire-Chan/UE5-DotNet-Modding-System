#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "../Core/Entity/BaseEntity.h"
#include "../Core/Enums/GameWorldEnums.h"
#include "Locomotion/PedInputComponent.h"
#include "Character/PedCharacterComponent.h"
#include "Data/PedDataComponent.h"
#include "../Animation/PedAnimationController.h"

// Forward Declarations
struct FPedSpawnConfiguration;

#include "Ped.generated.h"

/**
 * Main Ped class that combines all modular systems:
 * - BaseEntity functionality (health, state management)
 * - Input handling and movement (PedInputComponent)
 * - Visual appearance and meshes (PedCharacterComponent)  
 * - Behavioral data and attributes (PedDataComponent)
 * - Animation control (PedAnimationController)
 */
UCLASS(BlueprintType, Blueprintable)
class GAME_API APed : public ACharacter
{
    GENERATED_BODY()

public:
    APed();

protected:
    virtual void BeginPlay() override;

public:
    virtual void Tick(float DeltaTime) override;
    virtual void SetupPlayerInputComponent(UInputComponent* PlayerInputComponent) override;

    // Ped-specific functionality
    void InitializePed(const FPedSpawnConfiguration& SpawnConfig, const FString& UniqueActorName);

    // Component access
    UPedInputComponent* GetPedInputComponent() const { return PedInputComponent; }
    UPedCharacterComponent* GetPedCharacterComponent() const { return PedCharacterComponent; }
    UPedDataComponent* GetPedDataComponent() const { return PedDataComponent; }
    UBaseEntity* GetBaseEntityComponent() const { return BaseEntityComponent; }
    UPedAnimationController* GetPedAnimationController() const;

    // State queries
    bool IsPlayer() const;
    bool IsNPC() const;

    // Player control methods
    void SetPlayerControlled(bool bControlled);
    bool GetIsPlayerControlled() const { return bIsPlayerControlled; }

    // Factory support methods
    void SetCharacterName(const FString& NewCharacterName);
    FString GetCharacterName() const;

protected:
    // Core components
    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
    UPedInputComponent* PedInputComponent;

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
    UPedCharacterComponent* PedCharacterComponent;

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
    UPedDataComponent* PedDataComponent;

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
    UBaseEntity* BaseEntityComponent;

    // Configuration
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Configuration")
    bool bIsPlayerControlled;

    // State
    UPROPERTY(BlueprintReadOnly, Category = "State")
    bool bIsPedInitialized;

    UPROPERTY(BlueprintReadOnly, Category = "State")
    FString CharacterName;

private:
    // Internal initialization
    void ConnectComponentSystems();
    void SetupAnimationSystem();
};
