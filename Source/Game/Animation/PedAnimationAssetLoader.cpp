#include "PedAnimationAssetLoader.h"
#include "Engine/Engine.h"
#include "Animation/AnimSequence.h"

UPedAnimationAssetLoader::UPedAnimationAssetLoader()
{
    bInitialized = false;
}

void UPedAnimationAssetLoader::InitializeAnimationMappings()
{
    if (bInitialized)
        return;

    UE_LOG(LogTemp, Log, TEXT("PedAnimationAssetLoader: Initializing animation mappings..."));

    // Clear existing data
    LoadedAnimations.Empty();

    // Create all mappings for different categories
    CreatePlayerMovementMappings();
    CreateCrouchMovementMappings(); 
    CreateJumpAnimationMappings();
    CreateCombatMovementMappings();
    CreateClimbAnimationMappings();
    CreateVehicleAnimationMappings();

    bInitialized = true;
    UE_LOG(LogTemp, Log, TEXT("PedAnimationAssetLoader: Animation mappings initialized successfully"));
}

void UPedAnimationAssetLoader::CreatePlayerMovementMappings()
{
    PlayerMovement.CategoryName = TEXT("PlayerMovement");
    PlayerMovement.Animations.Empty();

    // Basic Movement Animations
    PlayerMovement.Animations.Add(CreateMapping(TEXT("Data/Animations/move_player/idle.onim"), TEXT("idle"), true));
    PlayerMovement.Animations.Add(CreateMapping(TEXT("Data/Animations/move_player/walk.onim"), TEXT("walk"), true));
    PlayerMovement.Animations.Add(CreateMapping(TEXT("Data/Animations/move_player/run.onim"), TEXT("run"), true));
    PlayerMovement.Animations.Add(CreateMapping(TEXT("Data/Animations/move_player/sprint.onim"), TEXT("sprint"), true));

    // Directional Movement
    PlayerMovement.Animations.Add(CreateMapping(TEXT("Data/Animations/move_player/walk_b.onim"), TEXT("walk_backward"), true));
    PlayerMovement.Animations.Add(CreateMapping(TEXT("Data/Animations/move_player/walk_strafe_l.onim"), TEXT("walk_strafe_left"), true));
    PlayerMovement.Animations.Add(CreateMapping(TEXT("Data/Animations/move_player/walk_strafe_r.onim"), TEXT("walk_strafe_right"), true));
    PlayerMovement.Animations.Add(CreateMapping(TEXT("Data/Animations/move_player/walk_strafe_b.onim"), TEXT("walk_strafe_backward"), true));
    PlayerMovement.Animations.Add(CreateMapping(TEXT("Data/Animations/move_player/run_strafe_l.onim"), TEXT("run_strafe_left"), true));
    PlayerMovement.Animations.Add(CreateMapping(TEXT("Data/Animations/move_player/run_strafe_r.onim"), TEXT("run_strafe_right"), true));
    PlayerMovement.Animations.Add(CreateMapping(TEXT("Data/Animations/move_player/run_strafe_b.onim"), TEXT("run_strafe_backward"), true));
    PlayerMovement.Animations.Add(CreateMapping(TEXT("Data/Animations/move_player/run_up.onim"), TEXT("run_up"), true));
    PlayerMovement.Animations.Add(CreateMapping(TEXT("Data/Animations/move_player/run_down.onim"), TEXT("run_down"), true));
    PlayerMovement.Animations.Add(CreateMapping(TEXT("Data/Animations/move_player/walk_up.onim"), TEXT("walk_up"), true));
    PlayerMovement.Animations.Add(CreateMapping(TEXT("Data/Animations/move_player/walk_down.onim"), TEXT("walk_down"), true));
    PlayerMovement.Animations.Add(CreateMapping(TEXT("Data/Animations/move_player/walk_c.onim"), TEXT("walk_casual"), true));

    // Start Animations
    PlayerMovement.Animations.Add(CreateMapping(TEXT("Data/Animations/move_player/wstart.onim"), TEXT("walk_start"), false));
    PlayerMovement.Animations.Add(CreateMapping(TEXT("Data/Animations/move_player/wstart_turn_l90.onim"), TEXT("walk_start_turn_left_90"), false));
    PlayerMovement.Animations.Add(CreateMapping(TEXT("Data/Animations/move_player/wstart_turn_r90.onim"), TEXT("walk_start_turn_right_90"), false));
    PlayerMovement.Animations.Add(CreateMapping(TEXT("Data/Animations/move_player/wstart_turn_l180.onim"), TEXT("walk_start_turn_left_180"), false));
    PlayerMovement.Animations.Add(CreateMapping(TEXT("Data/Animations/move_player/wstart_turn_r180.onim"), TEXT("walk_start_turn_right_180"), false));
    PlayerMovement.Animations.Add(CreateMapping(TEXT("Data/Animations/move_player/runstart_fwd.onim"), TEXT("run_start_forward"), false));
    PlayerMovement.Animations.Add(CreateMapping(TEXT("Data/Animations/move_player/runstart_l_90.onim"), TEXT("run_start_left_90"), false));
    PlayerMovement.Animations.Add(CreateMapping(TEXT("Data/Animations/move_player/runstart_r_90.onim"), TEXT("run_start_right_90"), false));
    PlayerMovement.Animations.Add(CreateMapping(TEXT("Data/Animations/move_player/runstart_l_180.onim"), TEXT("run_start_left_180"), false));
    PlayerMovement.Animations.Add(CreateMapping(TEXT("Data/Animations/move_player/runstart_r_180.onim"), TEXT("run_start_right_180"), false));

    // Stop Animations
    PlayerMovement.Animations.Add(CreateMapping(TEXT("Data/Animations/move_player/wstop_l.onim"), TEXT("walk_stop_left"), false));
    PlayerMovement.Animations.Add(CreateMapping(TEXT("Data/Animations/move_player/wstop_r.onim"), TEXT("walk_stop_right"), false));
    PlayerMovement.Animations.Add(CreateMapping(TEXT("Data/Animations/move_player/rstop_l.onim"), TEXT("run_stop_left"), false));
    PlayerMovement.Animations.Add(CreateMapping(TEXT("Data/Animations/move_player/rstop_r.onim"), TEXT("run_stop_right"), false));
    PlayerMovement.Animations.Add(CreateMapping(TEXT("Data/Animations/move_player/sstop_l.onim"), TEXT("sprint_stop_left"), false));
    PlayerMovement.Animations.Add(CreateMapping(TEXT("Data/Animations/move_player/sstop_r.onim"), TEXT("sprint_stop_right"), false));

    // Turn Animations
    PlayerMovement.Animations.Add(CreateMapping(TEXT("Data/Animations/move_player/walk_turn_l.onim"), TEXT("walk_turn_left"), false));
    PlayerMovement.Animations.Add(CreateMapping(TEXT("Data/Animations/move_player/walk_turn_r.onim"), TEXT("walk_turn_right"), false));
    PlayerMovement.Animations.Add(CreateMapping(TEXT("Data/Animations/move_player/walk_turn_l2.onim"), TEXT("walk_turn_left_sharp"), false));
    PlayerMovement.Animations.Add(CreateMapping(TEXT("Data/Animations/move_player/walk_turn_r2.onim"), TEXT("walk_turn_right_sharp"), false));
    PlayerMovement.Animations.Add(CreateMapping(TEXT("Data/Animations/move_player/walk_turn_l3.onim"), TEXT("walk_turn_left_tight"), false));
    PlayerMovement.Animations.Add(CreateMapping(TEXT("Data/Animations/move_player/walk_turn_r3.onim"), TEXT("walk_turn_right_tight"), false));
    PlayerMovement.Animations.Add(CreateMapping(TEXT("Data/Animations/move_player/walk_turn_180_l.onim"), TEXT("walk_turn_180_left"), false));
    PlayerMovement.Animations.Add(CreateMapping(TEXT("Data/Animations/move_player/walk_turn_180_r.onim"), TEXT("walk_turn_180_right"), false));
    PlayerMovement.Animations.Add(CreateMapping(TEXT("Data/Animations/move_player/run_turn_l.onim"), TEXT("run_turn_left"), false));
    PlayerMovement.Animations.Add(CreateMapping(TEXT("Data/Animations/move_player/run_turn_r.onim"), TEXT("run_turn_right"), false));
    PlayerMovement.Animations.Add(CreateMapping(TEXT("Data/Animations/move_player/run_turn_l2.onim"), TEXT("run_turn_left_sharp"), false));
    PlayerMovement.Animations.Add(CreateMapping(TEXT("Data/Animations/move_player/run_turn_r2.onim"), TEXT("run_turn_right_sharp"), false));
    PlayerMovement.Animations.Add(CreateMapping(TEXT("Data/Animations/move_player/run_turn_180.onim"), TEXT("run_turn_180"), false));
    PlayerMovement.Animations.Add(CreateMapping(TEXT("Data/Animations/move_player/run_turn_180_l.onim"), TEXT("run_turn_180_left"), false));
    PlayerMovement.Animations.Add(CreateMapping(TEXT("Data/Animations/move_player/run_turn_180_r.onim"), TEXT("run_turn_180_right"), false));
    PlayerMovement.Animations.Add(CreateMapping(TEXT("Data/Animations/move_player/sprint_turn_l.onim"), TEXT("sprint_turn_left"), false));
    PlayerMovement.Animations.Add(CreateMapping(TEXT("Data/Animations/move_player/sprint_turn_r.onim"), TEXT("sprint_turn_right"), false));
    PlayerMovement.Animations.Add(CreateMapping(TEXT("Data/Animations/move_player/sprint_turn_180_l.onim"), TEXT("sprint_turn_180_left"), false));
    PlayerMovement.Animations.Add(CreateMapping(TEXT("Data/Animations/move_player/sprint_turn_180_r.onim"), TEXT("sprint_turn_180_right"), false));
    PlayerMovement.Animations.Add(CreateMapping(TEXT("Data/Animations/move_player/turn_360_l.onim"), TEXT("turn_360_left"), false));
    PlayerMovement.Animations.Add(CreateMapping(TEXT("Data/Animations/move_player/turn_360_r.onim"), TEXT("turn_360_right"), false));
}

