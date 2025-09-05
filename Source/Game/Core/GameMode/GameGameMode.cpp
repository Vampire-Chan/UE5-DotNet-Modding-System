#include "GameGameMode.h"
#include "../../Peds/Ped.h"
#include "../../Peds/Factory/PedFactory.h"
#include "../../Peds/Character/PedCharacterComponent.h"
#include "../../Peds/Locomotion/PedInputComponent.h"
#include "../../Tasks/TaskFactory.h"
#include "../../Tasks/TaskManager.h"
#include "../../Core/Utils/GameLogger.h"
#include "../../Animation/AnimationGroupsLoader.h"
#include "Engine/World.h"
#include "Engine/SkeletalMesh.h"
#include "Components/SkeletalMeshComponent.h"
#include "Materials/MaterialInterface.h"
#include "Materials/MaterialInstanceDynamic.h"
#include "Engine/Texture2D.h"
#include "EnhancedInputComponent.h"
#include "EnhancedInputSubsystems.h"
#include "InputMappingContext.h"
#include "InputAction.h"
#include "Kismet/GameplayStatics.h"
#include "GameFramework/PlayerController.h"
#include "GameFramework/PlayerStart.h"
#include "EngineUtils.h"
#include "Math/UnrealMathUtility.h"
#include "DrawDebugHelpers.h"

AGameGameMode::AGameGameMode()
{
    PrimaryActorTick.bCanEverTick = true;

    // Initialize default values
    CurrentGameState = EGameState::PreGame;
    GameTime = 0.0f;
    bGameHasStarted = false;

    // Player settings
    PlayerSpawnLocation = FVector(0.0f, 0.0f, 100.0f);
    PlayerSpawnRotation = FRotator::ZeroRotator;
    bUsePlayerStart = true;
    bSpawnAtCustomLocation = false;

    // AI settings
    MaxAIPeds = 10;
    AIUpdateInterval = 0.5f;
    AISpawnRadius = 2000.0f;

    // Asset path defaults
    CharacterBasePath = TEXT("/Game/Characters");
    MaterialBasePath = TEXT("/Game/Characters/CharacterShaders");
    InputBasePath = TEXT("/Game/Input");

    // Initialize default configurations
    DefaultPlayerConfig.CharacterName = TEXT("PlayerNiko");
    DefaultPlayerConfig.VariantIndex = TEXT("000");
    DefaultPlayerConfig.BodyParts = { TEXT("head"), TEXT("uppr"), TEXT("lowr"), TEXT("hand"), TEXT("feet") };

    DefaultAIConfig = DefaultPlayerConfig; // Same config for AI initially

    // Initialize arrays
    AICharacters.Empty();
    AIPeds.Empty();

    // Initialize pointers
    PlayerCharacter = nullptr;
    PedFactory = nullptr;
    TaskFactory = nullptr;
    AnimationLoader = nullptr;
}

void AGameGameMode::BeginPlay()
{
    Super::BeginPlay();

    // Initialize custom logger
    FGameLogger::Initialize();
    GAME_LOG(TEXT("GameGameMode: BeginPlay started"));

    // Initialize factories
    InitializeFactories();

    // Set initial game state
    SetGameState(EGameState::Starting);

    // Start cleanup timer
    GetWorldTimerManager().SetTimer(CleanupTimer, this, &AGameGameMode::CleanupInvalidActors, 5.0f, true);

    // Start the game setup
    FTimerHandle StartupTimer;
    GetWorldTimerManager().SetTimer(StartupTimer, [this]()
        {
            StartGameLoop();
        }, 1.0f, false);
}

void AGameGameMode::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
    SetGameState(EGameState::Ending);

    // Clear all timers
    GetWorldTimerManager().ClearTimer(AIUpdateTimer);
    GetWorldTimerManager().ClearTimer(GameStateTimer);
    GetWorldTimerManager().ClearTimer(CleanupTimer);

    Super::EndPlay(EndPlayReason);
}

void AGameGameMode::Tick(float DeltaTime)
{
    Super::Tick(DeltaTime);

    if (IsGameInProgress())
    {
        UpdateGameTime(DeltaTime);
    }
}

