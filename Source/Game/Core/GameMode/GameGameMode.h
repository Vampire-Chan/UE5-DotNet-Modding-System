#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameModeBase.h"
#include "Engine/TimerHandle.h"
#include "Components/SkeletalMeshComponent.h"
#include "Materials/MaterialInstanceDynamic.h"
#include "GameGameMode.generated.h"

// Forward declarations
class APed;
class UPedFactory;
class UTaskFactory;
class UAnimationGroupsLoader;
class UEnhancedInputComponent;
class UInputMappingContext;
class UInputAction;

// Game state enum
UENUM(BlueprintType)
enum class EGameState : uint8
{
    PreGame     UMETA(DisplayName = "Pre Game"),
    Starting    UMETA(DisplayName = "Starting"),
    InProgress  UMETA(DisplayName = "In Progress"),
    Paused      UMETA(DisplayName = "Paused"),
    Ending      UMETA(DisplayName = "Ending")
};

// Modular character component info
USTRUCT(BlueprintType)
struct GAME_API FModularComponentInfo
{
    GENERATED_BODY()

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Component")
    FString ComponentName;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Component")
    FString MeshPath;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Component")
    TArray<FString> TexturePaths;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Component")
    USkeletalMeshComponent* MeshComponent;

    FModularComponentInfo()
    {
        ComponentName = TEXT("");
        MeshPath = TEXT("");
        MeshComponent = nullptr;
    }
};

// Character variant configuration
USTRUCT(BlueprintType)
struct GAME_API FCharacterVariantConfig
{
    GENERATED_BODY()

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Variant")
    FString CharacterName;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Variant")
    FString VariantIndex; // 000, 001, 002, etc.

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Variant")
    TArray<FString> BodyParts;

    FCharacterVariantConfig()
    {
        CharacterName = TEXT("PlayerNiko");
        VariantIndex = TEXT("000");
        BodyParts = { TEXT("head"), TEXT("uppr"), TEXT("lowr"), TEXT("hand"), TEXT("feet") };
    }
};

// Delegate declarations
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnGameStateChanged, EGameState, NewGameState);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FOnCharacterSpawned, ACharacter*, SpawnedCharacter, bool, bIsPlayerControlled);

UCLASS()
class GAME_API AGameGameMode : public AGameModeBase
{
    GENERATED_BODY()

public:
    AGameGameMode();

protected:
    virtual void BeginPlay() override;
    virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;
    virtual void Tick(float DeltaTime) override;

public:
    // ========== DELEGATES ==========
    UPROPERTY(BlueprintAssignable, Category = "Game Events")
    FOnGameStateChanged OnGameStateChanged;

    UPROPERTY(BlueprintAssignable, Category = "Game Events")
    FOnCharacterSpawned OnCharacterSpawned;

    // ========== GAME STATE MANAGEMENT ==========
    UFUNCTION(BlueprintCallable, Category = "Game State")
    void SetGameState(EGameState NewState);

    UFUNCTION(BlueprintPure, Category = "Game State")
    EGameState GetCurrentGameState() const { return CurrentGameState; }

    UFUNCTION(BlueprintPure, Category = "Game State")
    bool IsGameInProgress() const { return CurrentGameState == EGameState::InProgress; }

    UFUNCTION(BlueprintPure, Category = "Game State")
    float GetGameTime() const { return GameTime; }

    // ========== ENHANCED CHARACTER SPAWNING ==========
    UFUNCTION(BlueprintCallable, Category = "Character Spawning")
    ACharacter* SpawnModularCharacter(const FVector& SpawnLocation, const FCharacterVariantConfig& Config, bool bPlayerControlled = false);

    UFUNCTION(BlueprintCallable, Category = "Character Spawning")
    void SpawnPlayerCharacter();

    UFUNCTION(BlueprintCallable, Category = "Character Spawning")
    void SpawnAICharacters(int32 Count = 2);

