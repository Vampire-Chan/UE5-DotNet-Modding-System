#pragma once

#include "CoreMinimal.h"
#include "UObject/NoExportTypes.h"
#include "EntityIDManager.generated.h"

/**
 * Singleton class that manages unique Entity IDs across the entire game
 * Provides incremental ID generation for all entities
 */
UCLASS(BlueprintType)
class GAME_API UEntityIDManager : public UObject
{
    GENERATED_BODY()

public:
    UEntityIDManager();

    // Singleton access
    UFUNCTION(BlueprintCallable, Category = "Entity ID Manager")
    static UEntityIDManager* GetInstance();

    // ID Generation
    UFUNCTION(BlueprintCallable, Category = "Entity ID Manager")
    int32 GenerateNewEntityID();

    UFUNCTION(BlueprintCallable, Category = "Entity ID Manager")
    void ReserveEntityID(int32 EntityID);

    UFUNCTION(BlueprintCallable, Category = "Entity ID Manager")
    bool IsEntityIDTaken(int32 EntityID) const;

    UFUNCTION(BlueprintCallable, Category = "Entity ID Manager")
    void ReleaseEntityID(int32 EntityID);

    // ID Range Management
    UFUNCTION(BlueprintCallable, Category = "Entity ID Manager")
    void SetIDRange(int32 MinID, int32 MaxID);

    UFUNCTION(BlueprintCallable, Category = "Entity ID Manager")
    int32 GetNextAvailableID() const;

    UFUNCTION(BlueprintCallable, Category = "Entity ID Manager")
    int32 GetTotalActiveEntities() const;

    // Reset and Cleanup
    UFUNCTION(BlueprintCallable, Category = "Entity ID Manager")
    void ResetIDCounter();

    UFUNCTION(BlueprintCallable, Category = "Entity ID Manager")
    void ClearAllReservedIDs();

protected:
    // Current ID counter for incremental generation
    UPROPERTY(BlueprintReadOnly, Category = "Entity ID Manager")
    int32 CurrentIDCounter;

    // ID range limits
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Entity ID Manager")
    int32 MinEntityID;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Entity ID Manager")
    int32 MaxEntityID;

    // Set of currently reserved/taken IDs
    UPROPERTY()
    TSet<int32> ReservedEntityIDs;

    // Singleton instance
    static UEntityIDManager* Instance;

private:
    // Internal helpers
    bool IsIDInValidRange(int32 EntityID) const;
    int32 FindNextAvailableID(int32 StartID) const;
};
