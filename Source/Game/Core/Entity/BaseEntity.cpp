#include "BaseEntity.h"
#include "Engine/World.h"
#include "Engine/Engine.h"
#include "TimerManager.h"

// Initialize static ID counter
int32 UBaseEntity::NextEntityID = 1;

UBaseEntity::UBaseEntity()
{
    // ========== ENTITY IDENTITY SYSTEM ==========
    // Generate unique Entity ID automatically
    EntityID = NextEntityID++;
    EntityName = FString::Printf(TEXT("Entity_%d"), EntityID);
    EntityType = EEntityType::WorldObject;
    WorldOutlinerName = EntityName;
    DisplayName = EntityName;
    UniqueRuntimeID = "";
    OwnerActor = nullptr;
    SearchTags.Empty();
    
    // ========== EXISTING SYSTEM ==========
    bIsActive = true;
    bIsSpawned = false;
    
    // Health defaults
    MaxHealth = 100.0f;
    CurrentHealth = MaxHealth;
    bIsAlive = true;
    bIsInvulnerable = false;
    
    // Spatial defaults
    WorldPosition = FVector::ZeroVector;
    WorldRotation = FRotator::ZeroRotator;
    WorldScale = FVector::OneVector;
    
    // State defaults
    CurrentState = EEntityState::Idle;
    PreviousState = EEntityState::Idle;
    StateTimer = 0.0f;
    
    // Internal flags
    bEntityInitialized = false;
    LastUpdateTime = 0.0f;
    
    // Generate unique runtime ID
    GenerateUniqueRuntimeID();
}

UBaseEntity::~UBaseEntity()
{
    // Clean up identity system
    OwnerActor = nullptr;
    SearchTags.Empty();
}

// ========== ENTITY IDENTITY MANAGEMENT FUNCTIONS ==========

void UBaseEntity::SetupEntityIdentity(const FString& InEntityName, EEntityType InType, AActor* InOwnerActor)
{
    // Set basic identity
    EntityName = InEntityName;
    EntityType = InType;
    OwnerActor = InOwnerActor;
    
    // Set display name (can be customized later)
    DisplayName = InEntityName;
    
    // Generate unique runtime ID
    GenerateUniqueRuntimeID();
    
    // Set World Outliner name with ID suffix for uniqueness
    WorldOutlinerName = FString::Printf(TEXT("%s_%d"), *InEntityName, EntityID);
    
    // Update the actual actor name if we have one
    if (OwnerActor.IsValid())
    {
#if WITH_EDITOR
        OwnerActor->SetActorLabel(WorldOutlinerName);
#endif
    }
    
    // Add default search tags
    SearchTags.Empty();
    SearchTags.Add(InEntityName);
    SearchTags.Add(EntityTypeToString(InType));
    
    FString TypeString = EntityTypeToString(EntityType);
    UE_LOG(LogTemp, Log, TEXT("BaseEntity: Setup identity for %s (ID: %d, Type: %s)"), 
           *EntityName, EntityID, *TypeString);
}

void UBaseEntity::GenerateUniqueRuntimeID()
{
    // Create a unique runtime ID combining entity name, type, and timestamp
    FDateTime Now = FDateTime::Now();
    FString TypeString = EntityTypeToString(EntityType);
    UniqueRuntimeID = EntityName + TEXT("_") + TypeString + TEXT("_") + 
                      FString::FromInt(EntityID) + TEXT("_") + 
                      FString::Printf(TEXT("%lld"), Now.GetTicks());
}

void UBaseEntity::UpdateWorldOutlinerName(const FString& NewName)
{
    WorldOutlinerName = NewName;
    
    // Update the actual actor name if we have one
    if (OwnerActor.IsValid())
    {
#if WITH_EDITOR
        OwnerActor->SetActorLabel(WorldOutlinerName);
#endif
        UE_LOG(LogTemp, Log, TEXT("BaseEntity: Updated World Outliner name to %s for entity %s"), 
               *WorldOutlinerName, *EntityName);
    }
}

void UBaseEntity::AddSearchTag(const FString& Tag)
{
    if (!Tag.IsEmpty() && !SearchTags.Contains(Tag))
    {
        SearchTags.Add(Tag);
        UE_LOG(LogTemp, Verbose, TEXT("BaseEntity: Added search tag '%s' to entity %s"), 
               *Tag, *EntityName);
    }
}

void UBaseEntity::AddSearchTags(const TArray<FString>& Tags)
{
    for (const FString& Tag : Tags)
    {
        AddSearchTag(Tag);
    }
}

bool UBaseEntity::HasSearchTag(const FString& Tag) const
{
    return SearchTags.Contains(Tag);
}