void UPedAnimationAssetLoader::CreateCrouchMovementMappings()
{
    CrouchMovement.CategoryName = TEXT("CrouchMovement");
    CrouchMovement.Animations.Empty();

    // Basic Crouch Animations
    CrouchMovement.Animations.Add(CreateMapping(TEXT("Data/Animations/move_crouch/idle.onim"), TEXT("crouch_idle"), true));
    CrouchMovement.Animations.Add(CreateMapping(TEXT("Data/Animations/move_crouch/walk.onim"), TEXT("crouch_walk"), true));
    CrouchMovement.Animations.Add(CreateMapping(TEXT("Data/Animations/move_crouch/run.onim"), TEXT("crouch_run"), true));

    // Crouch Transitions
    CrouchMovement.Animations.Add(CreateMapping(TEXT("Data/Animations/move_crouch/idle2crouchidle.onim"), TEXT("idle_to_crouch"), false));
    CrouchMovement.Animations.Add(CreateMapping(TEXT("Data/Animations/move_crouch/crouchidle2idle.onim"), TEXT("crouch_to_idle"), false));

    // Crouch Movement Starts/Stops
    CrouchMovement.Animations.Add(CreateMapping(TEXT("Data/Animations/move_crouch/wstart.onim"), TEXT("crouch_walk_start"), false));
    CrouchMovement.Animations.Add(CreateMapping(TEXT("Data/Animations/move_crouch/wstop_l.onim"), TEXT("crouch_walk_stop_left"), false));
    CrouchMovement.Animations.Add(CreateMapping(TEXT("Data/Animations/move_crouch/wstop_r.onim"), TEXT("crouch_walk_stop_right"), false));
    CrouchMovement.Animations.Add(CreateMapping(TEXT("Data/Animations/move_crouch/rstop_l.onim"), TEXT("crouch_run_stop_left"), false));
    CrouchMovement.Animations.Add(CreateMapping(TEXT("Data/Animations/move_crouch/rstop_r.onim"), TEXT("crouch_run_stop_right"), false));

    // Crouch Turns
    CrouchMovement.Animations.Add(CreateMapping(TEXT("Data/Animations/move_crouch/walk_turn_l.onim"), TEXT("crouch_walk_turn_left"), false));
    CrouchMovement.Animations.Add(CreateMapping(TEXT("Data/Animations/move_crouch/walk_turn_r.onim"), TEXT("crouch_walk_turn_right"), false));
    CrouchMovement.Animations.Add(CreateMapping(TEXT("Data/Animations/move_crouch/run_turn_l.onim"), TEXT("crouch_run_turn_left"), false));
    CrouchMovement.Animations.Add(CreateMapping(TEXT("Data/Animations/move_crouch/run_turn_r.onim"), TEXT("crouch_run_turn_right"), false));
    CrouchMovement.Animations.Add(CreateMapping(TEXT("Data/Animations/move_crouch/run_turn_l2.onim"), TEXT("crouch_run_turn_left_sharp"), false));
    CrouchMovement.Animations.Add(CreateMapping(TEXT("Data/Animations/move_crouch/run_turn_r2.onim"), TEXT("crouch_run_turn_right_sharp"), false));
    CrouchMovement.Animations.Add(CreateMapping(TEXT("Data/Animations/move_crouch/turn_360_l.onim"), TEXT("crouch_turn_360_left"), false));
    CrouchMovement.Animations.Add(CreateMapping(TEXT("Data/Animations/move_crouch/turn_360_r.onim"), TEXT("crouch_turn_360_right"), false));
}

