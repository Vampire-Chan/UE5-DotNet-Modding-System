#pragma once

#include "CoreMinimal.h"
#include "UObject/NoExportTypes.h"
#include "Engine/World.h"
#include "Components/SkeletalMeshComponent.h"
#include "../Core/Enums/GameWorldEnums.h"
#include "../Peds/Ped.h"
#include "../Peds/Character/PedCharacterComponent.h"
#include "PedFactory.generated.h"

// Struct to hold ped data from XML
USTRUCT(BlueprintType)
struct FPedXMLData
{
    GENERATED_BODY()

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "XML Data")
    FString Name;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "XML Data")
    FString Type;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "XML Data")
    FString BaseSkeleton;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "XML Data")
    FString PropType;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "XML Data")
    FString AnimationGroup;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "XML Data")
    FString VoicePack;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "XML Data")
    FString DefaultVariation;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "XML Data")
    FString RelationshipGroup;

    FPedXMLData()
    {
        Name = TEXT("");
        Type = TEXT("");
        BaseSkeleton = TEXT("");
        PropType = TEXT("");
        AnimationGroup = TEXT("");
        VoicePack = TEXT("");
        DefaultVariation = TEXT("Default");
        RelationshipGroup = TEXT("CIVILIAN");
    }
};

// Struct to define a complete ped configuration
USTRUCT(BlueprintType)
struct FPedSpawnConfiguration
{
    GENERATED_BODY()

    // Basic spawn parameters
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Spawn")
    FString CharacterName;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Spawn")
    FString VariationName;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Spawn")
    FVector SpawnLocation;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Spawn")
    FRotator SpawnRotation;

    // Behavior
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Behavior")
    bool bAIEnabled;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Behavior")
    bool bPlayerControlled;

    FPedSpawnConfiguration()
    {
        CharacterName = TEXT("PlayerNiko");
        VariationName = TEXT("Default");
        SpawnLocation = FVector::ZeroVector;
        SpawnRotation = FRotator::ZeroRotator;
        bAIEnabled = true;
        bPlayerControlled = false;
    }
};

/**
 * PedFactory - Responsible for creating and spawning pedestrians in the game world
 * Handles proper mesh application, prop attachment, and component configuration
 */
UCLASS(BlueprintType, Blueprintable)
class GAME_API UPedFactory : public UObject
{
    GENERATED_BODY()

public:
    UPedFactory();

    // Main factory functions
    UFUNCTION(BlueprintCallable, Category = "Ped Factory")
    APed* SpawnPed(UWorld* World, const FPedSpawnConfiguration& Configuration);

    // Player control functions
    UFUNCTION(BlueprintCallable, Category = "Ped Factory")
    bool PossessPed(APed* Ped, APlayerController* PlayerController);

    UFUNCTION(BlueprintCallable, Category = "Ped Factory")
    bool UnpossessPed(APlayerController* PlayerController);

    UFUNCTION(BlueprintCallable, Category = "Ped Factory")
    void SetPedAIEnabled(APed* Ped, bool bEnabled);

private:
    // Factory configuration
    UPROPERTY(EditAnywhere, Category = "Factory Settings")
    TSubclassOf<APed> DefaultPedClass;
};