FString UBaseEntity::GetFullDisplayName() const
{
    // Return display name, fallback to entity name if display name is empty
    if (!DisplayName.IsEmpty())
    {
        return DisplayName;
    }
    return EntityName;
}

FString UBaseEntity::GetDebugString() const
{
    FString TagsString = FString::Join(SearchTags, TEXT(", "));
    FString TypeString = EntityTypeToString(EntityType);
    
    return FString::Printf(TEXT("Entity[ID:%d, Name:'%s', Type:%s, WorldName:'%s', Display:'%s', RuntimeID:'%s', Tags:[%s]]"),
                           EntityID,
                           *EntityName,
                           *TypeString,
                           *WorldOutlinerName,
                           *DisplayName,
                           *UniqueRuntimeID,
                           *TagsString);
}

AActor* UBaseEntity::GetOwnerActor() const
{
    return OwnerActor.Get();
}

void UBaseEntity::SetOwnerActor(AActor* NewOwnerActor)
{
    OwnerActor = NewOwnerActor;
    
    // Update the actor label if we have a world outliner name set
    if (OwnerActor.IsValid() && !WorldOutlinerName.IsEmpty())
    {
#if WITH_EDITOR
        OwnerActor->SetActorLabel(WorldOutlinerName);
#endif
    }
    
    UE_LOG(LogTemp, Log, TEXT("BaseEntity: Set owner actor for entity %s"), *EntityName);
}

FString UBaseEntity::EntityTypeToString(EEntityType Type)
{
    return UEnum::GetValueAsString(Type);
}

EEntityType UBaseEntity::StringToEntityType(const FString& TypeString)
{
    UEnum* EntityTypeEnum = StaticEnum<EEntityType>();
    if (EntityTypeEnum)
    {
        int64 Value = EntityTypeEnum->GetValueByName(FName(*TypeString));
        if (Value != INDEX_NONE)
        {
            return static_cast<EEntityType>(Value);
        }
    }
    return EEntityType::Invalid;
}

// ========== EXISTING ENTITY SYSTEM ==========

void UBaseEntity::InitializeEntity()
{
    if (bEntityInitialized)
    {
        UE_LOG(LogTemp, Warning, TEXT("BaseEntity: Entity %s is already initialized"), *EntityName);
        return;
    }
    
    // Validate entity data
    if (!ValidateEntityData())
    {
        UE_LOG(LogTemp, Error, TEXT("BaseEntity: Entity %s failed validation during initialization"), *EntityName);
        return;
    }
    
    // Set initial state
    CurrentHealth = MaxHealth;
    bIsAlive = true;
    CurrentState = EEntityState::Idle;
    StateTimer = 0.0f;
    
    bEntityInitialized = true;
    
    LogEntityStatus(TEXT("Initialized successfully"));
}

void UBaseEntity::SpawnEntity(const FVector& SpawnLocation, const FRotator& SpawnRotation)
{
    if (!bEntityInitialized)
    {
        InitializeEntity();
    }
    
    if (bIsSpawned)
    {
        UE_LOG(LogTemp, Warning, TEXT("BaseEntity: Entity %s is already spawned"), *EntityName);
        return;
    }
    
    // Set spawn transform
    WorldPosition = SpawnLocation;
    WorldRotation = SpawnRotation;
    
    bIsSpawned = true;
    bIsActive = true;
    
    // Call derived class event
    OnEntitySpawned();
    
    LogEntityStatus(FString::Printf(TEXT("Spawned at location %s"), *SpawnLocation.ToString()));
}

void UBaseEntity::DestroyEntity()
{
    if (!bIsSpawned)
    {
        UE_LOG(LogTemp, Warning, TEXT("BaseEntity: Entity %s is not spawned, cannot destroy"), *EntityName);
        return;
    }
    
    // Set death state if still alive
    if (bIsAlive)
    {
        bIsAlive = false;
        OnEntityDeath();
    }
    
    bIsSpawned = false;
    bIsActive = false;
    
    // Call derived class event
    OnEntityDestroyed();
    
    LogEntityStatus(TEXT("Destroyed"));
}

void UBaseEntity::SetEntityState(EEntityState NewState)
{
    if (CurrentState == NewState)
    {
        return;
    }
    
    EEntityState OldState = CurrentState;
    PreviousState = CurrentState;
    CurrentState = NewState;
    StateTimer = 0.0f;
    
    // Call derived class event
    OnEntityStateChanged(OldState, NewState);
    
    UE_LOG(LogTemp, VeryVerbose, TEXT("BaseEntity: Entity %s state changed from %d to %d"), 
           *EntityName, (int32)OldState, (int32)NewState);
}

