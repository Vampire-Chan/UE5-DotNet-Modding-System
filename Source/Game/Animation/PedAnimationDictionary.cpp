#include "PedAnimationDictionary.h"
#include "Engine/Engine.h"

UPedAnimationDictionary::UPedAnimationDictionary()
{
    // Constructor
}

void UPedAnimationDictionary::InitializeDefaultDictionaries()
{
    UE_LOG(LogTemp, Log, TEXT("PedAnimationDictionary: Initializing default dictionaries..."));

    // Clear existing data
    AllDictionaries.Empty();
    AnimationContexts.Empty();

    // Create all default dictionaries
    CreateDefaultMovementDictionaries();
    CreateDefaultCombatDictionaries();
    CreateDefaultVehicleDictionaries();
    CreateDefaultJumpDictionaries();

    // Create contexts
    FAnimationContext MovementContext;
    MovementContext.ContextName = TEXT("Movement");
    MovementContext.CurrentDictionaryName = TEXT("player_movement");
    MovementContext.AvailableDictionaries = {TEXT("player_movement"), TEXT("npc_movement"), TEXT("heavy_movement")};
    AnimationContexts.Add(MovementContext);

    FAnimationContext CrouchContext;
    CrouchContext.ContextName = TEXT("Crouch");
    CrouchContext.CurrentDictionaryName = TEXT("crouch_standard");
    CrouchContext.AvailableDictionaries = {TEXT("crouch_standard"), TEXT("crouch_tactical"), TEXT("crouch_stealth")};
    AnimationContexts.Add(CrouchContext);

    FAnimationContext JumpContext;
    JumpContext.ContextName = TEXT("Jump");
    JumpContext.CurrentDictionaryName = TEXT("jump_standard");
    JumpContext.AvailableDictionaries = {TEXT("jump_standard"), TEXT("jump_athletic"), TEXT("jump_heavy")};
    AnimationContexts.Add(JumpContext);

    FAnimationContext CombatContext;
    CombatContext.ContextName = TEXT("Combat");
    CombatContext.CurrentDictionaryName = TEXT("combat_standard");
    CombatContext.AvailableDictionaries = {TEXT("combat_standard"), TEXT("combat_strafe"), TEXT("combat_tactical")};
    AnimationContexts.Add(CombatContext);

    FAnimationContext VehicleContext;
    VehicleContext.ContextName = TEXT("Vehicle");
    VehicleContext.CurrentDictionaryName = TEXT("vehicle_standard");
    VehicleContext.AvailableDictionaries = {TEXT("vehicle_standard"), TEXT("vehicle_sports"), TEXT("vehicle_truck")};
    AnimationContexts.Add(VehicleContext);

    UE_LOG(LogTemp, Log, TEXT("PedAnimationDictionary: Default dictionaries initialized successfully"));
}

