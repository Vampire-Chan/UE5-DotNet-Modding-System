#include "InputManager.h"
#include "../../Peds/Ped.h"
#include "../../Peds/Locomotion/PedInputComponent.h"
#include "Engine/World.h"
#include "GameFramework/PlayerController.h"
#include "EnhancedInputSubsystems.h"

UInputManager::UInputManager()
{
    PrimaryComponentTick.bCanEverTick = false;
    bInputEnabled = true;
    CurrentInputCharacter = nullptr;
}

void UInputManager::BeginPlay()
{
    Super::BeginPlay();
    UE_LOG(LogTemp, Log, TEXT("InputManager: Initialized"));
}

// ========== INPUT CONTROL ==========

void UInputManager::EnableInput()
{
    bInputEnabled = true;
    UpdateInputMappingContext();
    UE_LOG(LogTemp, Log, TEXT("InputManager: Input enabled"));
}

void UInputManager::DisableInput()
{
    bInputEnabled = false;
    UpdateInputMappingContext();
    UE_LOG(LogTemp, Log, TEXT("InputManager: Input disabled"));
}

void UInputManager::ToggleInput()
{
    if (bInputEnabled)
    {
        DisableInput();
    }
    else
    {
        EnableInput();
    }
}

// ========== KEY-SPECIFIC INPUT CONTROL ==========

void UInputManager::DisableInputForKey(const FString& KeyName)
{
    DisabledKeys.Add(KeyName);
    UE_LOG(LogTemp, Log, TEXT("InputManager: Disabled input for key: %s"), *KeyName);
}

void UInputManager::EnableInputForKey(const FString& KeyName)
{
    DisabledKeys.Remove(KeyName);
    UE_LOG(LogTemp, Log, TEXT("InputManager: Enabled input for key: %s"), *KeyName);
}

void UInputManager::RestoreAllKeyInputs()
{
    DisabledKeys.Empty();
    UE_LOG(LogTemp, Log, TEXT("InputManager: Restored all key inputs"));
}

bool UInputManager::IsKeyInputDisabled(const FString& KeyName) const
{
    return DisabledKeys.Contains(KeyName);
}

// ========== CHARACTER INPUT MANAGEMENT ==========

void UInputManager::SetInputForCharacter(APed* Character, bool bEnable)
{
    if (!IsValid(Character))
    {
        UE_LOG(LogTemp, Warning, TEXT("InputManager: Invalid character provided to SetInputForCharacter"));
        return;
    }

    APlayerController* PC = GetPlayerController();
    if (!PC)
    {
        UE_LOG(LogTemp, Warning, TEXT("InputManager: No player controller found"));
        return;
    }

    if (bEnable)
    {
        // Possess the character
        PC->Possess(Character);
        CurrentInputCharacter = Character;
        
        // Set up input component if available
        if (UPedInputComponent* InputComp = Character->GetPedInputComponent())
        {
            if (UEnhancedInputComponent* EnhancedInputComp = Cast<UEnhancedInputComponent>(Character->InputComponent))
            {
                InputComp->SetupInputComponent(EnhancedInputComp);
            }
        }
        
        UE_LOG(LogTemp, Log, TEXT("InputManager: Input enabled for character: %s"), *Character->GetName());
    }
    else
    {
        // Unpossess the character
        PC->UnPossess();
        if (CurrentInputCharacter == Character)
        {
            CurrentInputCharacter = nullptr;
        }
        
        UE_LOG(LogTemp, Log, TEXT("InputManager: Input disabled for character: %s"), *Character->GetName());
    }
}

void UInputManager::TransferInputToCharacter(APed* FromCharacter, APed* ToCharacter)
{
    if (!IsValid(ToCharacter))
    {
        UE_LOG(LogTemp, Warning, TEXT("InputManager: Invalid target character for input transfer"));
        return;
    }

    // Disable input for current character
    if (IsValid(FromCharacter))
    {
        SetInputForCharacter(FromCharacter, false);
    }

    // Enable input for new character
    SetInputForCharacter(ToCharacter, true);
    
    UE_LOG(LogTemp, Log, TEXT("InputManager: Transferred input from %s to %s"), 
           FromCharacter ? *FromCharacter->GetName() : TEXT("None"), 
           *ToCharacter->GetName());
}

// ========== INTERNAL FUNCTIONS ==========

void UInputManager::UpdateInputMappingContext()
{
    APlayerController* PC = GetPlayerController();
    if (!PC)
    {
        return;
    }

    if (UEnhancedInputLocalPlayerSubsystem* Subsystem = ULocalPlayer::GetSubsystem<UEnhancedInputLocalPlayerSubsystem>(PC->GetLocalPlayer()))
    {
        if (bInputEnabled)
        {
            // Input is enabled - ensure mapping context is active
            // This is handled by the individual PedInputComponents
            UE_LOG(LogTemp, Verbose, TEXT("InputManager: Input mapping context updated - enabled"));
        }
        else
        {
            // Input is disabled - could remove all mapping contexts here if needed
            UE_LOG(LogTemp, Verbose, TEXT("InputManager: Input mapping context updated - disabled"));
        }
    }
}

APlayerController* UInputManager::GetPlayerController() const
{
    if (UWorld* World = GetWorld())
    {
        return World->GetFirstPlayerController();
    }
    return nullptr;
}