void UBaseEntity::TakeDamage(float DamageAmount, AActor* DamageSource)
{
    if (!bIsAlive || bIsInvulnerable || DamageAmount <= 0.0f)
    {
        return;
    }
    
    float PreviousHealth = CurrentHealth;
    CurrentHealth = FMath::Max(0.0f, CurrentHealth - DamageAmount);
    
    // Call derived class event
    OnEntityDamaged(DamageAmount, DamageSource);
    
    // Check for death
    if (CurrentHealth <= 0.0f && bIsAlive)
    {
        bIsAlive = false;
        SetEntityState(EEntityState::Dead);
        OnEntityDeath();
    }
    
    UE_LOG(LogTemp, Log, TEXT("BaseEntity: Entity %s took %.1f damage (%.1f -> %.1f HP)"), 
           *EntityName, DamageAmount, PreviousHealth, CurrentHealth);
}

void UBaseEntity::RestoreHealth(float HealAmount)
{
    if (!bIsAlive || HealAmount <= 0.0f)
    {
        return;
    }
    
    float PreviousHealth = CurrentHealth;
    CurrentHealth = FMath::Min(MaxHealth, CurrentHealth + HealAmount);
    
    // Call derived class event
    OnEntityHealed(HealAmount);
    
    UE_LOG(LogTemp, Log, TEXT("BaseEntity: Entity %s healed %.1f HP (%.1f -> %.1f HP)"), 
           *EntityName, HealAmount, PreviousHealth, CurrentHealth);
}

void UBaseEntity::SetMaxHealth(float NewMaxHealth)
{
    if (NewMaxHealth <= 0.0f)
    {
        UE_LOG(LogTemp, Warning, TEXT("BaseEntity: Invalid max health value: %.1f"), NewMaxHealth);
        return;
    }
    
    float HealthPercentage = GetHealthPercentage();
    MaxHealth = NewMaxHealth;
    CurrentHealth = MaxHealth * (HealthPercentage / 100.0f);
    
    UE_LOG(LogTemp, Log, TEXT("BaseEntity: Entity %s max health set to %.1f (current: %.1f)"), 
           *EntityName, MaxHealth, CurrentHealth);
}

bool UBaseEntity::IsEntityValid() const
{
    return bEntityInitialized && bIsSpawned && bIsActive;
}

float UBaseEntity::GetHealthPercentage() const
{
    if (MaxHealth <= 0.0f)
    {
        return 0.0f;
    }
    return (CurrentHealth / MaxHealth) * 100.0f;
}

float UBaseEntity::GetDistanceToLocation(const FVector& TargetLocation) const
{
    return FVector::Dist(WorldPosition, TargetLocation);
}

float UBaseEntity::GetDistanceToEntity(const UBaseEntity* OtherEntity) const
{
    if (!OtherEntity)
    {
        return -1.0f;
    }
    return GetDistanceToLocation(OtherEntity->WorldPosition);
}

void UBaseEntity::UpdateEntity(float DeltaTime)
{
    if (!IsEntityValid())
    {
        return;
    }
    
    StateTimer += DeltaTime;
    LastUpdateTime = GetWorld() ? GetWorld()->GetTimeSeconds() : 0.0f;
}

void UBaseEntity::OnEntitySpawned()
{
    // Base implementation - override in derived classes
}

void UBaseEntity::OnEntityDestroyed()
{
    // Base implementation - override in derived classes
}

void UBaseEntity::OnEntityStateChanged(EEntityState OldState, EEntityState NewState)
{
    // Base implementation - override in derived classes
}

void UBaseEntity::OnEntityDamaged(float DamageAmount, AActor* DamageSource)
{
    // Base implementation - override in derived classes
}

void UBaseEntity::OnEntityHealed(float HealAmount)
{
    // Base implementation - override in derived classes
}

void UBaseEntity::OnEntityDeath()
{
    // Base implementation - override in derived classes
}

bool UBaseEntity::ValidateEntityData() const
{
    if (EntityName.IsEmpty())
    {
        UE_LOG(LogTemp, Warning, TEXT("BaseEntity: Entity has empty name"));
        return false;
    }
    
    if (MaxHealth <= 0.0f)
    {
        UE_LOG(LogTemp, Warning, TEXT("BaseEntity: Entity %s has invalid max health: %.1f"), *EntityName, MaxHealth);
        return false;
    }
    
    return true;
}

void UBaseEntity::LogEntityStatus(const FString& Message) const
{
    UE_LOG(LogTemp, Log, TEXT("BaseEntity [%s]: %s"), *EntityName, *Message);
}