void UPedAnimationDictionary::CreateDefaultMovementDictionaries()
{
    // Player Movement Dictionary (move_player folder)
    {
        FAnimationDictionary PlayerMovement;
        PlayerMovement.DictionaryName = TEXT("player_movement");
        PlayerMovement.BaseFolderPath = TEXT("Data/Animations/move_player");
        PlayerMovement.Description = TEXT("Standard player movement animations");

        // Basic movements (looped)
        PlayerMovement.Animations.Add(CreateEntry(TEXT("idle"), TEXT("idle.onim"), true));
        PlayerMovement.Animations.Add(CreateEntry(TEXT("walk"), TEXT("walk.onim"), true));
        PlayerMovement.Animations.Add(CreateEntry(TEXT("walk_backward"), TEXT("walk_b.onim"), true));
        PlayerMovement.Animations.Add(CreateEntry(TEXT("walk_casual"), TEXT("walk_c.onim"), true));
        PlayerMovement.Animations.Add(CreateEntry(TEXT("walk_up"), TEXT("walk_up.onim"), true));
        PlayerMovement.Animations.Add(CreateEntry(TEXT("walk_down"), TEXT("walk_down.onim"), true));
        PlayerMovement.Animations.Add(CreateEntry(TEXT("walk_strafe_left"), TEXT("walk_strafe_l.onim"), true));
        PlayerMovement.Animations.Add(CreateEntry(TEXT("walk_strafe_right"), TEXT("walk_strafe_r.onim"), true));
        PlayerMovement.Animations.Add(CreateEntry(TEXT("walk_strafe_backward"), TEXT("walk_strafe_b.onim"), true));
        PlayerMovement.Animations.Add(CreateEntry(TEXT("run"), TEXT("run.onim"), true));
        PlayerMovement.Animations.Add(CreateEntry(TEXT("run_up"), TEXT("run_up.onim"), true));
        PlayerMovement.Animations.Add(CreateEntry(TEXT("run_down"), TEXT("run_down.onim"), true));
        PlayerMovement.Animations.Add(CreateEntry(TEXT("run_strafe_left"), TEXT("run_strafe_l.onim"), true));
        PlayerMovement.Animations.Add(CreateEntry(TEXT("run_strafe_right"), TEXT("run_strafe_r.onim"), true));
        PlayerMovement.Animations.Add(CreateEntry(TEXT("run_strafe_backward"), TEXT("run_strafe_b.onim"), true));
        PlayerMovement.Animations.Add(CreateEntry(TEXT("sprint"), TEXT("sprint.onim"), true));

        // Start animations (non-looped)
        PlayerMovement.Animations.Add(CreateEntry(TEXT("walk_start"), TEXT("wstart.onim"), false));
        PlayerMovement.Animations.Add(CreateEntry(TEXT("walk_start_turn_left_90"), TEXT("wstart_turn_l90.onim"), false));
        PlayerMovement.Animations.Add(CreateEntry(TEXT("walk_start_turn_right_90"), TEXT("wstart_turn_r90.onim"), false));
        PlayerMovement.Animations.Add(CreateEntry(TEXT("walk_start_turn_left_180"), TEXT("wstart_turn_l180.onim"), false));
        PlayerMovement.Animations.Add(CreateEntry(TEXT("walk_start_turn_right_180"), TEXT("wstart_turn_r180.onim"), false));
        PlayerMovement.Animations.Add(CreateEntry(TEXT("run_start_forward"), TEXT("runstart_fwd.onim"), false));
        PlayerMovement.Animations.Add(CreateEntry(TEXT("run_start_left_90"), TEXT("runstart_l_90.onim"), false));
        PlayerMovement.Animations.Add(CreateEntry(TEXT("run_start_right_90"), TEXT("runstart_r_90.onim"), false));
        PlayerMovement.Animations.Add(CreateEntry(TEXT("run_start_left_180"), TEXT("runstart_l_180.onim"), false));
        PlayerMovement.Animations.Add(CreateEntry(TEXT("run_start_right_180"), TEXT("runstart_r_180.onim"), false));

        // Stop animations (non-looped)
        PlayerMovement.Animations.Add(CreateEntry(TEXT("walk_stop_left"), TEXT("wstop_l.onim"), false));
        PlayerMovement.Animations.Add(CreateEntry(TEXT("walk_stop_right"), TEXT("wstop_r.onim"), false));
        PlayerMovement.Animations.Add(CreateEntry(TEXT("run_stop_left"), TEXT("rstop_l.onim"), false));
        PlayerMovement.Animations.Add(CreateEntry(TEXT("run_stop_right"), TEXT("rstop_r.onim"), false));
        PlayerMovement.Animations.Add(CreateEntry(TEXT("sprint_stop_left"), TEXT("sstop_l.onim"), false));
        PlayerMovement.Animations.Add(CreateEntry(TEXT("sprint_stop_right"), TEXT("sstop_r.onim"), false));

        // Turn animations (non-looped)
        PlayerMovement.Animations.Add(CreateEntry(TEXT("walk_turn_left"), TEXT("walk_turn_l.onim"), false));
        PlayerMovement.Animations.Add(CreateEntry(TEXT("walk_turn_right"), TEXT("walk_turn_r.onim"), false));
        PlayerMovement.Animations.Add(CreateEntry(TEXT("walk_turn_left_sharp"), TEXT("walk_turn_l2.onim"), false));
        PlayerMovement.Animations.Add(CreateEntry(TEXT("walk_turn_right_sharp"), TEXT("walk_turn_r2.onim"), false));
        PlayerMovement.Animations.Add(CreateEntry(TEXT("walk_turn_left_tight"), TEXT("walk_turn_l3.onim"), false));
        PlayerMovement.Animations.Add(CreateEntry(TEXT("walk_turn_right_tight"), TEXT("walk_turn_r3.onim"), false));
        PlayerMovement.Animations.Add(CreateEntry(TEXT("walk_turn_180_left"), TEXT("walk_turn_180_l.onim"), false));
        PlayerMovement.Animations.Add(CreateEntry(TEXT("walk_turn_180_right"), TEXT("walk_turn_180_r.onim"), false));
        PlayerMovement.Animations.Add(CreateEntry(TEXT("run_turn_left"), TEXT("run_turn_l.onim"), false));
        PlayerMovement.Animations.Add(CreateEntry(TEXT("run_turn_right"), TEXT("run_turn_r.onim"), false));
        PlayerMovement.Animations.Add(CreateEntry(TEXT("run_turn_left_sharp"), TEXT("run_turn_l2.onim"), false));
        PlayerMovement.Animations.Add(CreateEntry(TEXT("run_turn_right_sharp"), TEXT("run_turn_r2.onim"), false));
        PlayerMovement.Animations.Add(CreateEntry(TEXT("run_turn_180"), TEXT("run_turn_180.onim"), false));
        PlayerMovement.Animations.Add(CreateEntry(TEXT("run_turn_180_left"), TEXT("run_turn_180_l.onim"), false));
        PlayerMovement.Animations.Add(CreateEntry(TEXT("run_turn_180_right"), TEXT("run_turn_180_r.onim"), false));
        PlayerMovement.Animations.Add(CreateEntry(TEXT("sprint_turn_left"), TEXT("sprint_turn_l.onim"), false));
        PlayerMovement.Animations.Add(CreateEntry(TEXT("sprint_turn_right"), TEXT("sprint_turn_r.onim"), false));
        PlayerMovement.Animations.Add(CreateEntry(TEXT("sprint_turn_180_left"), TEXT("sprint_turn_180_l.onim"), false));
        PlayerMovement.Animations.Add(CreateEntry(TEXT("sprint_turn_180_right"), TEXT("sprint_turn_180_r.onim"), false));
        PlayerMovement.Animations.Add(CreateEntry(TEXT("turn_360_left"), TEXT("turn_360_l.onim"), false));
        PlayerMovement.Animations.Add(CreateEntry(TEXT("turn_360_right"), TEXT("turn_360_r.onim"), false));

        AllDictionaries.Add(PlayerMovement);
    }

    // NPC Movement Dictionary (could point to same folder but with different settings)
    {
        FAnimationDictionary NPCMovement;
        NPCMovement.DictionaryName = TEXT("npc_movement");
        NPCMovement.BaseFolderPath = TEXT("Data/Animations/move_player"); // Same folder, different tuning
        NPCMovement.Description = TEXT("NPC movement animations with different timing");

        // Copy all entries from player movement but with different play rates
        NPCMovement.Animations.Add(CreateEntry(TEXT("idle"), TEXT("idle.onim"), true, 0.9f));
        NPCMovement.Animations.Add(CreateEntry(TEXT("walk"), TEXT("walk.onim"), true, 0.8f));
        NPCMovement.Animations.Add(CreateEntry(TEXT("run"), TEXT("run.onim"), true, 0.85f));
        // ... (abbreviated for brevity, would include all animations with NPC-appropriate settings)

        AllDictionaries.Add(NPCMovement);
    }

    // Heavy Movement Dictionary (for large characters)
    {
        FAnimationDictionary HeavyMovement;
        HeavyMovement.DictionaryName = TEXT("heavy_movement");
        HeavyMovement.BaseFolderPath = TEXT("Data/Animations/move_player"); // Same folder, heavy character tuning
        HeavyMovement.Description = TEXT("Heavy character movement with slower, more deliberate animations");

        HeavyMovement.Animations.Add(CreateEntry(TEXT("idle"), TEXT("idle.onim"), true, 0.7f));
        HeavyMovement.Animations.Add(CreateEntry(TEXT("walk"), TEXT("walk.onim"), true, 0.6f));
        HeavyMovement.Animations.Add(CreateEntry(TEXT("run"), TEXT("run.onim"), true, 0.7f));
        // ... (abbreviated)

        AllDictionaries.Add(HeavyMovement);
    }

    // Crouch Movement Dictionaries
    {
        FAnimationDictionary CrouchStandard;
        CrouchStandard.DictionaryName = TEXT("crouch_standard");
        CrouchStandard.BaseFolderPath = TEXT("Data/Animations/move_crouch");
        CrouchStandard.Description = TEXT("Standard crouch movement animations");

        CrouchStandard.Animations.Add(CreateEntry(TEXT("idle"), TEXT("idle.onim"), true));
        CrouchStandard.Animations.Add(CreateEntry(TEXT("walk"), TEXT("walk.onim"), true));
        CrouchStandard.Animations.Add(CreateEntry(TEXT("run"), TEXT("run.onim"), true));
        CrouchStandard.Animations.Add(CreateEntry(TEXT("idle_to_crouch"), TEXT("idle2crouchidle.onim"), false));
        CrouchStandard.Animations.Add(CreateEntry(TEXT("crouch_to_idle"), TEXT("crouchidle2idle.onim"), false));
        CrouchStandard.Animations.Add(CreateEntry(TEXT("walk_start"), TEXT("wstart.onim"), false));
        CrouchStandard.Animations.Add(CreateEntry(TEXT("walk_stop_left"), TEXT("wstop_l.onim"), false));
        CrouchStandard.Animations.Add(CreateEntry(TEXT("walk_stop_right"), TEXT("wstop_r.onim"), false));
        CrouchStandard.Animations.Add(CreateEntry(TEXT("run_stop_left"), TEXT("rstop_l.onim"), false));
        CrouchStandard.Animations.Add(CreateEntry(TEXT("run_stop_right"), TEXT("rstop_r.onim"), false));
        CrouchStandard.Animations.Add(CreateEntry(TEXT("walk_turn_left"), TEXT("walk_turn_l.onim"), false));
        CrouchStandard.Animations.Add(CreateEntry(TEXT("walk_turn_right"), TEXT("walk_turn_r.onim"), false));
        CrouchStandard.Animations.Add(CreateEntry(TEXT("run_turn_left"), TEXT("run_turn_l.onim"), false));
        CrouchStandard.Animations.Add(CreateEntry(TEXT("run_turn_right"), TEXT("run_turn_r.onim"), false));
        CrouchStandard.Animations.Add(CreateEntry(TEXT("run_turn_left_sharp"), TEXT("run_turn_l2.onim"), false));
        CrouchStandard.Animations.Add(CreateEntry(TEXT("run_turn_right_sharp"), TEXT("run_turn_r2.onim"), false));
        CrouchStandard.Animations.Add(CreateEntry(TEXT("turn_360_left"), TEXT("turn_360_l.onim"), false));
        CrouchStandard.Animations.Add(CreateEntry(TEXT("turn_360_right"), TEXT("turn_360_r.onim"), false));

        AllDictionaries.Add(CrouchStandard);
    }
}

