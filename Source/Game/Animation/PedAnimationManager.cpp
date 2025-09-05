#include "PedAnimationManager.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Components/SkeletalMeshComponent.h"
#include "Engine/Engine.h"

UPedAnimationManager::UPedAnimationManager()
{
    PrimaryComponentTick.bCanEverTick = true;
    PrimaryComponentTick.TickGroup = TG_PrePhysics;

    // Initialize default values
    MovementThreshold = 5.0f;
    WalkSpeedThreshold = 150.0f;
    RunSpeedThreshold = 400.0f;
    TurnAngleThreshold = 15.0f;
    bAutoManageAnimations = true;
    bEnableSmoothing = true;
    SmoothingSpeed = 10.0f;

    // Initialize state variables
    LastMovementState = EPedMovementState::Idle;
    LastStanceState = EPedStanceState::Standing;
    LastSpeed = 0.0f;
    LastAnimationName = TEXT("");

    SmoothedSpeed = 0.0f;
    SmoothedDirection = 0.0f;
    TimeSinceLastStateChange = 0.0f;
    TimeSinceLastAnimation = 0.0f;

    AnimationController = nullptr;
    OwnerCharacter = nullptr;
    AnimationAssetLoader = nullptr;
    AnimationDictionary = nullptr;
    AnimationGroupsLoader = nullptr;
    AnimationGroupsXMLPath = TEXT("Data/Animations/AnimationGroups.xml");
}

void UPedAnimationManager::BeginPlay()
{
    Super::BeginPlay();

    // Get character reference
    OwnerCharacter = Cast<ACharacter>(GetOwner());
    if (!OwnerCharacter)
    {
        UE_LOG(LogTemp, Error, TEXT("PedAnimationManager: Owner is not a Character!"));
        return;
    }

    // Get animation controller from character's mesh
    if (OwnerCharacter->GetMesh())
    {
        AnimationController = Cast<UPedAnimationController>(OwnerCharacter->GetMesh()->GetAnimInstance());
        if (!AnimationController)
        {
            UE_LOG(LogTemp, Warning, TEXT("PedAnimationManager: No PedAnimationController found on character mesh"));
        }
    }

    // Initialize animation system
    InitializeAnimationSystem();

    UE_LOG(LogTemp, Log, TEXT("PedAnimationManager: Initialized for character %s"), 
           *OwnerCharacter->GetName());
}

void UPedAnimationManager::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
    Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

    if (!OwnerCharacter)
        return;

    // Update timing
    TimeSinceLastStateChange += DeltaTime;
    TimeSinceLastAnimation += DeltaTime;

    // Update smoothing values
    if (bEnableSmoothing)
    {
        UpdateSmoothingValues(DeltaTime);
    }

    // Update movement detection
    UpdateMovementDetection();

    // Check for state changes
    CheckForStateChanges();

    // Handle automatic animation management
    if (bAutoManageAnimations)
    {
        HandleAutoAnimationManagement();
    }
}

void UPedAnimationManager::UpdateMovementDetection()
{
    if (!OwnerCharacter || !OwnerCharacter->GetCharacterMovement())
        return;

    UCharacterMovementComponent* Movement = OwnerCharacter->GetCharacterMovement();
    FVector CurrentVelocity = Movement->Velocity;
    float CurrentSpeed = CurrentVelocity.Size();

    // Update smoothed values
    if (bEnableSmoothing)
    {
        // Speed is already smoothed in UpdateSmoothingValues
    }
    else
    {
        SmoothedSpeed = CurrentSpeed;
    }

    LastSpeed = CurrentSpeed;
}

void UPedAnimationManager::UpdateSmoothingValues(float DeltaTime)
{
    if (!OwnerCharacter || !OwnerCharacter->GetCharacterMovement())
        return;

    UCharacterMovementComponent* Movement = OwnerCharacter->GetCharacterMovement();
    FVector CurrentVelocity = Movement->Velocity;
    float CurrentSpeed = CurrentVelocity.Size();

    // Smooth speed changes
    SmoothedSpeed = FMath::FInterpTo(SmoothedSpeed, CurrentSpeed, DeltaTime, SmoothingSpeed);

    // Smooth direction changes
    if (CurrentSpeed > MovementThreshold)
    {
        FVector ForwardVector = OwnerCharacter->GetActorForwardVector();
        FVector VelocityNormalized = CurrentVelocity.GetSafeNormal();
        float CurrentDirection = FMath::Atan2(
            FVector::CrossProduct(ForwardVector, VelocityNormalized).Z,
            FVector::DotProduct(ForwardVector, VelocityNormalized)
        ) * 180.0f / PI;

        SmoothedDirection = FMath::FInterpTo(SmoothedDirection, CurrentDirection, DeltaTime, SmoothingSpeed);
    }
}

