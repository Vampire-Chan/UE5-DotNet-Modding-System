#include "PedFactory.h"
#include "Engine/World.h"
#include "Kismet/GameplayStatics.h"

UPedFactory::UPedFactory()
{
    DefaultPedClass = APed::StaticClass();
}

APed* UPedFactory::SpawnPed(UWorld* World, const FPedSpawnConfiguration& Configuration)
{
    if (!World || !DefaultPedClass)
    {
        UE_LOG(LogTemp, Error, TEXT("PedFactory: Invalid World or DefaultPedClass"));
        return nullptr;
    }

    // Generate a unique name for the spawned actor with timestamp
    FString BaseName = Configuration.CharacterName;
    if (BaseName.IsEmpty())
    {
        BaseName = TEXT("Ped");
    }
    
    FString UniqueName = BaseName;
    int32 Suffix = 0;
    
    // Add more thorough uniqueness checking
    while (StaticFindObjectFast(nullptr, World, *UniqueName) || 
           StaticFindObjectFast(nullptr, World->PersistentLevel, *UniqueName))
    {
        UniqueName = FString::Printf(TEXT("%s_%d_%d"), *BaseName, Suffix++, FDateTime::Now().GetTicks() % 10000);
    }

    FActorSpawnParameters SpawnParams;
    SpawnParams.Name = FName(*UniqueName); // Set the unique name for the actor
    SpawnParams.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AdjustIfPossibleButAlwaysSpawn;
    SpawnParams.bDeferConstruction = true; // Defer construction to set properties first
    SpawnParams.NameMode = FActorSpawnParameters::ESpawnActorNameMode::Requested; // Use the requested name

    APed* SpawnedPed = World->SpawnActor<APed>(DefaultPedClass, Configuration.SpawnLocation, Configuration.SpawnRotation, SpawnParams);

    if (!SpawnedPed)
    {
        UE_LOG(LogTemp, Error, TEXT("PedFactory: Failed to spawn ped actor"));
        return nullptr;
    }

    // Initialize the ped with the spawn configuration. This will trigger all internal loading.
    // The ped will store its unique name internally.
    SpawnedPed->InitializePed(Configuration, UniqueName);

    // Finalize the spawning process. This calls BeginPlay on the actor and its components.
    UGameplayStatics::FinishSpawningActor(SpawnedPed, FTransform(Configuration.SpawnRotation, Configuration.SpawnLocation));

    UE_LOG(LogTemp, Log, TEXT("PedFactory: Successfully spawned and initiated ped: %s (Unique Name: %s)"), *Configuration.CharacterName, *UniqueName);

    return SpawnedPed;
}

bool UPedFactory::PossessPed(APed* Ped, APlayerController* PlayerController)
{
    if (!Ped || !PlayerController)
    {
        UE_LOG(LogTemp, Error, TEXT("PedFactory: Invalid Ped or PlayerController for possession"));
        return false;
    }

    // Disable AI before possession
    SetPedAIEnabled(Ped, false);

    // Unpossess current pawn if any
    if (PlayerController->GetPawn())
    {
        PlayerController->UnPossess();
    }

    // Possess the ped
    PlayerController->Possess(Ped);
    Ped->SetPlayerControlled(true);

    UE_LOG(LogTemp, Log, TEXT("PedFactory: Player possessed ped: %s"), *Ped->GetCharacterName());
    return true;
}

bool UPedFactory::UnpossessPed(APlayerController* PlayerController)
{
    if (!PlayerController)
    {
        UE_LOG(LogTemp, Error, TEXT("PedFactory: Invalid PlayerController for unpossession"));
        return false;
    }

    APed* CurrentPed = Cast<APed>(PlayerController->GetPawn());
    if (CurrentPed)
    {
        CurrentPed->SetPlayerControlled(false);
        // Optionally re-enable AI
        SetPedAIEnabled(CurrentPed, true);
        
        UE_LOG(LogTemp, Log, TEXT("PedFactory: Player unpossessed ped: %s"), *CurrentPed->GetCharacterName());
    }

    PlayerController->UnPossess();
    return true;
}

void UPedFactory::SetPedAIEnabled(APed* Ped, bool bEnabled)
{
    if (!Ped)
    {
        return;
    }

    // This will be handled by the Ped's internal logic, possibly by its AI component.
    // For now, we just log the intent.
    UE_LOG(LogTemp, Log, TEXT("PedFactory: Set AI %s for ped: %s"), 
           bEnabled ? TEXT("Enabled") : TEXT("Disabled"), *Ped->GetCharacterName());
}