void UPedAnimationDictionary::CreateDefaultJumpDictionaries()
{
    FAnimationDictionary JumpStandard;
    JumpStandard.DictionaryName = TEXT("jump_standard");
    JumpStandard.BaseFolderPath = TEXT("Data/Animations/jump_std");
    JumpStandard.Description = TEXT("Standard jump animations");

    JumpStandard.Animations.Add(CreateEntry(TEXT("takeoff_left"), TEXT("jump_takeoff_l.onim"), false));
    JumpStandard.Animations.Add(CreateEntry(TEXT("takeoff_right"), TEXT("jump_takeoff_r.onim"), false));
    JumpStandard.Animations.Add(CreateEntry(TEXT("on_spot"), TEXT("jump_on_spot.onim"), false));
    JumpStandard.Animations.Add(CreateEntry(TEXT("inair_left"), TEXT("jump_inair_l.onim"), true));
    JumpStandard.Animations.Add(CreateEntry(TEXT("inair_right"), TEXT("jump_inair_r.onim"), true));
    JumpStandard.Animations.Add(CreateEntry(TEXT("land_left"), TEXT("jump_land_l.onim"), false));
    JumpStandard.Animations.Add(CreateEntry(TEXT("land_right"), TEXT("jump_land_r.onim"), false));
    JumpStandard.Animations.Add(CreateEntry(TEXT("land_roll"), TEXT("jump_land_roll.onim"), false));
    JumpStandard.Animations.Add(CreateEntry(TEXT("land_squat"), TEXT("jump_land_squat.onim"), false));

    AllDictionaries.Add(JumpStandard);
}