void UPedAnimationAssetLoader::CreateJumpAnimationMappings()
{
    JumpAnimations.CategoryName = TEXT("JumpAnimations");
    JumpAnimations.Animations.Empty();

    // Jump Takeoffs
    JumpAnimations.Animations.Add(CreateMapping(TEXT("Data/Animations/jump_std/jump_takeoff_l.onim"), TEXT("jump_takeoff_left"), false));
    JumpAnimations.Animations.Add(CreateMapping(TEXT("Data/Animations/jump_std/jump_takeoff_r.onim"), TEXT("jump_takeoff_right"), false));
    JumpAnimations.Animations.Add(CreateMapping(TEXT("Data/Animations/jump_std/jump_on_spot.onim"), TEXT("jump_on_spot"), false));

    // In Air
    JumpAnimations.Animations.Add(CreateMapping(TEXT("Data/Animations/jump_std/jump_inair_l.onim"), TEXT("jump_inair_left"), true));
    JumpAnimations.Animations.Add(CreateMapping(TEXT("Data/Animations/jump_std/jump_inair_r.onim"), TEXT("jump_inair_right"), true));

    // Landings
    JumpAnimations.Animations.Add(CreateMapping(TEXT("Data/Animations/jump_std/jump_land_l.onim"), TEXT("jump_land_left"), false));
    JumpAnimations.Animations.Add(CreateMapping(TEXT("Data/Animations/jump_std/jump_land_r.onim"), TEXT("jump_land_right"), false));
    JumpAnimations.Animations.Add(CreateMapping(TEXT("Data/Animations/jump_std/jump_land_roll.onim"), TEXT("jump_land_roll"), false));
    JumpAnimations.Animations.Add(CreateMapping(TEXT("Data/Animations/jump_std/jump_land_squat.onim"), TEXT("jump_land_squat"), false));
}

