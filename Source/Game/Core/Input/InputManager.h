#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "EnhancedInputSubsystems.h"
#include "InputManager.generated.h"

class UInputMappingContext;
class APed;

/**
 * Global Input Manager for controlling input states
 * Allows enabling/disabling input, input filtering by key, etc.
 */
UCLASS(BlueprintType, Blueprintable, ClassGroup=(Custom), meta=(BlueprintSpawnableComponent))
class GAME_API UInputManager : public UActorComponent
{
    GENERATED_BODY()

public:
    UInputManager();

protected:
    virtual void BeginPlay() override;

public:
    // ========== INPUT CONTROL ==========
    
    UFUNCTION(BlueprintCallable, Category = "Input Control")
    void EnableInput();
    
    UFUNCTION(BlueprintCallable, Category = "Input Control")
    void DisableInput();
    
    UFUNCTION(BlueprintCallable, Category = "Input Control")
    void ToggleInput();
    
    UFUNCTION(BlueprintCallable, Category = "Input Control")
    bool IsInputEnabled() const { return bInputEnabled; }

    // ========== KEY-SPECIFIC INPUT CONTROL ==========
    
    UFUNCTION(BlueprintCallable, Category = "Input Control")
    void DisableInputForKey(const FString& KeyName);
    
    UFUNCTION(BlueprintCallable, Category = "Input Control")
    void EnableInputForKey(const FString& KeyName);
    
    UFUNCTION(BlueprintCallable, Category = "Input Control")
    void RestoreAllKeyInputs();
    
    UFUNCTION(BlueprintCallable, Category = "Input Control")
    bool IsKeyInputDisabled(const FString& KeyName) const;

    // ========== CHARACTER INPUT MANAGEMENT ==========
    
    UFUNCTION(BlueprintCallable, Category = "Character Input")
    void SetInputForCharacter(APed* Character, bool bEnable);
    
    UFUNCTION(BlueprintCallable, Category = "Character Input")
    void TransferInputToCharacter(APed* FromCharacter, APed* ToCharacter);

protected:
    // ========== PROPERTIES ==========
    
    UPROPERTY(BlueprintReadOnly, Category = "Input State")
    bool bInputEnabled;
    
    UPROPERTY(BlueprintReadOnly, Category = "Input State")
    TSet<FString> DisabledKeys;
    
    UPROPERTY(BlueprintReadOnly, Category = "Input State")
    APed* CurrentInputCharacter;

private:
    // ========== INTERNAL FUNCTIONS ==========
    
    void UpdateInputMappingContext();
    APlayerController* GetPlayerController() const;
};