// ========== GAME STATE MANAGEMENT ==========
void AGameGameMode::SetGameState(EGameState NewState)
{
    if (CurrentGameState != NewState)
    {
        EGameState OldState = CurrentGameState;
        CurrentGameState = NewState;

        UE_LOG(LogTemp, Log, TEXT("GameGameMode: State changed from %d to %d"), (int32)OldState, (int32)NewState);

        // Broadcast state change
        OnGameStateChanged.Broadcast(NewState);

        // Handle state-specific logic
        switch (NewState)
        {
        case EGameState::Starting:
            bGameHasStarted = false;
            break;

        case EGameState::InProgress:
            bGameHasStarted = true;
            // Start AI update timer
            GetWorldTimerManager().SetTimer(AIUpdateTimer, this, &AGameGameMode::UpdateAIBehaviors, AIUpdateInterval, true);
            break;

        case EGameState::Paused:
            GetWorldTimerManager().PauseTimer(AIUpdateTimer);
            break;

        case EGameState::Ending:
            GetWorldTimerManager().ClearTimer(AIUpdateTimer);
            break;

        default:
            break;
        }
    }
}

// ========== ENHANCED CHARACTER SPAWNING ==========
ACharacter* AGameGameMode::SpawnModularCharacter(const FVector& SpawnLocation, const FCharacterVariantConfig& Config, bool bPlayerControlled)
{
    UE_LOG(LogTemp, Warning, TEXT("=== Spawning Modular Character: %s ==="), *Config.CharacterName);

    if (!GetWorld())
    {
        UE_LOG(LogTemp, Error, TEXT("No valid world context"));
        return nullptr;
    }

    // Check for world readiness
    if (!GetWorld()->AreActorsInitialized())
    {
        UE_LOG(LogTemp, Warning, TEXT("World actors not initialized, delaying spawn"));
        FTimerHandle DelayTimer;
        GetWorldTimerManager().SetTimer(DelayTimer, [this, SpawnLocation, Config, bPlayerControlled]()
            {
                SpawnModularCharacter(SpawnLocation, Config, bPlayerControlled);
            }, 0.5f, false);
        return nullptr;
    }

    // Spawn basic character
    FActorSpawnParameters SpawnParams;
    SpawnParams.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AdjustIfPossibleButAlwaysSpawn;
    SpawnParams.Name = FName(*FString::Printf(TEXT("%s_ModularCharacter"), *Config.CharacterName));

    ACharacter* NewCharacter = GetWorld()->SpawnActor<ACharacter>(ACharacter::StaticClass(), SpawnLocation, FRotator::ZeroRotator, SpawnParams);
    if (!NewCharacter)
    {
        UE_LOG(LogTemp, Error, TEXT("Failed to spawn character"));
        return nullptr;
    }

    USkeletalMeshComponent* MainMesh = NewCharacter->GetMesh();
    if (!MainMesh)
    {
        UE_LOG(LogTemp, Error, TEXT("Character has no mesh component"));
        return NewCharacter;
    }

    // Load master skeleton - FIXED
    FString SkeletonPath = FString::Printf(TEXT("%s/%s/SKEL_%s"), *CharacterBasePath, *Config.CharacterName, *Config.CharacterName);
    USkeleton* MasterSkeleton = LoadObject<USkeleton>(nullptr, *SkeletonPath);

    if (!MasterSkeleton)
    {
        UE_LOG(LogTemp, Error, TEXT("Failed to load master skeleton: %s"), *SkeletonPath);
        return NewCharacter;
    }

    // Load modular components
    TArray<FModularComponentInfo> ComponentInfos;
    bool bAllComponentsLoaded = true;

    for (const FString& BodyPart : Config.BodyParts)
    {
        if (!LoadCharacterComponent(NewCharacter, BodyPart, Config.VariantIndex, ComponentInfos))
        {
            UE_LOG(LogTemp, Warning, TEXT("Failed to load component: %s"), *BodyPart);
            bAllComponentsLoaded = false;
        }
    }

    if (ComponentInfos.Num() > 0)
    {
        // Set up master pose system
        SetupMasterPoseComponent(NewCharacter, ComponentInfos);

        // Set up enhanced input for player characters
        if (bPlayerControlled)
        {
            SetupEnhancedInput(NewCharacter);
            GivePlayerControl(NewCharacter);
            PlayerCharacter = NewCharacter;
        }
        else
        {
            AICharacters.Add(NewCharacter);
        }

        // Broadcast character spawned event
        OnCharacterSpawned.Broadcast(NewCharacter, bPlayerControlled);

        LogCharacterSpawnInfo(NewCharacter, Config);
    }

    UE_LOG(LogTemp, Warning, TEXT("=== Modular Character Spawn Complete ==="));
    return NewCharacter;
}