void UPedAnimationAssetLoader::CreateCombatMovementMappings()
{
    CombatMovement.CategoryName = TEXT("CombatMovement");
    CombatMovement.Animations.Empty();

    CombatStrafe.CategoryName = TEXT("CombatStrafe");
    CombatStrafe.Animations.Empty();

    // Note: The actual .onim files in move_combat_strafe and move_combat_strafe_c folders
    // would need to be examined to create proper mappings. For now, we'll create placeholder mappings.
    
    UE_LOG(LogTemp, Warning, TEXT("PedAnimationAssetLoader: Combat movement mappings need to be implemented based on actual .onim files"));
}

void UPedAnimationAssetLoader::CreateClimbAnimationMappings()
{
    ClimbAnimations.CategoryName = TEXT("ClimbAnimations");
    ClimbAnimations.Animations.Empty();

    // Note: The actual .onim files in climb_std folder would need to be examined
    UE_LOG(LogTemp, Warning, TEXT("PedAnimationAssetLoader: Climb animation mappings need to be implemented based on actual .onim files"));
}

void UPedAnimationAssetLoader::CreateVehicleAnimationMappings()
{
    VehicleAnimations.CategoryName = TEXT("VehicleAnimations");
    VehicleAnimations.Animations.Empty();

    // Note: The actual .onim files in veh@std folder would need to be examined
    UE_LOG(LogTemp, Warning, TEXT("PedAnimationAssetLoader: Vehicle animation mappings need to be implemented based on actual .onim files"));
}