void UPedAnimationManager::CheckForStateChanges()
{
    if (!AnimationController)
        return;

    EPedMovementState CurrentMovementState = AnimationController->CurrentMovementState;
    EPedStanceState CurrentStanceState = AnimationController->CurrentStanceState;

    // Check for movement state changes
    if (CurrentMovementState != LastMovementState)
    {
        OnMovementStateChanged(LastMovementState, CurrentMovementState);
        LastMovementState = CurrentMovementState;
        TimeSinceLastStateChange = 0.0f;
    }

    // Check for stance state changes
    if (CurrentStanceState != LastStanceState)
    {
        OnStanceStateChanged(LastStanceState, CurrentStanceState);
        LastStanceState = CurrentStanceState;
        TimeSinceLastStateChange = 0.0f;
    }

    // Check for animation changes
    FString CurrentAnimationName = AnimationController->CurrentAnimationName;
    if (CurrentAnimationName != LastAnimationName)
    {
        if (!LastAnimationName.IsEmpty())
        {
            OnAnimationFinished(LastAnimationName);
        }
        
        if (!CurrentAnimationName.IsEmpty() && CurrentAnimationName != TEXT("None"))
        {
            OnAnimationStarted(CurrentAnimationName);
        }
        
        LastAnimationName = CurrentAnimationName;
        TimeSinceLastAnimation = 0.0f;
    }
}

void UPedAnimationManager::HandleAutoAnimationManagement()
{
    if (!AnimationController || !OwnerCharacter)
        return;

    EPedMovementState CurrentState = AnimationController->CurrentMovementState;
    
    // Only auto-manage if we can cancel current animation or if enough time has passed
    if (!AnimationController->CanPlayAnimation() && TimeSinceLastStateChange < 0.5f)
        return;

    FString SelectedAnimation;

    // Select appropriate animation based on current state
    switch (CurrentState)
    {
        case EPedMovementState::Idle:
            SelectedAnimation = SelectBestIdleAnimation();
            break;
            
        case EPedMovementState::Walking:
            SelectedAnimation = SelectBestWalkAnimation();
            break;
            
        case EPedMovementState::Running:
            SelectedAnimation = SelectBestRunAnimation();
            break;
            
        case EPedMovementState::Sprinting:
            SelectedAnimation = SelectBestSprintAnimation();
            break;
            
        case EPedMovementState::Jumping:
            SelectedAnimation = SelectBestJumpAnimation();
            break;
            
        case EPedMovementState::Crouching:
            SelectedAnimation = SelectBestCrouchAnimation();
            break;
            
        default:
            break;
    }

    // Play selected animation if it's different from current
    if (!SelectedAnimation.IsEmpty() && SelectedAnimation != AnimationController->CurrentAnimationName)
    {
        PlayAnimationByName(SelectedAnimation);
    }
}

FString UPedAnimationManager::SelectBestIdleAnimation()
{
    if (AnimationController->CurrentStanceState == EPedStanceState::Crouched)
    {
        return TEXT("idle"); // Will resolve to crouch context
    }
    
    return TEXT("idle"); // Will resolve to movement context
}

FString UPedAnimationManager::SelectBestWalkAnimation()
{
    if (AnimationController->CurrentStanceState == EPedStanceState::Crouched)
    {
        return TEXT("crouch_walk");
    }

    // Check movement direction for directional walking
    float Direction = SmoothedDirection;
    
    if (FMath::Abs(Direction) < 30.0f) // Forward
    {
        return TEXT("walk");
    }
    else if (Direction > 150.0f || Direction < -150.0f) // Backward
    {
        return TEXT("walk_backward");
    }
    else if (Direction > 30.0f && Direction < 150.0f) // Right strafe
    {
        return TEXT("walk_strafe_right");
    }
    else if (Direction > -150.0f && Direction < -30.0f) // Left strafe
    {
        return TEXT("walk_strafe_left");
    }

    return TEXT("walk");
}

FString UPedAnimationManager::SelectBestRunAnimation()
{
    if (AnimationController->CurrentStanceState == EPedStanceState::Crouched)
    {
        return TEXT("crouch_run");
    }

    // Check movement direction for directional running
    float Direction = SmoothedDirection;
    
    if (FMath::Abs(Direction) < 30.0f) // Forward
    {
        return TEXT("run");
    }
    else if (Direction > 150.0f || Direction < -150.0f) // Backward
    {
        return TEXT("run_strafe_backward");
    }
    else if (Direction > 30.0f && Direction < 150.0f) // Right strafe
    {
        return TEXT("run_strafe_right");
    }
    else if (Direction > -150.0f && Direction < -30.0f) // Left strafe
    {
        return TEXT("run_strafe_left");
    }

    return TEXT("run");
}