    // ========== MODULAR CHARACTER SYSTEM ==========
    UFUNCTION(BlueprintCallable, Category = "Modular Character")
    bool LoadCharacterComponent(ACharacter* Character, const FString& ComponentName, const FString& VariantIndex, TArray<FModularComponentInfo>& ComponentInfos);

    UFUNCTION(BlueprintCallable, Category = "Modular Character")
    void LoadTexturesForComponent(USkeletalMeshComponent* MeshComponent, const FString& ComponentName, const FString& VariantIndex);

    UFUNCTION(BlueprintCallable, Category = "Modular Character")
    FString BuildAssetPath(const FString& CharacterName, const FString& ComponentName, const FString& VariantIndex, const FString& AssetType = TEXT(""));

    UFUNCTION(BlueprintCallable, Category = "Modular Character")
    void SetupMasterPoseComponent(ACharacter* Character, const TArray<FModularComponentInfo>& ComponentInfos);

    // ========== INPUT MANAGEMENT ==========
    UFUNCTION(BlueprintCallable, Category = "Input")
    void SetupEnhancedInput(ACharacter* Character);

    UFUNCTION(BlueprintCallable, Category = "Input")
    void GivePlayerControl(ACharacter* Character);

    // ========== AI MANAGEMENT ==========
    UFUNCTION(BlueprintCallable, Category = "AI")
    void UpdateAIBehaviors();

    UFUNCTION(BlueprintCallable, Category = "AI")
    void SpawnAIPed(const FVector& Location, const FString& PedName);

    // ========== UTILITY FUNCTIONS ==========
    UFUNCTION(BlueprintCallable, Category = "Utility")
    FVector GetRandomSpawnLocation() const;

    UFUNCTION(BlueprintCallable, Category = "Utility")
    void CleanupInvalidActors();

protected:
    // ========== INITIALIZATION ==========
    void InitializeFactories();
    void StartGameLoop();
    void UpdateGameTime(float DeltaTime);

    // ========== INTERNAL HELPERS ==========
    UMaterialInstanceDynamic* CreateDynamicMaterial(USkeletalMeshComponent* MeshComponent);
    bool ValidateAssetPath(const FString& AssetPath) const;
    void LogCharacterSpawnInfo(ACharacter* Character, const FCharacterVariantConfig& Config) const;

protected:
    // ========== GAME STATE ==========
    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Game State")
    EGameState CurrentGameState;

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Game State")
    float GameTime;

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Game State")
    bool bGameHasStarted;

    // ========== PLAYER SETTINGS ==========
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Player Settings")
    FVector PlayerSpawnLocation;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Player Settings")
    FRotator PlayerSpawnRotation;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Player Settings")
    bool bUsePlayerStart;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Player Settings")
    bool bSpawnAtCustomLocation;

    // ========== CHARACTER REFERENCES ==========
    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Characters")
    ACharacter* PlayerCharacter;

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Characters")
    TArray<ACharacter*> AICharacters;

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Characters")
    TArray<APed*> AIPeds;

    // ========== DEFAULT CONFIGURATION ==========
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Character Config")
    FCharacterVariantConfig DefaultPlayerConfig;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Character Config")
    FCharacterVariantConfig DefaultAIConfig;

    // ========== AI SETTINGS ==========
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "AI Settings")
    int32 MaxAIPeds;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "AI Settings")
    float AIUpdateInterval;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "AI Settings")
    float AISpawnRadius;

    // ========== ASSET PATHS ==========
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Asset Paths")
    FString CharacterBasePath;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Asset Paths")
    FString MaterialBasePath;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Asset Paths")
    FString InputBasePath;

    // ========== FACTORIES ==========
    UPROPERTY()
    UPedFactory* PedFactory;

    UPROPERTY()
    UTaskFactory* TaskFactory;

    UPROPERTY()
    UAnimationGroupsLoader* AnimationLoader;

    // ========== TIMERS ==========
    FTimerHandle AIUpdateTimer;
    FTimerHandle GameStateTimer;
    FTimerHandle CleanupTimer;
};