FAnimationFileMapping UPedAnimationAssetLoader::CreateMapping(const FString& OnimPath, const FString& AnimName, bool bLoop, float PlayRate)
{
    FAnimationFileMapping Mapping;
    Mapping.OnimFilePath = OnimPath;
    Mapping.AnimationName = AnimName;
    Mapping.UAssetPath = ConvertOnimPathToUAsset(OnimPath);
    Mapping.bIsLooped = bLoop;
    Mapping.DefaultPlayRate = PlayRate;
    Mapping.DefaultBlendTime = 0.15f;
    
    return Mapping;
}

FString UPedAnimationAssetLoader::ConvertOnimPathToUAsset(const FString& OnimPath)
{
    // Convert from Data/Animations/... to /Game/Content/Animations/...
    FString UAssetPath = OnimPath;
    UAssetPath = UAssetPath.Replace(TEXT("Data/"), TEXT("/Game/Content/"));
    UAssetPath = UAssetPath.Replace(TEXT(".onim"), TEXT(""));
    
    return UAssetPath;
}

UAnimSequence* UPedAnimationAssetLoader::GetAnimationByName(const FString& AnimationName)
{
    // Check cache first
    if (UAnimSequence** FoundAnim = LoadedAnimations.Find(AnimationName))
    {
        return *FoundAnim;
    }

    // Find the mapping
    FAnimationFileMapping* FoundMapping = nullptr;
    
    // Search through all categories
    TArray<FAnimationCategory*> Categories = {
        &PlayerMovement, &CrouchMovement, &JumpAnimations, 
        &CombatMovement, &CombatStrafe, &ClimbAnimations, &VehicleAnimations
    };

    for (FAnimationCategory* Category : Categories)
    {
        for (FAnimationFileMapping& Mapping : Category->Animations)
        {
            if (Mapping.AnimationName == AnimationName)
            {
                FoundMapping = &Mapping;
                break;
            }
        }
        if (FoundMapping) break;
    }

    if (!FoundMapping)
    {
        UE_LOG(LogTemp, Warning, TEXT("PedAnimationAssetLoader: Animation '%s' not found in mappings"), *AnimationName);
        return nullptr;
    }

    // Try to load the animation
    UAnimSequence* LoadedAnim = LoadObject<UAnimSequence>(nullptr, *FoundMapping->UAssetPath);
    
    if (LoadedAnim)
    {
        // Cache the loaded animation
        LoadedAnimations.Add(AnimationName, LoadedAnim);
        UE_LOG(LogTemp, Log, TEXT("PedAnimationAssetLoader: Successfully loaded animation '%s' from '%s'"), 
               *AnimationName, *FoundMapping->UAssetPath);
    }
    else
    {
        UE_LOG(LogTemp, Error, TEXT("PedAnimationAssetLoader: Failed to load animation '%s' from '%s'"), 
               *AnimationName, *FoundMapping->UAssetPath);
    }

    return LoadedAnim;
}

TArray<UAnimSequence*> UPedAnimationAssetLoader::GetAnimationsByCategory(const FString& CategoryName)
{
    TArray<UAnimSequence*> CategoryAnimations;
    
    FAnimationCategory* TargetCategory = nullptr;
    
    if (CategoryName == TEXT("PlayerMovement"))
        TargetCategory = &PlayerMovement;
    else if (CategoryName == TEXT("CrouchMovement"))
        TargetCategory = &CrouchMovement;
    else if (CategoryName == TEXT("JumpAnimations"))
        TargetCategory = &JumpAnimations;
    else if (CategoryName == TEXT("CombatMovement"))
        TargetCategory = &CombatMovement;
    else if (CategoryName == TEXT("CombatStrafe"))
        TargetCategory = &CombatStrafe;
    else if (CategoryName == TEXT("ClimbAnimations"))
        TargetCategory = &ClimbAnimations;
    else if (CategoryName == TEXT("VehicleAnimations"))
        TargetCategory = &VehicleAnimations;

    if (TargetCategory)
    {
        for (const FAnimationFileMapping& Mapping : TargetCategory->Animations)
        {
            UAnimSequence* Anim = GetAnimationByName(Mapping.AnimationName);
            if (Anim)
            {
                CategoryAnimations.Add(Anim);
            }
        }
    }

    return CategoryAnimations;
}