FString UPedAnimationManager::SelectBestSprintAnimation()
{
    return TEXT("sprint");
}

FString UPedAnimationManager::SelectBestJumpAnimation()
{
    if (!OwnerCharacter || !OwnerCharacter->GetCharacterMovement())
        return TEXT("jump_on_spot");

    UCharacterMovementComponent* Movement = OwnerCharacter->GetCharacterMovement();
    
    if (Movement->IsFalling())
    {
        if (Movement->Velocity.Z > 0) // Going up
        {
            return TEXT("jump_takeoff_left"); // Could alternate or choose based on foot
        }
        else // Falling down
        {
            return TEXT("jump_inair_left"); // Could alternate
        }
    }
    else
    {
        return TEXT("jump_land_left"); // Landing
    }
}

FString UPedAnimationManager::SelectBestCrouchAnimation()
{
    if (SmoothedSpeed < MovementThreshold)
    {
        return TEXT("crouch_idle");
    }
    else if (SmoothedSpeed < RunSpeedThreshold)
    {
        return TEXT("crouch_walk");
    }
    else
    {
        return TEXT("crouch_run");
    }
}

void UPedAnimationManager::PlayAnimationByName(const FString& AnimationName, bool bForcePlay)
{
    if (!AnimationController || !AnimationDictionary)
    {
        UE_LOG(LogTemp, Warning, TEXT("PedAnimationManager: Cannot play animation - missing controller or dictionary"));
        return;
    }

    // Determine context based on current animation region
    EPedAnimationRegion CurrentRegion = AnimationController->DetermineAnimationRegion();
    FString ContextName;

    switch (CurrentRegion)
    {
        case EPedAnimationRegion::OnFoot:
            ContextName = TEXT("OnFoot");
            break;
        case EPedAnimationRegion::Crouch:
            ContextName = TEXT("Crouch");
            break;
        case EPedAnimationRegion::Jump:
            ContextName = TEXT("Jump");
            break;
        case EPedAnimationRegion::Combat:
            ContextName = TEXT("Combat");
            break;
        case EPedAnimationRegion::Cover:
            ContextName = TEXT("Cover");
            break;
        case EPedAnimationRegion::InVehicle:
            ContextName = TEXT("InVehicle");
            break;
        case EPedAnimationRegion::Interaction:
            ContextName = TEXT("Interaction");
            break;
        case EPedAnimationRegion::Emote:
            ContextName = TEXT("Interaction"); // Emotes use interaction context
            break;
        default:
            ContextName = TEXT("OnFoot"); // Fallback
            break;
    }

    // Get animation entry from dictionary
    FAnimationEntry AnimEntry = AnimationDictionary->GetAnimationEntry(ContextName, AnimationName);
    if (AnimEntry.AnimationName.IsEmpty())
    {
        UE_LOG(LogTemp, Warning, TEXT("PedAnimationManager: Animation '%s' not found in region '%s' (context: %s)"), 
               *AnimationName, *UEnum::GetValueAsString(CurrentRegion), *ContextName);
        return;
    }

    // Get full path
    FString AnimationPath = AnimationDictionary->GetAnimationPath(ContextName, AnimationName);
    if (AnimationPath.IsEmpty())
    {
        UE_LOG(LogTemp, Error, TEXT("PedAnimationManager: Failed to resolve path for animation '%s' in region '%s'"), 
               *AnimationName, *UEnum::GetValueAsString(CurrentRegion));
        return;
    }

    // Load animation asset
    UAnimSequence* AnimAsset = LoadObject<UAnimSequence>(nullptr, *AnimationPath);
    if (!AnimAsset)
    {
        UE_LOG(LogTemp, Error, TEXT("PedAnimationManager: Failed to load animation asset from '%s'"), *AnimationPath);
        return;
    }

    // Create animation data
    FAnimationData AnimData;
    AnimData.AnimSequence = AnimAsset;
    AnimData.BlendInTime = AnimEntry.DefaultBlendTime;
    AnimData.BlendOutTime = AnimEntry.DefaultBlendTime;
    AnimData.PlayRate = AnimEntry.DefaultPlayRate;
    AnimData.bLooping = AnimEntry.bIsLooped;
    AnimData.bCanBeCancelled = true;

    // Play animation
    AnimationController->PlayAnimation(AnimData, bForcePlay);

    UE_LOG(LogTemp, Log, TEXT("PedAnimationManager: Playing animation '%s' from region '%s' (context: %s, path: %s)"), 
           *AnimationName, *UEnum::GetValueAsString(CurrentRegion), *ContextName, *AnimationPath);
}