void AGameGameMode::SpawnPlayerCharacter()
{
    // Determine spawn location
    FVector SpawnLocation = PlayerSpawnLocation;
    FRotator SpawnRotation = PlayerSpawnRotation;

    if (bUsePlayerStart && !bSpawnAtCustomLocation)
    {
        for (TActorIterator<APlayerStart> ActorItr(GetWorld()); ActorItr; ++ActorItr)
        {
            APlayerStart* PlayerStart = *ActorItr;
            if (PlayerStart)
            {
                SpawnLocation = PlayerStart->GetActorLocation();
                SpawnRotation = PlayerStart->GetActorRotation();
                break;
            }
        }
    }

    // Spawn player character
    PlayerCharacter = SpawnModularCharacter(SpawnLocation, DefaultPlayerConfig, true);

    if (PlayerCharacter)
    {
        UE_LOG(LogTemp, Warning, TEXT("Player character spawned successfully"));
    }
    else
    {
        UE_LOG(LogTemp, Error, TEXT("Failed to spawn player character"));
    }
}

void AGameGameMode::SpawnAICharacters(int32 Count)
{
    for (int32 i = 0; i < Count; i++)
    {
        FVector AILocation = GetRandomSpawnLocation();
        FCharacterVariantConfig AIConfig = DefaultAIConfig;
        AIConfig.CharacterName = FString::Printf(TEXT("AI_%s_%d"), *DefaultAIConfig.CharacterName, i);

        ACharacter* AICharacter = SpawnModularCharacter(AILocation, AIConfig, false);
        if (AICharacter)
        {
            UE_LOG(LogTemp, Log, TEXT("AI Character %d spawned successfully"), i);
        }
    }

    UE_LOG(LogTemp, Log, TEXT("Spawned %d AI characters"), AICharacters.Num());
}

// ========== MODULAR CHARACTER SYSTEM ==========
bool AGameGameMode::LoadCharacterComponent(ACharacter* Character, const FString& ComponentName, const FString& VariantIndex, TArray<FModularComponentInfo>& ComponentInfos)
{
    if (!Character)
    {
        return false;
    }

    // Build mesh path
    FString MeshPath = BuildAssetPath(DefaultPlayerConfig.CharacterName, ComponentName, ComponentName + TEXT("_") + VariantIndex);

    // Validate asset exists
    if (!ValidateAssetPath(MeshPath))
    {
        UE_LOG(LogTemp, Warning, TEXT("Asset not found: %s"), *MeshPath);
        return false;
    }

    // Load skeletal mesh - FIXED
    USkeletalMesh* ComponentMesh = LoadObject<USkeletalMesh>(nullptr, *MeshPath);

    if (!ComponentMesh)
    {
        UE_LOG(LogTemp, Error, TEXT("Failed to load mesh: %s"), *MeshPath);
        return false;
    }

    // Create skeletal mesh component
    USkeletalMeshComponent* MeshComponent = NewObject<USkeletalMeshComponent>(Character);
    MeshComponent->SetSkeletalMesh(ComponentMesh);
    MeshComponent->SetAnimationMode(EAnimationMode::AnimationSingleNode);
    MeshComponent->AttachToComponent(Character->GetRootComponent(), FAttachmentTransformRules::KeepWorldTransform);

    // Load textures for this component
    LoadTexturesForComponent(MeshComponent, ComponentName, VariantIndex);

    // Store component info
    FModularComponentInfo ComponentInfo;
    ComponentInfo.ComponentName = ComponentName;
    ComponentInfo.MeshPath = MeshPath;
    ComponentInfo.MeshComponent = MeshComponent;
    ComponentInfos.Add(ComponentInfo);

    UE_LOG(LogTemp, Log, TEXT("Loaded component: %s"), *ComponentName);
    return true;
}