bool UPedAnimationAssetLoader::LoadAllAnimations()
{
    if (!bInitialized)
    {
        InitializeAnimationMappings();
    }

    UE_LOG(LogTemp, Log, TEXT("PedAnimationAssetLoader: Loading all animations..."));

    int32 SuccessCount = 0;
    int32 TotalCount = 0;

    TArray<FAnimationCategory*> Categories = {
        &PlayerMovement, &CrouchMovement, &JumpAnimations, 
        &CombatMovement, &CombatStrafe, &ClimbAnimations, &VehicleAnimations
    };

    for (FAnimationCategory* Category : Categories)
    {
        for (const FAnimationFileMapping& Mapping : Category->Animations)
        {
            TotalCount++;
            if (GetAnimationByName(Mapping.AnimationName))
            {
                SuccessCount++;
            }
        }
    }

    UE_LOG(LogTemp, Log, TEXT("PedAnimationAssetLoader: Loaded %d/%d animations successfully"), SuccessCount, TotalCount);
    
    return SuccessCount == TotalCount;
}

bool UPedAnimationAssetLoader::IsAnimationLoaded(const FString& AnimationName)
{
    return LoadedAnimations.Contains(AnimationName);
}

TArray<FString> UPedAnimationAssetLoader::GetAllAnimationNames()
{
    TArray<FString> AllNames;
    
    TArray<FAnimationCategory*> Categories = {
        &PlayerMovement, &CrouchMovement, &JumpAnimations, 
        &CombatMovement, &CombatStrafe, &ClimbAnimations, &VehicleAnimations
    };

    for (FAnimationCategory* Category : Categories)
    {
        for (const FAnimationFileMapping& Mapping : Category->Animations)
        {
            AllNames.Add(Mapping.AnimationName);
        }
    }

    return AllNames;
}

TArray<FString> UPedAnimationAssetLoader::GetAnimationNamesByCategory(const FString& CategoryName)
{
    TArray<FString> CategoryNames;
    
    FAnimationCategory* TargetCategory = nullptr;
    
    if (CategoryName == TEXT("PlayerMovement"))
        TargetCategory = &PlayerMovement;
    else if (CategoryName == TEXT("CrouchMovement"))
        TargetCategory = &CrouchMovement;
    else if (CategoryName == TEXT("JumpAnimations"))
        TargetCategory = &JumpAnimations;
    else if (CategoryName == TEXT("CombatMovement"))
        TargetCategory = &CombatMovement;
    else if (CategoryName == TEXT("CombatStrafe"))
        TargetCategory = &CombatStrafe;
    else if (CategoryName == TEXT("ClimbAnimations"))
        TargetCategory = &ClimbAnimations;
    else if (CategoryName == TEXT("VehicleAnimations"))
        TargetCategory = &VehicleAnimations;

    if (TargetCategory)
    {
        for (const FAnimationFileMapping& Mapping : TargetCategory->Animations)
        {
            CategoryNames.Add(Mapping.AnimationName);
        }
    }

    return CategoryNames;
}

FAnimationFileMapping UPedAnimationAssetLoader::GetAnimationMapping(const FString& AnimationName)
{
    TArray<FAnimationCategory*> Categories = {
        &PlayerMovement, &CrouchMovement, &JumpAnimations, 
        &CombatMovement, &CombatStrafe, &ClimbAnimations, &VehicleAnimations
    };

    for (FAnimationCategory* Category : Categories)
    {
        for (const FAnimationFileMapping& Mapping : Category->Animations)
        {
            if (Mapping.AnimationName == AnimationName)
            {
                return Mapping;
            }
        }
    }

    return FAnimationFileMapping(); // Return empty mapping if not found
}

UAnimSequence* UPedAnimationAssetLoader::LoadAnimationAsset(const FString& AnimationPath)
{
    // Convert .onim path to .uasset path
    FString UAssetPath = ConvertOnimPathToUAsset(AnimationPath);
    
    // Check if already loaded in cache
    if (LoadedAnimations.Contains(UAssetPath))
    {
        return LoadedAnimations[UAssetPath];
    }
    
    // Try to load the animation asset
    UAnimSequence* AnimSequence = LoadObject<UAnimSequence>(nullptr, *UAssetPath);
    
    if (AnimSequence)
    {
        // Cache the loaded animation
        LoadedAnimations.Add(UAssetPath, AnimSequence);
        UE_LOG(LogTemp, Log, TEXT("PedAnimationAssetLoader: Successfully loaded animation: %s"), *UAssetPath);
        return AnimSequence;
    }
    else
    {
        UE_LOG(LogTemp, Warning, TEXT("PedAnimationAssetLoader: Failed to load animation: %s"), *UAssetPath);
        return nullptr;
    }
}