void UPedAnimationManager::StopCurrentAnimation()
{
    if (AnimationController)
    {
        AnimationController->CancelCurrentAnimation();
    }
}

void UPedAnimationManager::SetMovementState(EPedMovementState NewState)
{
    if (AnimationController)
    {
        AnimationController->SetMovementState(NewState);
    }
}

void UPedAnimationManager::SetStanceState(EPedStanceState NewState)
{
    if (AnimationController)
    {
        AnimationController->SetStanceState(NewState);
    }
}

EPedMovementState UPedAnimationManager::GetCurrentMovementState() const
{
    if (AnimationController)
    {
        return AnimationController->CurrentMovementState;
    }
    return EPedMovementState::Idle;
}

EPedStanceState UPedAnimationManager::GetCurrentStanceState() const
{
    if (AnimationController)
    {
        return AnimationController->CurrentStanceState;
    }
    return EPedStanceState::Standing;
}

float UPedAnimationManager::GetCurrentSpeed() const
{
    return bEnableSmoothing ? SmoothedSpeed : LastSpeed;
}

FString UPedAnimationManager::GetCurrentAnimationName() const
{
    if (AnimationController)
    {
        return AnimationController->CurrentAnimationName;
    }
    return TEXT("None");
}

TArray<FString> UPedAnimationManager::GetAvailableAnimations(const FString& CategoryName)
{
    if (AnimationAssetLoader)
    {
        if (CategoryName.IsEmpty())
        {
            return AnimationAssetLoader->GetAllAnimationNames();
        }
        else
        {
            return AnimationAssetLoader->GetAnimationNamesByCategory(CategoryName);
        }
    }
    
    return TArray<FString>();
}

bool UPedAnimationManager::IsAnimationAvailable(const FString& AnimationName)
{
    if (AnimationAssetLoader)
    {
        return AnimationAssetLoader->IsAnimationLoaded(AnimationName);
    }
    return false;
}

void UPedAnimationManager::InitializeAnimationSystem()
{
    // Create Animation Groups Loader if not assigned
    if (!AnimationGroupsLoader)
    {
        AnimationGroupsLoader = NewObject<UAnimationGroupsLoader>(this);
    }

    // Create Animation Dictionary if not assigned
    if (!AnimationDictionary)
    {
        AnimationDictionary = NewObject<UPedAnimationDictionary>(this);
    }

    // Load animation groups from XML
    LoadAnimationGroupsFromXML();

    // Setup dictionaries from loaded groups
    SetupDictionariesFromXML();

    if (AnimationAssetLoader)
    {
        AnimationAssetLoader->InitializeAnimationMappings();
        UE_LOG(LogTemp, Log, TEXT("PedAnimationManager: Animation asset loader initialized"));
    }
    else
    {
        UE_LOG(LogTemp, Warning, TEXT("PedAnimationManager: No AnimationAssetLoader assigned"));
    }

    UE_LOG(LogTemp, Log, TEXT("PedAnimationManager: Animation system fully initialized"));
}

// Dictionary Management Functions
bool UPedAnimationManager::SwapAnimationDictionary(const FString& ContextName, const FString& NewDictionaryName)
{
    if (!AnimationDictionary)
    {
        UE_LOG(LogTemp, Error, TEXT("PedAnimationManager: No AnimationDictionary assigned"));
        return false;
    }

    // Use SAFE dictionary swapping with validation and fallback
    bool Success = AnimationDictionary->SafeSwapDictionary(ContextName, NewDictionaryName, true);
    
    if (Success)
    {
        UE_LOG(LogTemp, Log, TEXT("PedAnimationManager: Successfully swapped '%s' context to dictionary '%s'"), 
               *ContextName, *NewDictionaryName);
               
        // Get validation report for logging
        FString ValidationReport;
        if (AnimationDictionary->ValidateDictionaryForContext(ContextName, NewDictionaryName, ValidationReport))
        {
            float ValidationScore = AnimationDictionary->GetDictionaryValidationScore(NewDictionaryName);
            UE_LOG(LogTemp, Log, TEXT("Dictionary validation score: %.2f"), ValidationScore);
        }
    }
    else
    {
        TArray<FString> MissingClips = AnimationDictionary->GetMissingRequiredClips(NewDictionaryName);
        UE_LOG(LogTemp, Warning, TEXT("PedAnimationManager: Failed to swap to '%s' - Missing %d required clips"), 
               *NewDictionaryName, MissingClips.Num());
    }

    return Success;
}