void UPedAnimationDictionary::CreateDefaultCombatDictionaries()
{
    FAnimationDictionary CombatStandard;
    CombatStandard.DictionaryName = TEXT("combat_standard");
    CombatStandard.BaseFolderPath = TEXT("Data/Animations/move_combat_strafe");
    CombatStandard.Description = TEXT("Standard combat movement animations");
    // Note: Would need to examine actual files in move_combat_strafe folder
    AllDictionaries.Add(CombatStandard);

    FAnimationDictionary CombatStrafe;
    CombatStrafe.DictionaryName = TEXT("combat_strafe");
    CombatStrafe.BaseFolderPath = TEXT("Data/Animations/move_combat_strafe_c");
    CombatStrafe.Description = TEXT("Combat strafe animations");
    // Note: Would need to examine actual files in move_combat_strafe_c folder
    AllDictionaries.Add(CombatStrafe);
}

void UPedAnimationDictionary::CreateDefaultVehicleDictionaries()
{
    FAnimationDictionary VehicleStandard;
    VehicleStandard.DictionaryName = TEXT("vehicle_standard");
    VehicleStandard.BaseFolderPath = TEXT("Data/Animations/veh@std");
    VehicleStandard.Description = TEXT("Standard vehicle animations");
    // Note: Would need to examine actual files in veh@std folder
    AllDictionaries.Add(VehicleStandard);
}

FAnimationEntry UPedAnimationDictionary::CreateEntry(const FString& AnimName, const FString& FileName, bool bLoop, float PlayRate)
{
    return FAnimationEntry(AnimName, FileName, bLoop, PlayRate, 0.15f);
}