void AGameGameMode::LoadTexturesForComponent(USkeletalMeshComponent* MeshComponent, const FString& ComponentName, const FString& VariantIndex)
{
    if (!MeshComponent)
    {
        return;
    }

    // Build texture paths
    FString TextureBasePath = FString::Printf(TEXT("%s/%s/%s/Texture"), *CharacterBasePath, *DefaultPlayerConfig.CharacterName, *ComponentName);

    // Load textures directly - FIXED
    FString DiffusePath = FString::Printf(TEXT("%s/%s_diff_%s_a_uni"), *TextureBasePath, *ComponentName, *VariantIndex);
    UTexture2D* DiffuseTexture = LoadObject<UTexture2D>(nullptr, *DiffusePath);

    FString NormalPath = FString::Printf(TEXT("%s/%s_normal_%s"), *TextureBasePath, *ComponentName, *VariantIndex);
    UTexture2D* NormalTexture = LoadObject<UTexture2D>(nullptr, *NormalPath);

    FString SpecularPath = FString::Printf(TEXT("%s/%s_spec_%s"), *TextureBasePath, *ComponentName, *VariantIndex);
    UTexture2D* SpecularTexture = LoadObject<UTexture2D>(nullptr, *SpecularPath);

    // Create and apply dynamic material
    if (DiffuseTexture)
    {
        UMaterialInstanceDynamic* DynamicMaterial = CreateDynamicMaterial(MeshComponent);
        if (DynamicMaterial)
        {
            DynamicMaterial->SetTextureParameterValue(TEXT("DiffuseTexture"), DiffuseTexture);
            if (NormalTexture)
                DynamicMaterial->SetTextureParameterValue(TEXT("NormalTexture"), NormalTexture);
            if (SpecularTexture)
                DynamicMaterial->SetTextureParameterValue(TEXT("SpecularTexture"), SpecularTexture);

            MeshComponent->SetMaterial(0, DynamicMaterial);
            UE_LOG(LogTemp, Log, TEXT("Applied textures for: %s"), *ComponentName);
        }
    }
}

FString AGameGameMode::BuildAssetPath(const FString& CharacterName, const FString& ComponentName, const FString& VariantIndex, const FString& AssetType)
{
    if (AssetType.IsEmpty())
    {
        return FString::Printf(TEXT("%s/%s/%s/%s"), *CharacterBasePath, *CharacterName, *ComponentName, *VariantIndex);
    }
    else
    {
        return FString::Printf(TEXT("%s/%s/%s/%s/%s"), *CharacterBasePath, *CharacterName, *ComponentName, *AssetType, *VariantIndex);
    }
}

void AGameGameMode::SetupMasterPoseComponent(ACharacter* Character, const TArray<FModularComponentInfo>& ComponentInfos)
{
    if (!Character || ComponentInfos.Num() == 0)
    {
        return;
    }

    USkeletalMeshComponent* MainMesh = Character->GetMesh();
    USkeletalMeshComponent* MasterComponent = nullptr;

    // Use the first component (typically head) as master
    if (ComponentInfos.Num() > 0 && ComponentInfos[0].MeshComponent)
    {
        MasterComponent = ComponentInfos[0].MeshComponent;
        MainMesh->SetSkeletalMesh(MasterComponent->GetSkeletalMeshAsset());
    }

    // Set all other components to follow the master
    for (int32 i = 1; i < ComponentInfos.Num(); i++)
    {
        if (ComponentInfos[i].MeshComponent && MasterComponent)
        {
            ComponentInfos[i].MeshComponent->SetLeaderPoseComponent(MasterComponent);
        }
    }

    UE_LOG(LogTemp, Log, TEXT("Master pose system setup with %d components"), ComponentInfos.Num());
}

// ========== INPUT MANAGEMENT ==========
void AGameGameMode::SetupEnhancedInput(ACharacter* Character)
{
    if (!Character)
    {
        return;
    }

    APlayerController* PC = GetWorld()->GetFirstPlayerController();
    if (!PC)
    {
        return;
    }

    // Load input mapping context - FIXED
    FString InputContextPath = FString::Printf(TEXT("%s/InputMappingContext/IMC_Default"), *InputBasePath);
    UInputMappingContext* InputContext = LoadObject<UInputMappingContext>(nullptr, *InputContextPath);

    if (InputContext)
    {
        if (UEnhancedInputLocalPlayerSubsystem* Subsystem = ULocalPlayer::GetSubsystem<UEnhancedInputLocalPlayerSubsystem>(PC->GetLocalPlayer()))
        {
            Subsystem->ClearAllMappings();
            Subsystem->AddMappingContext(InputContext, 0);
            UE_LOG(LogTemp, Log, TEXT("Enhanced input context added"));
        }
    }
}

void AGameGameMode::GivePlayerControl(ACharacter* Character)
{
    if (!Character)
    {
        return;
    }

    APlayerController* PC = GetWorld()->GetFirstPlayerController();
    if (PC)
    {
        PC->Possess(Character);
        UE_LOG(LogTemp, Log, TEXT("Player control given to character"));
    }
}