FString UPedAnimationManager::GetCurrentDictionary(const FString& ContextName)
{
    if (!AnimationDictionary)
    {
        return TEXT("");
    }

    return AnimationDictionary->GetCurrentDictionaryForContext(ContextName);
}

TArray<FString> UPedAnimationManager::GetAvailableDictionaries(const FString& ContextName)
{
    if (!AnimationDictionary)
    {
        return TArray<FString>();
    }

    return AnimationDictionary->GetAvailableDictionariesForContext(ContextName);
}

void UPedAnimationManager::SwapMovementDictionary(const FString& NewDictionaryName)
{
    SwapAnimationDictionary(TEXT("Movement"), NewDictionaryName);
}

void UPedAnimationManager::SwapCrouchDictionary(const FString& NewDictionaryName)
{
    SwapAnimationDictionary(TEXT("Crouch"), NewDictionaryName);
}

void UPedAnimationManager::SwapJumpDictionary(const FString& NewDictionaryName)
{
    SwapAnimationDictionary(TEXT("Jump"), NewDictionaryName);
}

void UPedAnimationManager::SwapCombatDictionary(const FString& NewDictionaryName)
{
    SwapAnimationDictionary(TEXT("Combat"), NewDictionaryName);
}

void UPedAnimationManager::SwapVehicleDictionary(const FString& NewDictionaryName)
{
    SwapAnimationDictionary(TEXT("Vehicle"), NewDictionaryName);
}

// Animation Groups Functions
bool UPedAnimationManager::LoadAnimationGroupsFromXML()
{
    if (!AnimationGroupsLoader)
    {
        UE_LOG(LogTemp, Error, TEXT("PedAnimationManager: AnimationGroupsLoader is null"));
        return false;
    }

    FString FullPath = FPaths::ProjectContentDir() + AnimationGroupsXMLPath;
    bool Success = AnimationGroupsLoader->LoadAnimationGroupsFromXML(FullPath);
    
    if (Success)
    {
        UE_LOG(LogTemp, Log, TEXT("PedAnimationManager: Successfully loaded animation groups from %s"), *FullPath);
    }
    else
    {
        UE_LOG(LogTemp, Error, TEXT("PedAnimationManager: Failed to load animation groups from %s"), *FullPath);
    }

    return Success;
}

TArray<FString> UPedAnimationManager::GetAvailableAnimationGroups(const FString& EntityType)
{
    if (!AnimationGroupsLoader)
    {
        return TArray<FString>();
    }

    EAnimationEntityType EnumEntityType = AnimationGroupsLoader->StringToEntityType(EntityType);
    return AnimationGroupsLoader->GetDictionaryNamesByEntityType(EnumEntityType);
}

void UPedAnimationManager::SetupDictionariesFromXML()
{
    if (!AnimationGroupsLoader || !AnimationDictionary)
    {
        UE_LOG(LogTemp, Error, TEXT("PedAnimationManager: Missing AnimationGroupsLoader or AnimationDictionary"));
        return;
    }

    // Setup dictionaries from loaded groups
    AnimationGroupsLoader->SetupDictionariesFromGroups(AnimationDictionary);
    
    // Setup contexts from loaded groups
    AnimationGroupsLoader->SetupContextsFromGroups(AnimationDictionary);

    UE_LOG(LogTemp, Log, TEXT("PedAnimationManager: Animation dictionaries and contexts setup from XML"));
}

// Vehicle Animation Implementation
void UPedAnimationManager::UpdateVehicleAnimationInputs(float SteeringInput, float ThrottleInput, float BrakeInput, int32 CurrentGear, float RPM, float Speed)
{
    if (AnimationController && AnimationController->CurrentAnimationRegion == EPedAnimationRegion::InVehicle)
    {
        // Update vehicle input variables in the animation controller
        AnimationController->VehicleSteeringInput = SteeringInput;
        AnimationController->VehicleThrottleInput = ThrottleInput;
        AnimationController->VehicleBrakeInput = BrakeInput;
        AnimationController->VehicleCurrentGear = CurrentGear;
        AnimationController->VehicleRPM = RPM;
        AnimationController->VehicleSpeed = Speed;
        
        // Trigger appropriate vehicle animations based on inputs
        if (FMath::Abs(SteeringInput) > 0.1f)
        {
            FString SteeringAnim = SteeringInput > 0 ? TEXT("steer_right") : TEXT("steer_left");
            PlayAnimationInRegion(EPedAnimationRegion::InVehicle, SteeringAnim, 0.1f, true);
        }
        
        if (ThrottleInput > 0.1f)
        {
            PlayAnimationInRegion(EPedAnimationRegion::InVehicle, TEXT("accelerate"), 0.1f, true);
        }
        else if (BrakeInput > 0.1f)
        {
            PlayAnimationInRegion(EPedAnimationRegion::InVehicle, TEXT("brake"), 0.1f, true);
        }
        else
        {
            PlayAnimationInRegion(EPedAnimationRegion::InVehicle, TEXT("idle"), 0.2f, true);
        }
    }
}