bool UPedAnimationDictionary::SwapDictionary(const FString& ContextName, const FString& NewDictionaryName)
{
    FAnimationContext* Context = FindContext(ContextName);
    if (!Context)
    {
        UE_LOG(LogTemp, Error, TEXT("PedAnimationDictionary: Context '%s' not found"), *ContextName);
        return false;
    }

    // Check if the new dictionary is available for this context
    if (!Context->AvailableDictionaries.Contains(NewDictionaryName))
    {
        UE_LOG(LogTemp, Error, TEXT("PedAnimationDictionary: Dictionary '%s' not available for context '%s'"), *NewDictionaryName, *ContextName);
        return false;
    }

    // Check if the dictionary exists
    if (!FindDictionary(NewDictionaryName))
    {
        UE_LOG(LogTemp, Error, TEXT("PedAnimationDictionary: Dictionary '%s' does not exist"), *NewDictionaryName);
        return false;
    }

    FString OldDictionary = Context->CurrentDictionaryName;
    Context->CurrentDictionaryName = NewDictionaryName;

    UE_LOG(LogTemp, Log, TEXT("PedAnimationDictionary: Swapped context '%s' from '%s' to '%s'"), 
           *ContextName, *OldDictionary, *NewDictionaryName);

    // Trigger event
    OnDictionarySwapped(ContextName, OldDictionary, NewDictionaryName);

    return true;
}

FAnimationDictionary UPedAnimationDictionary::GetDictionaryByName(const FString& DictionaryName)
{
    FAnimationDictionary* Found = FindDictionary(DictionaryName);
    if (Found)
    {
        return *Found;
    }
    
    return FAnimationDictionary(); // Return empty if not found
}

FString UPedAnimationDictionary::GetCurrentDictionaryForContext(const FString& ContextName)
{
    FAnimationContext* Context = FindContext(ContextName);
    if (Context)
    {
        return Context->CurrentDictionaryName;
    }
    
    return TEXT("");
}

TArray<FString> UPedAnimationDictionary::GetAvailableDictionariesForContext(const FString& ContextName)
{
    FAnimationContext* Context = FindContext(ContextName);
    if (Context)
    {
        return Context->AvailableDictionaries;
    }
    
    return TArray<FString>();
}

FString UPedAnimationDictionary::GetAnimationPath(const FString& ContextName, const FString& AnimationName)
{
    // Get current dictionary for context
    FString DictionaryName = GetCurrentDictionaryForContext(ContextName);
    if (DictionaryName.IsEmpty())
    {
        return TEXT("");
    }

    // Get dictionary
    FAnimationDictionary Dictionary = GetDictionaryByName(DictionaryName);
    if (Dictionary.DictionaryName.IsEmpty())
    {
        return TEXT("");
    }

    // Find animation entry
    for (const FAnimationEntry& Entry : Dictionary.Animations)
    {
        if (Entry.AnimationName == AnimationName)
        {
            // Convert to UAsset path
            FString FullPath = Dictionary.BaseFolderPath + TEXT("/") + Entry.FileName;
            FullPath = FullPath.Replace(TEXT("Data/"), TEXT("/Game/Content/"));
            FullPath = FullPath.Replace(TEXT(".onim"), TEXT(""));
            return FullPath;
        }
    }

    return TEXT("");
}

FAnimationEntry UPedAnimationDictionary::GetAnimationEntry(const FString& ContextName, const FString& AnimationName)
{
    // Get current dictionary for context
    FString DictionaryName = GetCurrentDictionaryForContext(ContextName);
    if (DictionaryName.IsEmpty())
    {
        return FAnimationEntry();
    }

    // Get dictionary
    FAnimationDictionary Dictionary = GetDictionaryByName(DictionaryName);
    if (Dictionary.DictionaryName.IsEmpty())
    {
        return FAnimationEntry();
    }

    // Find animation entry
    for (const FAnimationEntry& Entry : Dictionary.Animations)
    {
        if (Entry.AnimationName == AnimationName)
        {
            return Entry;
        }
    }

    return FAnimationEntry();
}

TArray<FString> UPedAnimationDictionary::GetAllAnimationNamesInContext(const FString& ContextName)
{
    TArray<FString> AnimationNames;

    // Get current dictionary for context
    FString DictionaryName = GetCurrentDictionaryForContext(ContextName);
    if (DictionaryName.IsEmpty())
    {
        return AnimationNames;
    }

    // Get dictionary
    FAnimationDictionary Dictionary = GetDictionaryByName(DictionaryName);
    if (Dictionary.DictionaryName.IsEmpty())
    {
        return AnimationNames;
    }

    // Collect all animation names
    for (const FAnimationEntry& Entry : Dictionary.Animations)
    {
        AnimationNames.Add(Entry.AnimationName);
    }

    return AnimationNames;
}

bool UPedAnimationDictionary::AddDictionary(const FAnimationDictionary& NewDictionary)
{
    // Check if dictionary with same name already exists
    if (FindDictionary(NewDictionary.DictionaryName))
    {
        UE_LOG(LogTemp, Warning, TEXT("PedAnimationDictionary: Dictionary '%s' already exists"), *NewDictionary.DictionaryName);
        return false;
    }

    AllDictionaries.Add(NewDictionary);
    UE_LOG(LogTemp, Log, TEXT("PedAnimationDictionary: Added dictionary '%s'"), *NewDictionary.DictionaryName);
    return true;
}

