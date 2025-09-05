#include "EntityIDManager.h"
#include "Engine/World.h"

// Static instance initialization
UEntityIDManager* UEntityIDManager::Instance = nullptr;

UEntityIDManager::UEntityIDManager()
{
    // Initialize default values
    CurrentIDCounter = 1; // Start from 1, reserve 0 for "Invalid"
    MinEntityID = 1;
    MaxEntityID = 999999999; // Allow up to ~1 billion entities
    ReservedEntityIDs.Empty();
}

UEntityIDManager* UEntityIDManager::GetInstance()
{
    if (!Instance)
    {
        Instance = NewObject<UEntityIDManager>();
        Instance->AddToRoot(); // Prevent garbage collection
    }
    return Instance;
}

int32 UEntityIDManager::GenerateNewEntityID()
{
    // Find next available ID starting from current counter
    int32 NewID = FindNextAvailableID(CurrentIDCounter);
    
    if (NewID > MaxEntityID)
    {
        // Wrap around and search from the beginning
        NewID = FindNextAvailableID(MinEntityID);
        
        if (NewID > MaxEntityID)
        {
            UE_LOG(LogTemp, Error, TEXT("EntityIDManager: No available Entity IDs! All %d IDs are taken."), MaxEntityID - MinEntityID + 1);
            return 0; // Return invalid ID
        }
    }

    // Reserve the ID and update counter
    ReservedEntityIDs.Add(NewID);
    CurrentIDCounter = NewID + 1;

    UE_LOG(LogTemp, VeryVerbose, TEXT("EntityIDManager: Generated new Entity ID: %d"), NewID);
    return NewID;
}

void UEntityIDManager::ReserveEntityID(int32 EntityID)
{
    if (!IsIDInValidRange(EntityID))
    {
        UE_LOG(LogTemp, Warning, TEXT("EntityIDManager: Attempted to reserve Entity ID %d which is outside valid range [%d, %d]"), 
               EntityID, MinEntityID, MaxEntityID);
        return;
    }

    if (ReservedEntityIDs.Contains(EntityID))
    {
        UE_LOG(LogTemp, Warning, TEXT("EntityIDManager: Entity ID %d is already reserved"), EntityID);
        return;
    }

    ReservedEntityIDs.Add(EntityID);
    UE_LOG(LogTemp, VeryVerbose, TEXT("EntityIDManager: Reserved Entity ID: %d"), EntityID);
}

bool UEntityIDManager::IsEntityIDTaken(int32 EntityID) const
{
    return ReservedEntityIDs.Contains(EntityID);
}

void UEntityIDManager::ReleaseEntityID(int32 EntityID)
{
    if (ReservedEntityIDs.Remove(EntityID))
    {
        UE_LOG(LogTemp, VeryVerbose, TEXT("EntityIDManager: Released Entity ID: %d"), EntityID);
    }
    else
    {
        UE_LOG(LogTemp, Warning, TEXT("EntityIDManager: Attempted to release Entity ID %d that was not reserved"), EntityID);
    }
}

void UEntityIDManager::SetIDRange(int32 MinID, int32 MaxID)
{
    if (MinID >= MaxID)
    {
        UE_LOG(LogTemp, Error, TEXT("EntityIDManager: Invalid ID range: Min (%d) must be less than Max (%d)"), MinID, MaxID);
        return;
    }

    MinEntityID = MinID;
    MaxEntityID = MaxID;
    
    // Ensure current counter is within valid range
    if (CurrentIDCounter < MinEntityID)
    {
        CurrentIDCounter = MinEntityID;
    }
    else if (CurrentIDCounter > MaxEntityID)
    {
        CurrentIDCounter = MinEntityID;
    }

    UE_LOG(LogTemp, Log, TEXT("EntityIDManager: Set ID range to [%d, %d]"), MinEntityID, MaxEntityID);
}

int32 UEntityIDManager::GetNextAvailableID() const
{
    return FindNextAvailableID(CurrentIDCounter);
}

int32 UEntityIDManager::GetTotalActiveEntities() const
{
    return ReservedEntityIDs.Num();
}

void UEntityIDManager::ResetIDCounter()
{
    CurrentIDCounter = MinEntityID;
    UE_LOG(LogTemp, Log, TEXT("EntityIDManager: Reset ID counter to %d"), CurrentIDCounter);
}

void UEntityIDManager::ClearAllReservedIDs()
{
    int32 PreviousCount = ReservedEntityIDs.Num();
    ReservedEntityIDs.Empty();
    CurrentIDCounter = MinEntityID;
    UE_LOG(LogTemp, Log, TEXT("EntityIDManager: Cleared %d reserved Entity IDs"), PreviousCount);
}

bool UEntityIDManager::IsIDInValidRange(int32 EntityID) const
{
    return EntityID >= MinEntityID && EntityID <= MaxEntityID;
}

int32 UEntityIDManager::FindNextAvailableID(int32 StartID) const
{
    for (int32 ID = StartID; ID <= MaxEntityID; ++ID)
    {
        if (!ReservedEntityIDs.Contains(ID))
        {
            return ID;
        }
    }
    
    // No available ID found in range
    return MaxEntityID + 1; // Return out-of-range value to indicate failure
}