// Combat Animation System Implementation
bool UPedAnimationManager::PlayCombatAnimation(const FString& AnimationName, EPedWeaponType WeaponType, float BlendTime, bool bLoop)
{
    if (!AnimationController || !AnimationDictionary)
    {
        UE_LOG(LogTemp, Error, TEXT("PedAnimationManager: AnimationController or AnimationDictionary is null"));
        return false;
    }

    // Get weapon-specific animation name
    FString WeaponSpecificAnimName = GetWeaponSpecificAnimationName(AnimationName, WeaponType);
    
    // First try weapon-specific animation
    FString AnimationPath = AnimationDictionary->GetAnimationPath(TEXT("Combat"), WeaponSpecificAnimName);
    
    // If weapon-specific animation doesn't exist, fall back to base combat animation
    if (AnimationPath.IsEmpty())
    {
        AnimationPath = AnimationDictionary->GetAnimationPath(TEXT("Combat"), AnimationName);
    }
    
    if (AnimationPath.IsEmpty())
    {
        UE_LOG(LogTemp, Warning, TEXT("PedAnimationManager: Combat animation '%s' not found for weapon type '%s'"), 
               *AnimationName, *UEnum::GetValueAsString(WeaponType));
        return false;
    }

    // Load and play the animation
    UAnimSequence* AnimSequence = AnimationAssetLoader->LoadAnimationAsset(AnimationPath);
    if (AnimSequence)
    {
        // Update combat state
        AnimationController->CurrentWeaponType = WeaponType;
        AnimationController->bIsInCombat = true;
        
        // Play the animation
        return PlayAnimationInRegion(EPedAnimationRegion::Combat, WeaponSpecificAnimName.IsEmpty() ? AnimationName : WeaponSpecificAnimName, BlendTime, bLoop);
    }

    return false;
}

bool UPedAnimationManager::SwitchWeaponAnimationSet(EPedWeaponType NewWeaponType)
{
    if (!AnimationDictionary)
    {
        return false;
    }

    // Get weapon-specific dictionary name
    FString WeaponDictionary = GetWeaponDictionaryName(NewWeaponType);
    
    // Use SAFE dictionary swapping with validation
    bool bSuccess = AnimationDictionary->SafeSwapDictionary(TEXT("Combat"), WeaponDictionary, true);
    
    if (bSuccess && AnimationController)
    {
        AnimationController->CurrentWeaponType = NewWeaponType;
        UE_LOG(LogTemp, Log, TEXT("PedAnimationManager: Safely switched to weapon animation set: %s"), *WeaponDictionary);
    }
    else
    {
        UE_LOG(LogTemp, Warning, TEXT("PedAnimationManager: Failed to switch to weapon animation set: %s (using fallback)"), *WeaponDictionary);
    }
    
    return bSuccess;
}

FString UPedAnimationManager::GetWeaponSpecificAnimationName(const FString& BaseAnimationName, EPedWeaponType WeaponType)
{
    if (WeaponType == EPedWeaponType::None)
    {
        return BaseAnimationName;
    }

    // Get weapon prefix
    FString WeaponPrefix = GetWeaponPrefix(WeaponType);
    
    // Return weapon-specific animation name: "pistol_aim", "rifle_reload", etc.
    return FString::Printf(TEXT("%s_%s"), *WeaponPrefix, *BaseAnimationName);
}