bool UPedAnimationDictionary::RemoveDictionary(const FString& DictionaryName)
{
    for (int32 i = 0; i < AllDictionaries.Num(); i++)
    {
        if (AllDictionaries[i].DictionaryName == DictionaryName)
        {
            AllDictionaries.RemoveAt(i);
            UE_LOG(LogTemp, Log, TEXT("PedAnimationDictionary: Removed dictionary '%s'"), *DictionaryName);
            return true;
        }
    }

    UE_LOG(LogTemp, Warning, TEXT("PedAnimationDictionary: Dictionary '%s' not found for removal"), *DictionaryName);
    return false;
}

bool UPedAnimationDictionary::AddContext(const FAnimationContext& NewContext)
{
    // Check if context with same name already exists
    if (FindContext(NewContext.ContextName))
    {
        UE_LOG(LogTemp, Warning, TEXT("PedAnimationDictionary: Context '%s' already exists"), *NewContext.ContextName);
        return false;
    }

    AnimationContexts.Add(NewContext);
    UE_LOG(LogTemp, Log, TEXT("PedAnimationDictionary: Added context '%s'"), *NewContext.ContextName);
    return true;
}

TArray<FString> UPedAnimationDictionary::GetAllContextNames()
{
    TArray<FString> ContextNames;
    for (const FAnimationContext& Context : AnimationContexts)
    {
        ContextNames.Add(Context.ContextName);
    }
    return ContextNames;
}

TArray<FString> UPedAnimationDictionary::GetAllDictionaryNames()
{
    TArray<FString> DictionaryNames;
    for (const FAnimationDictionary& Dictionary : AllDictionaries)
    {
        DictionaryNames.Add(Dictionary.DictionaryName);
    }
    return DictionaryNames;
}

FAnimationContext* UPedAnimationDictionary::FindContext(const FString& ContextName)
{
    for (FAnimationContext& Context : AnimationContexts)
    {
        if (Context.ContextName == ContextName)
        {
            return &Context;
        }
    }
    return nullptr;
}

FAnimationDictionary* UPedAnimationDictionary::FindDictionary(const FString& DictionaryName)
{
    for (FAnimationDictionary& Dictionary : AllDictionaries)
    {
        if (Dictionary.DictionaryName == DictionaryName)
        {
            return &Dictionary;
        }
    }
    return nullptr;
}

// ============ VALIDATION SYSTEM IMPLEMENTATION ============

bool UPedAnimationDictionary::ValidateDictionary(const FString& DictionaryName, FString& OutValidationReport)
{
    FAnimationDictionary* Dictionary = FindDictionary(DictionaryName);
    if (!Dictionary)
    {
        OutValidationReport = FString::Printf(TEXT("Dictionary '%s' not found"), *DictionaryName);
        return false;
    }

    TArray<FString> MissingClips;
    bool bHasRequiredClips = ValidateRequiredClips(*Dictionary, MissingClips);
    float ValidationScore = CalculateValidationScore(*Dictionary);
    
    OutValidationReport = GenerateValidationReport(*Dictionary);
    
    // Dictionary is valid if validation score meets minimum threshold
    bool bIsValid = ValidationScore >= Dictionary->MinimumValidationScore;
    
    UE_LOG(LogTemp, Log, TEXT("Dictionary '%s' validation: Score=%.2f, MinRequired=%.2f, Valid=%s"), 
           *DictionaryName, ValidationScore, Dictionary->MinimumValidationScore, bIsValid ? TEXT("YES") : TEXT("NO"));
    
    return bIsValid;
}

bool UPedAnimationDictionary::ValidateDictionaryForContext(const FString& ContextName, const FString& DictionaryName, FString& OutValidationReport)
{
    // First validate the dictionary itself
    if (!ValidateDictionary(DictionaryName, OutValidationReport))
    {
        return false;
    }
    
    // Check if dictionary is available for this context
    FAnimationContext* Context = FindContext(ContextName);
    if (!Context)
    {
        OutValidationReport += FString::Printf(TEXT("\nContext '%s' not found"), *ContextName);
        return false;
    }
    
    if (!Context->AvailableDictionaries.Contains(DictionaryName))
    {
        OutValidationReport += FString::Printf(TEXT("\nDictionary '%s' not available for context '%s'"), *DictionaryName, *ContextName);
        return false;
    }
    
    return true;
}

float UPedAnimationDictionary::GetDictionaryValidationScore(const FString& DictionaryName)
{
    FAnimationDictionary* Dictionary = FindDictionary(DictionaryName);
    if (!Dictionary)
    {
        return 0.0f;
    }
    
    return CalculateValidationScore(*Dictionary);
}

