#pragma once

#include "CoreMinimal.h"
#include "UObject/NoExportTypes.h"
#include "Engine/Engine.h"
#include "Core/Enums/GameWorldEnums.h"
#include "BaseEntity.generated.h"

/**
 * Base Entity class that serves as foundation for all game entities
 * This provides common functionality for Peds, Vehicles, and WorldObjects
 * Includes complete identity system for bridging custom logic with Unreal systems
 */
UCLASS(BlueprintType, Blueprintable)
class GAME_API UBaseEntity : public UObject
{
    GENERATED_BODY()

public:
    UBaseEntity();
    virtual ~UBaseEntity();

private:
    // Static ID counter for generating unique entity IDs
    static int32 NextEntityID;

public:
    // ========== IDENTITY SYSTEM ==========
    
    // Core Identity - Our Custom System
    UPROPERTY(EditAnywhere, Category = "Entity Identity")
    int32 EntityID;

    UPROPERTY(EditAnywhere, Category = "Entity Identity")
    FString EntityName; // From XML (e.g., "PlayerNiko")

    UPROPERTY(EditAnywhere, Category = "Entity Identity")
    EEntityType EntityType;

    // Unreal System Integration
    UPROPERTY(EditAnywhere, Category = "Entity Identity")
    FString WorldOutlinerName; // What shows in World Outliner

    UPROPERTY(EditAnywhere, Category = "Entity Identity")
    FString DisplayName; // For UI and debugging (e.g., "Player Niko")

    UPROPERTY(EditAnywhere, Category = "Entity Identity")
    FString UniqueRuntimeID; // Combination ID for cross-system lookup

    // Actor Reference (set by owning actor)
    UPROPERTY(EditAnywhere, Category = "Entity Identity")
    TWeakObjectPtr<AActor> OwnerActor;

    // Search and Classification Tags
    UPROPERTY(EditAnywhere, Category = "Entity Identity")
    TArray<FString> SearchTags; // For queries ["Player", "Niko", "Human", etc.]

    // ========== ENTITY STATE ==========

    // Health & Status
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Entity Health")
    float MaxHealth;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Entity Health")
    float CurrentHealth;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Entity Health")
    bool bIsActive;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Entity Health")
    bool bIsSpawned;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Entity Health")
    bool bIsAlive;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Entity Health")
    bool bIsInvulnerable;

    // Spatial Properties
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Entity Spatial")
    FVector WorldPosition;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Entity Spatial")
    FRotator WorldRotation;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Entity Spatial")
    FVector WorldScale;

    // Entity State
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Entity State")
    EEntityState CurrentState;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Entity State")
    float StateTimer;

    // ========== IDENTITY MANAGEMENT FUNCTIONS ==========

    // Initialize complete entity identity
    void SetupEntityIdentity(const FString& InEntityName, EEntityType InType, AActor* InOwnerActor = nullptr);

    // Generate unique runtime ID for cross-system lookup
    void GenerateUniqueRuntimeID();

    // Set World Outliner name (updates the actual actor name in Unreal)
    void UpdateWorldOutlinerName(const FString& NewName);

    // Add search tags for entity queries
    UFUNCTION(BlueprintCallable, Category = "Entity Identity")
    void AddSearchTag(const FString& Tag);

    void AddSearchTags(const TArray<FString>& Tags);

    // Query functions
    bool HasSearchTag(const FString& Tag) const;

    FString GetFullDisplayName() const; // e.g., "Player Niko"

    FString GetDebugString() const; // Complete identity info for debugging

    // Actor integration
    AActor* GetOwnerActor() const;

    void SetOwnerActor(AActor* NewOwnerActor);

    // Utility functions for enum conversion
    static FString EntityTypeToString(EEntityType Type);
    static EEntityType StringToEntityType(const FString& TypeString);

    // ========== CORE ENTITY FUNCTIONS ==========

    // Entity Functions
    UFUNCTION(BlueprintCallable, Category = "Entity Core")
    virtual void InitializeEntity();

    UFUNCTION(BlueprintCallable, Category = "Entity Core")
    virtual void SpawnEntity(const FVector& SpawnLocation, const FRotator& SpawnRotation);

    UFUNCTION(BlueprintCallable, Category = "Entity Core")
    virtual void DestroyEntity();

    UFUNCTION(BlueprintCallable, Category = "Entity Core")
    virtual void SetEntityState(EEntityState NewState);

    // Health Functions
    UFUNCTION(BlueprintCallable, Category = "Entity Health")
    virtual void TakeDamage(float DamageAmount, AActor* DamageSource = nullptr);

    UFUNCTION(BlueprintCallable, Category = "Entity Health")
    virtual void RestoreHealth(float HealAmount);

    UFUNCTION(BlueprintCallable, Category = "Entity Health")
    virtual void SetMaxHealth(float NewMaxHealth);

    // Utility Functions
    UFUNCTION(BlueprintCallable, BlueprintPure, Category = "Entity Core")
    bool IsEntityValid() const;

    UFUNCTION(BlueprintCallable, BlueprintPure, Category = "Entity Health")
    float GetHealthPercentage() const;

    UFUNCTION(BlueprintCallable, BlueprintPure, Category = "Entity Spatial")
    float GetDistanceToLocation(const FVector& TargetLocation) const;

    UFUNCTION(BlueprintCallable, BlueprintPure, Category = "Entity Spatial")
    float GetDistanceToEntity(const UBaseEntity* OtherEntity) const;

protected:
    // Internal update function - called by derived classes
    virtual void UpdateEntity(float DeltaTime);

    // Event functions for derived classes to override
    virtual void OnEntitySpawned();
    virtual void OnEntityDestroyed();
    virtual void OnEntityStateChanged(EEntityState OldState, EEntityState NewState);
    virtual void OnEntityDamaged(float DamageAmount, AActor* DamageSource);
    virtual void OnEntityHealed(float HealAmount);
    virtual void OnEntityDeath();

    // Validation helpers
    bool ValidateEntityData() const;
    void LogEntityStatus(const FString& Message) const;

private:
    // Internal state tracking
    EEntityState PreviousState;
    float LastUpdateTime;
    bool bEntityInitialized;
};