void UPedAnimationManager::SetCombatState(EPedCombatState NewCombatState)
{
    if (AnimationController)
    {
        AnimationController->CurrentCombatState = NewCombatState;
        
        // Trigger appropriate animations based on combat state
        switch (NewCombatState)
        {
            case EPedCombatState::Alert:
                PlayCombatAnimation(TEXT("alert"), AnimationController->CurrentWeaponType);
                break;
            case EPedCombatState::Aiming:
                AnimationController->bIsAiming = true;
                PlayCombatAnimation(TEXT("aim"), AnimationController->CurrentWeaponType);
                break;
            case EPedCombatState::Firing:
                PlayCombatAnimation(TEXT("fire"), AnimationController->CurrentWeaponType);
                break;
            case EPedCombatState::Reloading:
                PlayCombatAnimation(TEXT("reload"), AnimationController->CurrentWeaponType);
                break;
            case EPedCombatState::Relaxed:
                AnimationController->bIsAiming = false;
                PlayCombatAnimation(TEXT("idle"), AnimationController->CurrentWeaponType);
                break;
        }
    }
}

void UPedAnimationManager::SetWeaponType(EPedWeaponType NewWeaponType)
{
    if (AnimationController && AnimationController->CurrentWeaponType != NewWeaponType)
    {
        // Switch animation set first
        SwitchWeaponAnimationSet(NewWeaponType);
        
        // Update weapon type
        AnimationController->CurrentWeaponType = NewWeaponType;
        
        // If in combat, update current animation
        if (AnimationController->bIsInCombat)
        {
            SetCombatState(AnimationController->CurrentCombatState);
        }
    }
}

void UPedAnimationManager::EnterCombatMode(EPedWeaponType WeaponType)
{
    if (AnimationController)
    {
        // Set weapon type and switch animation set
        SetWeaponType(WeaponType);
        
        // Enter combat region
        SetAnimationRegion(EPedAnimationRegion::Combat);
        
        // Set combat state
        AnimationController->bIsInCombat = true;
        SetCombatState(EPedCombatState::Alert);
        
        UE_LOG(LogTemp, Log, TEXT("PedAnimationManager: Entered combat mode with weapon: %s"), 
               *UEnum::GetValueAsString(WeaponType));
    }
}

void UPedAnimationManager::ExitCombatMode()
{
    if (AnimationController)
    {
        // Clear combat state
        AnimationController->bIsInCombat = false;
        AnimationController->bIsAiming = false;
        AnimationController->CurrentCombatState = EPedCombatState::Relaxed;
        
        // Return to appropriate movement region
        SetAnimationRegion(EPedAnimationRegion::OnFoot);
        
        UE_LOG(LogTemp, Log, TEXT("PedAnimationManager: Exited combat mode"));
    }
}

// Helper function for weapon prefixes
FString UPedAnimationManager::GetWeaponPrefix(EPedWeaponType WeaponType)
{
    switch (WeaponType)
    {
        case EPedWeaponType::Pistol:    return TEXT("pistol");
        case EPedWeaponType::Rifle:     return TEXT("rifle");
        case EPedWeaponType::Shotgun:   return TEXT("shotgun");
        case EPedWeaponType::SMG:       return TEXT("smg");
        case EPedWeaponType::Sniper:    return TEXT("sniper");
        case EPedWeaponType::Melee:     return TEXT("melee");
        case EPedWeaponType::Thrown:    return TEXT("thrown");
        case EPedWeaponType::Heavy:     return TEXT("heavy");
        default:                        return TEXT("");
    }
}

// Helper function for weapon dictionary names
FString UPedAnimationManager::GetWeaponDictionaryName(EPedWeaponType WeaponType)
{
    switch (WeaponType)
    {
        case EPedWeaponType::Pistol:    return TEXT("Combat_Pistol");
        case EPedWeaponType::Rifle:     return TEXT("Combat_Rifle");
        case EPedWeaponType::Shotgun:   return TEXT("Combat_Shotgun");
        case EPedWeaponType::SMG:       return TEXT("Combat_SMG");
        case EPedWeaponType::Sniper:    return TEXT("Combat_Sniper");
        case EPedWeaponType::Melee:     return TEXT("Combat_Melee");
        case EPedWeaponType::Thrown:    return TEXT("Combat_Thrown");
        case EPedWeaponType::Heavy:     return TEXT("Combat_Heavy");
        default:                        return TEXT("Combat");
    }
}

// ============ DICTIONARY VALIDATION FUNCTIONS ============