TArray<FString> UPedAnimationDictionary::GetMissingRequiredClips(const FString& DictionaryName)
{
    TArray<FString> MissingClips;
    FAnimationDictionary* Dictionary = FindDictionary(DictionaryName);
    if (Dictionary)
    {
        ValidateRequiredClips(*Dictionary, MissingClips);
    }
    return MissingClips;
}

TArray<FString> UPedAnimationDictionary::GetAvailableClips(const FString& DictionaryName)
{
    TArray<FString> AvailableClips;
    FAnimationDictionary* Dictionary = FindDictionary(DictionaryName);
    if (Dictionary)
    {
        for (const FAnimationEntry& Entry : Dictionary->Animations)
        {
            AvailableClips.Add(Entry.AnimationName);
        }
    }
    return AvailableClips;
}

bool UPedAnimationDictionary::CanSafelySwapDictionary(const FString& ContextName, const FString& NewDictionaryName)
{
    FString ValidationReport;
    return ValidateDictionaryForContext(ContextName, NewDictionaryName, ValidationReport);
}

bool UPedAnimationDictionary::SafeSwapDictionary(const FString& ContextName, const FString& NewDictionaryName, bool bAllowFallback)
{
    FString ValidationReport;
    
    // Try to validate and swap to the new dictionary
    if (ValidateDictionaryForContext(ContextName, NewDictionaryName, ValidationReport))
    {
        bool bSwapped = SwapDictionary(ContextName, NewDictionaryName);
        if (bSwapped)
        {
            UE_LOG(LogTemp, Log, TEXT("Successfully swapped to dictionary '%s' for context '%s'"), *NewDictionaryName, *ContextName);
            return true;
        }
    }
    
    UE_LOG(LogTemp, Warning, TEXT("Failed to swap to dictionary '%s' for context '%s': %s"), *NewDictionaryName, *ContextName, *ValidationReport);
    
    // If fallback is allowed, try to find a valid alternative
    if (bAllowFallback)
    {
        FAnimationContext* Context = FindContext(ContextName);
        if (Context)
        {
            for (const FString& AlternativeDictionary : Context->AvailableDictionaries)
            {
                if (AlternativeDictionary != NewDictionaryName && CanSafelySwapDictionary(ContextName, AlternativeDictionary))
                {
                    UE_LOG(LogTemp, Log, TEXT("Falling back to dictionary '%s' for context '%s'"), *AlternativeDictionary, *ContextName);
                    return SwapDictionary(ContextName, AlternativeDictionary);
                }
            }
        }
        
        UE_LOG(LogTemp, Error, TEXT("No valid fallback dictionary found for context '%s'"), *ContextName);
    }
    
    return false;
}

void UPedAnimationDictionary::SetupDictionaryValidationRules()
{
    // Now just calls the built-in validation rules
    InitializeBuiltInValidationRules();
    UE_LOG(LogTemp, Log, TEXT("Dictionary validation rules setup complete"));
}

void UPedAnimationDictionary::InitializeBuiltInValidationRules()
{
    // Set up validation rules for all dictionaries based on actual .onim files
    for (FAnimationDictionary& Dictionary : AllDictionaries)
    {
        FString DictName = Dictionary.DictionaryName;
        
        // Clear existing rules
        Dictionary.RequiredClips.Empty();
        Dictionary.OptionalClips.Empty();
        
        // Set up rules based on dictionary type
        if (DictName.Contains(TEXT("Move_Player")) || DictName.Contains(TEXT("player_movement")))
        {
            Dictionary.RequiredClips = {TEXT("idle"), TEXT("walk"), TEXT("run"), TEXT("walk_strafe_l"), TEXT("walk_strafe_r"), TEXT("run_strafe_l"), TEXT("run_strafe_r")};
            Dictionary.OptionalClips = {TEXT("sprint"), TEXT("walk_b"), TEXT("run_strafe_b"), TEXT("wstart"), TEXT("wstop_l"), TEXT("wstop_r"), TEXT("turn_360_l"), TEXT("turn_360_r")};
            Dictionary.MinimumValidationScore = 0.7f;
        }
        else if (DictName.Contains(TEXT("Combat")))
        {
            Dictionary.RequiredClips = {TEXT("idle"), TEXT("walk"), TEXT("run"), TEXT("walk_strafe_l"), TEXT("walk_strafe_r"), TEXT("walk_strafe_b")};
            Dictionary.OptionalClips = {TEXT("run_strafe_l"), TEXT("run_strafe_r"), TEXT("shuffle_stop"), TEXT("turn_360_l"), TEXT("turn_360_r")};
            Dictionary.MinimumValidationScore = 0.6f;
        }
        else if (DictName.Contains(TEXT("Vehicle")))
        {
            Dictionary.RequiredClips = {TEXT("sit_drive"), TEXT("sit_pass"), TEXT("steer_l"), TEXT("steer_r"), TEXT("get_in_ds"), TEXT("get_out_ds")};
            Dictionary.OptionalClips = {TEXT("horn"), TEXT("keystart"), TEXT("hotwire"), TEXT("heavy_brake_ds"), TEXT("reverse")};
            Dictionary.MinimumValidationScore = 0.8f;
        }
        else if (DictName.Contains(TEXT("Jump")))
        {
            Dictionary.RequiredClips = {TEXT("jump_takeoff_l"), TEXT("jump_takeoff_r"), TEXT("jump_land_l"), TEXT("jump_land_r")};
            Dictionary.OptionalClips = {TEXT("jump_on_spot"), TEXT("jump_inair_l"), TEXT("jump_inair_r"), TEXT("jump_land_roll"), TEXT("jump_land_squat")};
            Dictionary.MinimumValidationScore = 0.75f;
        }
        else if (DictName.Contains(TEXT("Crouch")))
        {
            Dictionary.RequiredClips = {TEXT("idle"), TEXT("walk"), TEXT("idle2crouchidle"), TEXT("crouchidle2idle")};
            Dictionary.OptionalClips = {TEXT("run"), TEXT("wstart"), TEXT("wstop_l"), TEXT("wstop_r"), TEXT("walk_turn_l"), TEXT("walk_turn_r")};
            Dictionary.MinimumValidationScore = 0.75f;
        }
    }
    
    UE_LOG(LogTemp, Log, TEXT("Built-in validation rules initialized for %d dictionaries"), AllDictionaries.Num());
}

