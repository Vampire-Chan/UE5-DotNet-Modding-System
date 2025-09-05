#include "Ped.h"
#include "Components/SkeletalMeshComponent.h"
#include "Components/CapsuleComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "EnhancedInputComponent.h"
#include "Factory/PedFactory.h" // Required for FPedSpawnConfiguration

APed::APed()
{
    PrimaryActorTick.bCanEverTick = true;

    // Create core components
    PedInputComponent = CreateDefaultSubobject<UPedInputComponent>(TEXT("PedInputComponent"));
    PedCharacterComponent = CreateDefaultSubobject<UPedCharacterComponent>(TEXT("PedCharacterComponent"));
    PedDataComponent = CreateDefaultSubobject<UPedDataComponent>(TEXT("PedDataComponent"));
    BaseEntityComponent = CreateDefaultSubobject<UBaseEntity>(TEXT("BaseEntityComponent"));

    // Configure default settings
    bIsPlayerControlled = false;
    bIsPedInitialized = false;
    CharacterName = TEXT("Unknown");
}

void APed::BeginPlay()
{
    Super::BeginPlay();
    // Initialization is now handled by InitializePed, called by the factory after deferred spawning.
}

void APed::Tick(float DeltaTime)
{
    Super::Tick(DeltaTime);
}

void APed::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
    Super::SetupPlayerInputComponent(PlayerInputComponent);
    if (bIsPlayerControlled && PedInputComponent)
    {
        if (UEnhancedInputComponent* EnhancedInputComponent = Cast<UEnhancedInputComponent>(PlayerInputComponent))
        {
            PedInputComponent->SetupInputComponent(EnhancedInputComponent);
        }
    }
}

void APed::InitializePed(const FPedSpawnConfiguration& SpawnConfig, const FString& UniqueActorName)
{
    if (bIsPedInitialized)
    {
        UE_LOG(LogTemp, Warning, TEXT("Ped %s already initialized"), *GetName());
        return;
    }

    SetCharacterName(UniqueActorName); // Use the unique name for internal storage
    SetPlayerControlled(SpawnConfig.bPlayerControlled);

    // Setup Entity Identity System
    if (BaseEntityComponent)
    {
        BaseEntityComponent->SetupEntityIdentity(UniqueActorName, EEntityType::Ped, this);
        BaseEntityComponent->AddSearchTag(SpawnConfig.bPlayerControlled ? TEXT("Player") : TEXT("AI"));
        BaseEntityComponent->AddSearchTag(TEXT("Character"));
        // Add other relevant tags from SpawnConfig if available (e.g., AnimationGroup, Personality)
        UE_LOG(LogTemp, Log, TEXT("Ped: Entity Identity System setup for %s - %s"), 
               *UniqueActorName, 
               *BaseEntityComponent->GetDebugString());
    }
    else
    {
        UE_LOG(LogTemp, Warning, TEXT("Ped: No BaseEntityComponent found"));
    }

    // Load data and visuals
    if (PedDataComponent)
    {
        PedDataComponent->LoadBehaviorProfile(SpawnConfig.CharacterName); // Use original CharacterName for profile lookup
    }
    if (PedCharacterComponent)
    {
        PedCharacterComponent->LoadFromXMLData(SpawnConfig.CharacterName); // Use original CharacterName for visual data lookup
        PedCharacterComponent->LoadVariationSet(SpawnConfig.VariationName);
    }

    ConnectComponentSystems();
    SetupAnimationSystem();

    bIsPedInitialized = true;
    UE_LOG(LogTemp, Log, TEXT("Ped initialized: %s with variation %s (Unique Actor Name: %s)"), *SpawnConfig.CharacterName, *SpawnConfig.VariationName, *UniqueActorName);
}

UPedAnimationController* APed::GetPedAnimationController() const
{
    if (GetMesh())
    {
        return Cast<UPedAnimationController>(GetMesh()->GetAnimInstance());
    }
    return nullptr;
}

bool APed::IsPlayer() const
{
    return bIsPlayerControlled;
}

bool APed::IsNPC() const
{
    return !bIsPlayerControlled;
}

void APed::SetPlayerControlled(bool bControlled)
{
    bIsPlayerControlled = bControlled;
}

void APed::SetCharacterName(const FString& NewCharacterName)
{
    CharacterName = NewCharacterName;
#if WITH_EDITOR
    SetActorLabel(NewCharacterName);
#endif
}

FString APed::GetCharacterName() const
{
    return CharacterName;
}

void APed::ConnectComponentSystems()
{
    if (PedInputComponent && PedCharacterComponent)
    {
        UPedAnimationController* AnimController = GetPedAnimationController();
        if (AnimController)
        {
            PedInputComponent->SetAnimationController(AnimController);
        }
    }
}

void APed::SetupAnimationSystem()
{
    if (PedCharacterComponent)
    {
        PedCharacterComponent->SetupAnimationController(GetMesh());
    }
}