bool UPedAnimationManager::ValidateCurrentDictionaries()
{
    if (!AnimationDictionary)
    {
        UE_LOG(LogTemp, Error, TEXT("PedAnimationManager: No AnimationDictionary assigned"));
        return false;
    }

    TArray<FString> ContextNames = AnimationDictionary->GetAllContextNames();
    bool bAllValid = true;

    for (const FString& ContextName : ContextNames)
    {
        FString CurrentDict = AnimationDictionary->GetCurrentDictionaryForContext(ContextName);
        if (!CurrentDict.IsEmpty())
        {
            FString ValidationReport;
            bool bIsValid = AnimationDictionary->ValidateDictionaryForContext(ContextName, CurrentDict, ValidationReport);
            
            if (!bIsValid)
            {
                UE_LOG(LogTemp, Warning, TEXT("Context '%s' dictionary '%s' failed validation: %s"), 
                       *ContextName, *CurrentDict, *ValidationReport);
                bAllValid = false;
            }
            else
            {
                float ValidationScore = AnimationDictionary->GetDictionaryValidationScore(CurrentDict);
                UE_LOG(LogTemp, Log, TEXT("Context '%s' dictionary '%s' validated with score %.2f"), 
                       *ContextName, *CurrentDict, ValidationScore);
            }
        }
    }

    return bAllValid;
}

FString UPedAnimationManager::GetDictionaryValidationReport(const FString& DictionaryName)
{
    if (!AnimationDictionary)
    {
        return TEXT("No AnimationDictionary assigned");
    }

    FString ValidationReport;
    AnimationDictionary->ValidateDictionary(DictionaryName, ValidationReport);
    return ValidationReport;
}

TArray<FString> UPedAnimationManager::GetMissingClipsForDictionary(const FString& DictionaryName)
{
    if (!AnimationDictionary)
    {
        return TArray<FString>();
    }

    return AnimationDictionary->GetMissingRequiredClips(DictionaryName);
}

bool UPedAnimationManager::CanSafelyUseDictionary(const FString& ContextName, const FString& DictionaryName)
{
    if (!AnimationDictionary)
    {
        return false;
    }

    return AnimationDictionary->CanSafelySwapDictionary(ContextName, DictionaryName);
}

void UPedAnimationManager::SetupValidationRules()
{
    if (AnimationDictionary)
    {
        AnimationDictionary->InitializeBuiltInValidationRules();
        UE_LOG(LogTemp, Log, TEXT("PedAnimationManager: Built-in validation rules setup complete"));
    }
}

void UPedAnimationManager::SwapInVehicleDictionary(const FString& NewDictionaryName)
{
    SwapAnimationDictionary(TEXT("InVehicle"), NewDictionaryName);
}

void UPedAnimationManager::SwapCoverDictionary(const FString& NewDictionaryName)
{
    SwapAnimationDictionary(TEXT("Cover"), NewDictionaryName);
}

void UPedAnimationManager::SwapInteractionDictionary(const FString& NewDictionaryName)
{
    SwapAnimationDictionary(TEXT("Interaction"), NewDictionaryName);
}

void UPedAnimationManager::SetAnimationRegion(EPedAnimationRegion NewRegion)
{
    if (AnimationController)
    {
        AnimationController->SetAnimationRegion(NewRegion);
    }
}

EPedAnimationRegion UPedAnimationManager::GetCurrentAnimationRegion() const
{
    if (AnimationController)
    {
        return AnimationController->CurrentAnimationRegion;
    }
    return EPedAnimationRegion::OnFoot;
}

bool UPedAnimationManager::PlayAnimationInRegion(EPedAnimationRegion Region, const FString& AnimationName, float BlendTime, bool bLoop)
{
    if (!AnimationController || !AnimationDictionary)
    {
        return false;
    }
    
    // Get context name for the region
    FString ContextName = AnimationController->GetRegionContextName(Region);
    
    // Try to get animation from dictionary
    FString ResolvedAnimationPath = AnimationDictionary->GetAnimationPath(ContextName, AnimationName);
    
    if (ResolvedAnimationPath.IsEmpty())
    {
        UE_LOG(LogTemp, Warning, TEXT("PedAnimationManager: Animation '%s' not found in region '%s'"), 
               *AnimationName, *ContextName);
        return false;
    }
    
    // Load and play the animation
    if (AnimationAssetLoader)
    {
        UAnimSequence* AnimSequence = AnimationAssetLoader->LoadAnimationAsset(ResolvedAnimationPath);
        if (AnimSequence)
        {
            // Create animation data and play it
            FAnimationData AnimData;
            AnimData.AnimSequence = AnimSequence;
            AnimData.BlendInTime = BlendTime;
            AnimData.BlendOutTime = BlendTime;
            AnimData.bLooping = bLoop;
            AnimData.PlayRate = 1.0f;
            
            AnimationController->PlayAnimation(AnimData);
            
            UE_LOG(LogTemp, Log, TEXT("PedAnimationManager: Playing animation '%s' in region '%s'"), 
                   *AnimationName, *ContextName);
            return true;
        }
    }
    
    return false;
}