TArray<FString> UPedAnimationDictionary::GetRequiredClipsForDictionary(const FString& DictionaryName)
{
    FAnimationDictionary* Dictionary = FindDictionary(DictionaryName);
    if (Dictionary)
    {
        return Dictionary->RequiredClips;
    }
    return TArray<FString>();
}

TArray<FString> UPedAnimationDictionary::GetOptionalClipsForDictionary(const FString& DictionaryName)
{
    FAnimationDictionary* Dictionary = FindDictionary(DictionaryName);
    if (Dictionary)
    {
        return Dictionary->OptionalClips;
    }
    return TArray<FString>();
}

// ============ VALIDATION HELPER FUNCTIONS ============

bool UPedAnimationDictionary::ValidateRequiredClips(const FAnimationDictionary& Dictionary, TArray<FString>& OutMissingClips)
{
    OutMissingClips.Empty();
    
    TArray<FString> AvailableAnimNames;
    for (const FAnimationEntry& Entry : Dictionary.Animations)
    {
        AvailableAnimNames.Add(Entry.AnimationName);
    }
    
    for (const FString& RequiredClip : Dictionary.RequiredClips)
    {
        if (!AvailableAnimNames.Contains(RequiredClip))
        {
            OutMissingClips.Add(RequiredClip);
        }
    }
    
    return OutMissingClips.Num() == 0;
}

float UPedAnimationDictionary::CalculateValidationScore(const FAnimationDictionary& Dictionary)
{
    if (Dictionary.RequiredClips.Num() == 0)
    {
        return 1.0f; // No requirements = always valid
    }
    
    TArray<FString> MissingClips;
    ValidateRequiredClips(Dictionary, MissingClips);
    
    int32 FoundRequiredClips = Dictionary.RequiredClips.Num() - MissingClips.Num();
    float Score = (float)FoundRequiredClips / (float)Dictionary.RequiredClips.Num();
    
    return FMath::Clamp(Score, 0.0f, 1.0f);
}

FString UPedAnimationDictionary::GenerateValidationReport(const FAnimationDictionary& Dictionary)
{
    FString Report = FString::Printf(TEXT("=== VALIDATION REPORT: %s ===\n"), *Dictionary.DictionaryName);
    Report += FString::Printf(TEXT("Path: %s\n"), *Dictionary.BaseFolderPath);
    Report += FString::Printf(TEXT("Total Animations: %d\n"), Dictionary.Animations.Num());
    
    TArray<FString> MissingClips;
    ValidateRequiredClips(Dictionary, MissingClips);
    float ValidationScore = CalculateValidationScore(Dictionary);
    
    Report += FString::Printf(TEXT("Validation Score: %.2f (Min Required: %.2f)\n"), ValidationScore, Dictionary.MinimumValidationScore);
    Report += FString::Printf(TEXT("Required Clips: %d/%d Found\n"), Dictionary.RequiredClips.Num() - MissingClips.Num(), Dictionary.RequiredClips.Num());
    
    if (MissingClips.Num() > 0)
    {
        Report += TEXT("Missing Required Clips:\n");
        for (const FString& MissingClip : MissingClips)
        {
            Report += FString::Printf(TEXT("  - %s\n"), *MissingClip);
        }
    }
    else
    {
        Report += TEXT("✅ All required clips present!\n");
    }
    
    return Report;
}