// ========== AI MANAGEMENT ==========
void AGameGameMode::UpdateAIBehaviors()
{
    // Clean up invalid AI characters
    AICharacters.RemoveAll([](ACharacter* Character) {
        return !IsValid(Character);
        });

    // Update AI behaviors here
    for (ACharacter* AICharacter : AICharacters)
    {
        if (IsValid(AICharacter))
        {
            // Add AI behavior logic here
            // This could involve task assignment, movement, etc.
        }
    }
}

void AGameGameMode::SpawnAIPed(const FVector& Location, const FString& PedName)
{
    if (!PedFactory || AIPeds.Num() >= MaxAIPeds)
    {
        return;
    }

    // Use existing Ped spawning system for backwards compatibility
    // This could be refactored to use the new modular system
}

// ========== UTILITY FUNCTIONS ==========
FVector AGameGameMode::GetRandomSpawnLocation() const
{
    FVector BaseLocation = PlayerCharacter ? PlayerCharacter->GetActorLocation() : PlayerSpawnLocation;

    float RandomAngle = FMath::RandRange(0.0f, 2.0f * PI);
    float RandomDistance = FMath::RandRange(500.0f, AISpawnRadius);

    FVector RandomOffset = FVector(
        FMath::Cos(RandomAngle) * RandomDistance,
        FMath::Sin(RandomAngle) * RandomDistance,
        100.0f
    );

    return BaseLocation + RandomOffset;
}

void AGameGameMode::CleanupInvalidActors()
{
    // Clean up AI characters
    AICharacters.RemoveAll([](ACharacter* Character) {
        return !IsValid(Character);
        });

    // Clean up AI Peds
    AIPeds.RemoveAll([](APed* Ped) {
        return !IsValid(Ped);
        });
}

// ========== INTERNAL HELPERS ==========
void AGameGameMode::InitializeFactories()
{
    UE_LOG(LogTemp, Log, TEXT("Initializing factories..."));

    // Create animation loader
    AnimationLoader = NewObject<UAnimationGroupsLoader>(this);
    if (AnimationLoader)
    {
        bool bAnimationLoaded = AnimationLoader->LoadAnimationGroupsFromXML();
        if (bAnimationLoaded)
        {
            UE_LOG(LogTemp, Log, TEXT("Animation groups loaded successfully"));
        }
        else
        {
            UE_LOG(LogTemp, Error, TEXT("Failed to load animation groups"));
        }
    }

    // Create factories
    PedFactory = NewObject<UPedFactory>(this);
    TaskFactory = NewObject<UTaskFactory>(this);

    if (PedFactory && TaskFactory)
    {
        UE_LOG(LogTemp, Log, TEXT("Factories initialized successfully"));
    }
    else
    {
        UE_LOG(LogTemp, Error, TEXT("Failed to initialize factories"));
    }
}

void AGameGameMode::StartGameLoop()
{
    UE_LOG(LogTemp, Log, TEXT("Starting game loop"));

    // Spawn player character
    SpawnPlayerCharacter();

    // Spawn AI characters
    SpawnAICharacters(2);

    // Set game to in progress
    SetGameState(EGameState::InProgress);

    UE_LOG(LogTemp, Log, TEXT("Game loop started successfully"));
}

void AGameGameMode::UpdateGameTime(float DeltaTime)
{
    GameTime += DeltaTime;
}

UMaterialInstanceDynamic* AGameGameMode::CreateDynamicMaterial(USkeletalMeshComponent* MeshComponent)
{
    if (!MeshComponent)
    {
        return nullptr;
    }

    // FIXED - Use correct variable name and path
    FString MaterialPath = FString::Printf(TEXT("%s/M_Ped"), *MaterialBasePath);
    UMaterialInterface* BaseMaterial = LoadObject<UMaterialInterface>(nullptr, *MaterialPath);

    if (BaseMaterial)
    {
        return UMaterialInstanceDynamic::Create(BaseMaterial, MeshComponent);
    }

    return nullptr;
}

bool AGameGameMode::ValidateAssetPath(const FString& AssetPath) const
{
    // Simple validation - could be enhanced with actual asset registry checks
    return !AssetPath.IsEmpty() && AssetPath.StartsWith(TEXT("/Game/"));
}

void AGameGameMode::LogCharacterSpawnInfo(ACharacter* Character, const FCharacterVariantConfig& Config) const
{
    if (Character)
    {
        UE_LOG(LogTemp, Log, TEXT("Character '%s' spawned with variant '%s' at location %s"),
            *Config.CharacterName, *Config.VariantIndex, *Character->GetActorLocation().ToString());
    }
}
