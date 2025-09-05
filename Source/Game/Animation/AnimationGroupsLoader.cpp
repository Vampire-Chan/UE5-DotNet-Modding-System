#include "AnimationGroupsLoader.h"
#include "Engine/Engine.h"
#include "HAL/PlatformFilemanager.h"
#include "Misc/FileHelper.h"

UAnimationGroupsLoader::UAnimationGroupsLoader()
{
    // Constructor
}

bool UAnimationGroupsLoader::LoadAnimationGroupsFromXML(const FString& XMLFilePath)
{
    FString FilePath = XMLFilePath;
    if (FilePath.IsEmpty())
    {
        // Default path to AnimationGroups.xml in the project root Data folder
        FilePath = FPaths::ProjectDir() + TEXT("Data/Animations/AnimationGroups.xml");
    }

    // Read the XML file
    FString XMLContent;
    if (!FFileHelper::LoadFileToString(XMLContent, *FilePath))
    {
        UE_LOG(LogTemp, Error, TEXT("AnimationGroupsLoader: Failed to load XML file from %s"), *FilePath);
        return false;
    }

    UE_LOG(LogTemp, Log, TEXT("AnimationGroupsLoader: Successfully loaded XML file from %s"), *FilePath);

    // Parse the XML content
    return ParseXMLContent(XMLContent);
}

bool UAnimationGroupsLoader::ParseXMLContent(const FString& XMLContent)
{
    LoadedDictionaries.Empty();

    // Find all AnimationDictionary elements
    TArray<FString> DictionaryElements = SplitXMLElements(XMLContent, TEXT("AnimationDictionary"));

    for (const FString& Element : DictionaryElements)
    {
        FAnimationDictionaryDefinition Definition = ParseDictionaryElement(Element);
        if (!Definition.Name.IsEmpty())
        {
            LoadedDictionaries.Add(Definition);
            UE_LOG(LogTemp, Log, TEXT("AnimationGroupsLoader: Loaded dictionary '%s' -> '%s' (%s)"), 
                   *Definition.Name, *Definition.Path, *EntityTypeToString(Definition.EntityType));
        }
    }

    UE_LOG(LogTemp, Log, TEXT("AnimationGroupsLoader: Loaded %d animation dictionaries"), LoadedDictionaries.Num());
    return LoadedDictionaries.Num() > 0;
}

FAnimationDictionaryDefinition UAnimationGroupsLoader::ParseDictionaryElement(const FString& ElementContent)
{
    FAnimationDictionaryDefinition Definition;

    // Extract attributes
    Definition.Name = ExtractAttributeValue(ElementContent, TEXT("Name"));
    Definition.Path = ExtractAttributeValue(ElementContent, TEXT("Path"));
    FString EntityTypeStr = ExtractAttributeValue(ElementContent, TEXT("EntityType"));
    Definition.EntityType = StringToEntityType(EntityTypeStr);

    // Generate description
    Definition.Description = FString::Printf(TEXT("%s animations for %s"), 
                                           *Definition.Name, *EntityTypeToString(Definition.EntityType));

    return Definition;
}

FString UAnimationGroupsLoader::ExtractAttributeValue(const FString& ElementContent, const FString& AttributeName)
{
    FString SearchPattern = AttributeName + TEXT("=\"");
    int32 StartIndex = ElementContent.Find(SearchPattern);
    if (StartIndex == INDEX_NONE)
    {
        return TEXT("");
    }

    StartIndex += SearchPattern.Len();
    int32 EndIndex = ElementContent.Find(TEXT("\""), ESearchCase::CaseSensitive, ESearchDir::FromStart, StartIndex);
    if (EndIndex == INDEX_NONE)
    {
        return TEXT("");
    }

    return ElementContent.Mid(StartIndex, EndIndex - StartIndex);
}

TArray<FString> UAnimationGroupsLoader::SplitXMLElements(const FString& XMLContent, const FString& ElementName)
{
    TArray<FString> Elements;
    FString StartTag = TEXT("<") + ElementName;
    FString EndTag = TEXT("/>");

    int32 SearchStart = 0;
    while (true)
    {
        int32 StartIndex = XMLContent.Find(StartTag, ESearchCase::CaseSensitive, ESearchDir::FromStart, SearchStart);
        if (StartIndex == INDEX_NONE)
        {
            break;
        }

        int32 EndIndex = XMLContent.Find(EndTag, ESearchCase::CaseSensitive, ESearchDir::FromStart, StartIndex);
        if (EndIndex == INDEX_NONE)
        {
            break;
        }

        EndIndex += EndTag.Len();
        FString Element = XMLContent.Mid(StartIndex, EndIndex - StartIndex);
        Elements.Add(Element);

        SearchStart = EndIndex;
    }

    return Elements;
}

void UAnimationGroupsLoader::SetupDictionariesFromGroups(UPedAnimationDictionary* TargetDictionary)
{
    if (!TargetDictionary)
    {
        UE_LOG(LogTemp, Error, TEXT("AnimationGroupsLoader: TargetDictionary is null"));
        return;
    }

    UE_LOG(LogTemp, Log, TEXT("AnimationGroupsLoader: Setting up dictionaries from loaded groups..."));

    for (const FAnimationDictionaryDefinition& Definition : LoadedDictionaries)
    {
        // Create animation dictionary
        FAnimationDictionary NewDictionary;
        NewDictionary.DictionaryName = Definition.Name;
        NewDictionary.BaseFolderPath = ConvertPathToUAssetPath(Definition.Path);
        NewDictionary.Description = Definition.Description;

        // Create default animation entries based on known patterns
        CreateDefaultAnimationEntries(NewDictionary, Definition.Name);

        // Add to target dictionary
        TargetDictionary->AddDictionary(NewDictionary);

        UE_LOG(LogTemp, Log, TEXT("AnimationGroupsLoader: Created dictionary '%s' with %d animations"), 
               *NewDictionary.DictionaryName, NewDictionary.Animations.Num());
    }
}

void UAnimationGroupsLoader::SetupContextsFromGroups(UPedAnimationDictionary* TargetDictionary)
{
    if (!TargetDictionary)
    {
        UE_LOG(LogTemp, Error, TEXT("AnimationGroupsLoader: TargetDictionary is null"));
        return;
    }

    UE_LOG(LogTemp, Log, TEXT("AnimationGroupsLoader: Setting up contexts from loaded groups..."));

    // Get PED dictionaries
    TArray<FAnimationDictionaryDefinition> PedDictionaries = GetDictionariesByEntityType(EAnimationEntityType::PED);

    // Create OnFoot context (replaces Movement)
    FAnimationContext OnFootContext;
    OnFootContext.ContextName = TEXT("OnFoot");
    OnFootContext.CurrentDictionaryName = TEXT("Move_Player");
    for (const FAnimationDictionaryDefinition& Dict : PedDictionaries)
    {
        if (Dict.Name.Contains(TEXT("Move_")) && !Dict.Name.Contains(TEXT("Combat")) && !Dict.Name.Contains(TEXT("Crouch")))
        {
            OnFootContext.AvailableDictionaries.AddUnique(Dict.Name);
        }
    }
    TargetDictionary->AddContext(OnFootContext);

    // Create Crouch context
    FAnimationContext CrouchContext;
    CrouchContext.ContextName = TEXT("Crouch");
    CrouchContext.CurrentDictionaryName = TEXT("Crouch_Standard");
    for (const FAnimationDictionaryDefinition& Dict : PedDictionaries)
    {
        if (Dict.Name.Contains(TEXT("Crouch")) || Dict.Name.Contains(TEXT("Stealth")))
        {
            CrouchContext.AvailableDictionaries.AddUnique(Dict.Name);
        }
    }
    TargetDictionary->AddContext(CrouchContext);

    // Create Jump context
    FAnimationContext JumpContext;
    JumpContext.ContextName = TEXT("Jump");
    JumpContext.CurrentDictionaryName = TEXT("Jump_Standard");
    for (const FAnimationDictionaryDefinition& Dict : PedDictionaries)
    {
        if (Dict.Name.Contains(TEXT("Jump")) || Dict.Name.Contains(TEXT("Climb")))
        {
            JumpContext.AvailableDictionaries.AddUnique(Dict.Name);
        }
    }
    TargetDictionary->AddContext(JumpContext);

    // Create Combat context
    FAnimationContext CombatContext;
    CombatContext.ContextName = TEXT("Combat");
    CombatContext.CurrentDictionaryName = TEXT("Move_Combat_Strafe");
    for (const FAnimationDictionaryDefinition& Dict : PedDictionaries)
    {
        if (Dict.Name.Contains(TEXT("Combat")) || Dict.Name.Contains(TEXT("Weapon")))
        {
            CombatContext.AvailableDictionaries.AddUnique(Dict.Name);
        }
    }
    TargetDictionary->AddContext(CombatContext);

    // Create Cover context
    FAnimationContext CoverContext;
    CoverContext.ContextName = TEXT("Cover");
    CoverContext.CurrentDictionaryName = TEXT("Ped_Combat_Cover"); // Assuming this exists
    for (const FAnimationDictionaryDefinition& Dict : PedDictionaries)
    {
        if (Dict.Name.Contains(TEXT("Cover")) || Dict.Name.Contains(TEXT("Peek")))
        {
            CoverContext.AvailableDictionaries.AddUnique(Dict.Name);
        }
    }
    TargetDictionary->AddContext(CoverContext);

    // Create InVehicle context
    FAnimationContext InVehicleContext;
    InVehicleContext.ContextName = TEXT("InVehicle");
    InVehicleContext.CurrentDictionaryName = TEXT("Vehicle_Standard");
    TArray<FAnimationDictionaryDefinition> VehicleDictionaries = GetDictionariesByEntityType(EAnimationEntityType::VEHICLE);
    for (const FAnimationDictionaryDefinition& Dict : VehicleDictionaries)
    {
        InVehicleContext.AvailableDictionaries.AddUnique(Dict.Name);
    }
    // Also add ped vehicle animations
    for (const FAnimationDictionaryDefinition& Dict : PedDictionaries)
    {
        if (Dict.Name.Contains(TEXT("Vehicle")) || Dict.Name.Contains(TEXT("Driving")))
        {
            InVehicleContext.AvailableDictionaries.AddUnique(Dict.Name);
        }
    }
    TargetDictionary->AddContext(InVehicleContext);

    // Create Interaction context
    FAnimationContext InteractionContext;
    InteractionContext.ContextName = TEXT("Interaction");
    InteractionContext.CurrentDictionaryName = TEXT("Ped_Interactions");
    for (const FAnimationDictionaryDefinition& Dict : PedDictionaries)
    {
        if (Dict.Name.Contains(TEXT("Interaction")) || Dict.Name.Contains(TEXT("Emote")))
        {
            InteractionContext.AvailableDictionaries.AddUnique(Dict.Name);
        }
    }
    TargetDictionary->AddContext(InteractionContext);

    UE_LOG(LogTemp, Log, TEXT("AnimationGroupsLoader: Created 7 animation contexts (OnFoot, Crouch, Jump, Combat, Cover, InVehicle, Interaction)"));
}

void UAnimationGroupsLoader::CreateDefaultAnimationEntries(FAnimationDictionary& Dictionary, const FString& DictionaryName)
{
    // Create entries based on dictionary name
    if (DictionaryName == TEXT("Move_Player"))
    {
        CreatePlayerMovementEntries(Dictionary);
    }
    else if (DictionaryName == TEXT("Crouch_Standard"))
    {
        CreateCrouchMovementEntries(Dictionary);
    }
    else if (DictionaryName == TEXT("Jump_Standard"))
    {
        CreateJumpStandardEntries(Dictionary);
    }
    else if (DictionaryName.Contains(TEXT("Combat_Strafe")))
    {
        CreateCombatStrafeEntries(Dictionary);
    }
    else if (DictionaryName == TEXT("Vehicle_Standard"))
    {
        CreateVehicleStandardEntries(Dictionary);
    }
    else
    {
        // For unknown dictionaries, create basic entries
        Dictionary.Animations.Add(CreateAnimEntry(TEXT("idle"), TEXT("idle.onim"), true));
        Dictionary.Animations.Add(CreateAnimEntry(TEXT("action"), TEXT("action.onim"), false));
    }
}

void UAnimationGroupsLoader::CreatePlayerMovementEntries(FAnimationDictionary& Dictionary)
{
    // Basic movements (looped)
    Dictionary.Animations.Add(CreateAnimEntry(TEXT("idle"), TEXT("idle.onim"), true));
    Dictionary.Animations.Add(CreateAnimEntry(TEXT("walk"), TEXT("walk.onim"), true));
    Dictionary.Animations.Add(CreateAnimEntry(TEXT("walk_backward"), TEXT("walk_b.onim"), true));
    Dictionary.Animations.Add(CreateAnimEntry(TEXT("walk_casual"), TEXT("walk_c.onim"), true));
    Dictionary.Animations.Add(CreateAnimEntry(TEXT("walk_up"), TEXT("walk_up.onim"), true));
    Dictionary.Animations.Add(CreateAnimEntry(TEXT("walk_down"), TEXT("walk_down.onim"), true));
    Dictionary.Animations.Add(CreateAnimEntry(TEXT("walk_strafe_left"), TEXT("walk_strafe_l.onim"), true));
    Dictionary.Animations.Add(CreateAnimEntry(TEXT("walk_strafe_right"), TEXT("walk_strafe_r.onim"), true));
    Dictionary.Animations.Add(CreateAnimEntry(TEXT("walk_strafe_backward"), TEXT("walk_strafe_b.onim"), true));
    Dictionary.Animations.Add(CreateAnimEntry(TEXT("run"), TEXT("run.onim"), true));
    Dictionary.Animations.Add(CreateAnimEntry(TEXT("run_up"), TEXT("run_up.onim"), true));
    Dictionary.Animations.Add(CreateAnimEntry(TEXT("run_down"), TEXT("run_down.onim"), true));
    Dictionary.Animations.Add(CreateAnimEntry(TEXT("run_strafe_left"), TEXT("run_strafe_l.onim"), true));
    Dictionary.Animations.Add(CreateAnimEntry(TEXT("run_strafe_right"), TEXT("run_strafe_r.onim"), true));
    Dictionary.Animations.Add(CreateAnimEntry(TEXT("run_strafe_backward"), TEXT("run_strafe_b.onim"), true));
    Dictionary.Animations.Add(CreateAnimEntry(TEXT("sprint"), TEXT("sprint.onim"), true));

    // Start animations (non-looped)
    Dictionary.Animations.Add(CreateAnimEntry(TEXT("walk_start"), TEXT("wstart.onim"), false));
    Dictionary.Animations.Add(CreateAnimEntry(TEXT("walk_start_turn_left_90"), TEXT("wstart_turn_l90.onim"), false));
    Dictionary.Animations.Add(CreateAnimEntry(TEXT("walk_start_turn_right_90"), TEXT("wstart_turn_r90.onim"), false));
    Dictionary.Animations.Add(CreateAnimEntry(TEXT("walk_start_turn_left_180"), TEXT("wstart_turn_l180.onim"), false));
    Dictionary.Animations.Add(CreateAnimEntry(TEXT("walk_start_turn_right_180"), TEXT("wstart_turn_r180.onim"), false));
    Dictionary.Animations.Add(CreateAnimEntry(TEXT("run_start_forward"), TEXT("runstart_fwd.onim"), false));
    Dictionary.Animations.Add(CreateAnimEntry(TEXT("run_start_left_90"), TEXT("runstart_l_90.onim"), false));
    Dictionary.Animations.Add(CreateAnimEntry(TEXT("run_start_right_90"), TEXT("runstart_r_90.onim"), false));
    Dictionary.Animations.Add(CreateAnimEntry(TEXT("run_start_left_180"), TEXT("runstart_l_180.onim"), false));
    Dictionary.Animations.Add(CreateAnimEntry(TEXT("run_start_right_180"), TEXT("runstart_r_180.onim"), false));

    // Stop animations (non-looped)
    Dictionary.Animations.Add(CreateAnimEntry(TEXT("walk_stop_left"), TEXT("wstop_l.onim"), false));
    Dictionary.Animations.Add(CreateAnimEntry(TEXT("walk_stop_right"), TEXT("wstop_r.onim"), false));
    Dictionary.Animations.Add(CreateAnimEntry(TEXT("run_stop_left"), TEXT("rstop_l.onim"), false));
    Dictionary.Animations.Add(CreateAnimEntry(TEXT("run_stop_right"), TEXT("rstop_r.onim"), false));
    Dictionary.Animations.Add(CreateAnimEntry(TEXT("sprint_stop_left"), TEXT("sstop_l.onim"), false));
    Dictionary.Animations.Add(CreateAnimEntry(TEXT("sprint_stop_right"), TEXT("sstop_r.onim"), false));

    // Turn animations (non-looped)
    Dictionary.Animations.Add(CreateAnimEntry(TEXT("walk_turn_left"), TEXT("walk_turn_l.onim"), false));
    Dictionary.Animations.Add(CreateAnimEntry(TEXT("walk_turn_right"), TEXT("walk_turn_r.onim"), false));
    Dictionary.Animations.Add(CreateAnimEntry(TEXT("walk_turn_left_sharp"), TEXT("walk_turn_l2.onim"), false));
    Dictionary.Animations.Add(CreateAnimEntry(TEXT("walk_turn_right_sharp"), TEXT("walk_turn_r2.onim"), false));
    Dictionary.Animations.Add(CreateAnimEntry(TEXT("walk_turn_left_tight"), TEXT("walk_turn_l3.onim"), false));
    Dictionary.Animations.Add(CreateAnimEntry(TEXT("walk_turn_right_tight"), TEXT("walk_turn_r3.onim"), false));
    Dictionary.Animations.Add(CreateAnimEntry(TEXT("walk_turn_180_left"), TEXT("walk_turn_180_l.onim"), false));
    Dictionary.Animations.Add(CreateAnimEntry(TEXT("walk_turn_180_right"), TEXT("walk_turn_180_r.onim"), false));
    Dictionary.Animations.Add(CreateAnimEntry(TEXT("run_turn_left"), TEXT("run_turn_l.onim"), false));
    Dictionary.Animations.Add(CreateAnimEntry(TEXT("run_turn_right"), TEXT("run_turn_r.onim"), false));
    Dictionary.Animations.Add(CreateAnimEntry(TEXT("run_turn_left_sharp"), TEXT("run_turn_l2.onim"), false));
    Dictionary.Animations.Add(CreateAnimEntry(TEXT("run_turn_right_sharp"), TEXT("run_turn_r2.onim"), false));
    Dictionary.Animations.Add(CreateAnimEntry(TEXT("run_turn_180"), TEXT("run_turn_180.onim"), false));
    Dictionary.Animations.Add(CreateAnimEntry(TEXT("run_turn_180_left"), TEXT("run_turn_180_l.onim"), false));
    Dictionary.Animations.Add(CreateAnimEntry(TEXT("run_turn_180_right"), TEXT("run_turn_180_r.onim"), false));
    Dictionary.Animations.Add(CreateAnimEntry(TEXT("sprint_turn_left"), TEXT("sprint_turn_l.onim"), false));
    Dictionary.Animations.Add(CreateAnimEntry(TEXT("sprint_turn_right"), TEXT("sprint_turn_r.onim"), false));
    Dictionary.Animations.Add(CreateAnimEntry(TEXT("sprint_turn_180_left"), TEXT("sprint_turn_180_l.onim"), false));
    Dictionary.Animations.Add(CreateAnimEntry(TEXT("sprint_turn_180_right"), TEXT("sprint_turn_180_r.onim"), false));
    Dictionary.Animations.Add(CreateAnimEntry(TEXT("turn_360_left"), TEXT("turn_360_l.onim"), false));
    Dictionary.Animations.Add(CreateAnimEntry(TEXT("turn_360_right"), TEXT("turn_360_r.onim"), false));
}

void UAnimationGroupsLoader::CreateCrouchMovementEntries(FAnimationDictionary& Dictionary)
{
    Dictionary.Animations.Add(CreateAnimEntry(TEXT("idle"), TEXT("idle.onim"), true));
    Dictionary.Animations.Add(CreateAnimEntry(TEXT("walk"), TEXT("walk.onim"), true));
    Dictionary.Animations.Add(CreateAnimEntry(TEXT("run"), TEXT("run.onim"), true));
    Dictionary.Animations.Add(CreateAnimEntry(TEXT("idle_to_crouch"), TEXT("idle2crouchidle.onim"), false));
    Dictionary.Animations.Add(CreateAnimEntry(TEXT("crouch_to_idle"), TEXT("crouchidle2idle.onim"), false));
    Dictionary.Animations.Add(CreateAnimEntry(TEXT("walk_start"), TEXT("wstart.onim"), false));
    Dictionary.Animations.Add(CreateAnimEntry(TEXT("walk_stop_left"), TEXT("wstop_l.onim"), false));
    Dictionary.Animations.Add(CreateAnimEntry(TEXT("walk_stop_right"), TEXT("wstop_r.onim"), false));
    Dictionary.Animations.Add(CreateAnimEntry(TEXT("run_stop_left"), TEXT("rstop_l.onim"), false));
    Dictionary.Animations.Add(CreateAnimEntry(TEXT("run_stop_right"), TEXT("rstop_r.onim"), false));
    Dictionary.Animations.Add(CreateAnimEntry(TEXT("walk_turn_left"), TEXT("walk_turn_l.onim"), false));
    Dictionary.Animations.Add(CreateAnimEntry(TEXT("walk_turn_right"), TEXT("walk_turn_r.onim"), false));
    Dictionary.Animations.Add(CreateAnimEntry(TEXT("run_turn_left"), TEXT("run_turn_l.onim"), false));
    Dictionary.Animations.Add(CreateAnimEntry(TEXT("run_turn_right"), TEXT("run_turn_r.onim"), false));
    Dictionary.Animations.Add(CreateAnimEntry(TEXT("run_turn_left_sharp"), TEXT("run_turn_l2.onim"), false));
    Dictionary.Animations.Add(CreateAnimEntry(TEXT("run_turn_right_sharp"), TEXT("run_turn_r2.onim"), false));
    Dictionary.Animations.Add(CreateAnimEntry(TEXT("turn_360_left"), TEXT("turn_360_l.onim"), false));
    Dictionary.Animations.Add(CreateAnimEntry(TEXT("turn_360_right"), TEXT("turn_360_r.onim"), false));
}

void UAnimationGroupsLoader::CreateJumpStandardEntries(FAnimationDictionary& Dictionary)
{
    Dictionary.Animations.Add(CreateAnimEntry(TEXT("takeoff_left"), TEXT("jump_takeoff_l.onim"), false));
    Dictionary.Animations.Add(CreateAnimEntry(TEXT("takeoff_right"), TEXT("jump_takeoff_r.onim"), false));
    Dictionary.Animations.Add(CreateAnimEntry(TEXT("on_spot"), TEXT("jump_on_spot.onim"), false));
    Dictionary.Animations.Add(CreateAnimEntry(TEXT("inair_left"), TEXT("jump_inair_l.onim"), true));
    Dictionary.Animations.Add(CreateAnimEntry(TEXT("inair_right"), TEXT("jump_inair_r.onim"), true));
    Dictionary.Animations.Add(CreateAnimEntry(TEXT("land_left"), TEXT("jump_land_l.onim"), false));
    Dictionary.Animations.Add(CreateAnimEntry(TEXT("land_right"), TEXT("jump_land_r.onim"), false));
    Dictionary.Animations.Add(CreateAnimEntry(TEXT("land_roll"), TEXT("jump_land_roll.onim"), false));
    Dictionary.Animations.Add(CreateAnimEntry(TEXT("land_squat"), TEXT("jump_land_squat.onim"), false));
}

void UAnimationGroupsLoader::CreateCombatStrafeEntries(FAnimationDictionary& Dictionary)
{
    // Note: Would need to examine actual files in move_combat_strafe folders
    // For now, create basic combat entries
    Dictionary.Animations.Add(CreateAnimEntry(TEXT("idle"), TEXT("idle.onim"), true));
    Dictionary.Animations.Add(CreateAnimEntry(TEXT("strafe_left"), TEXT("strafe_l.onim"), true));
    Dictionary.Animations.Add(CreateAnimEntry(TEXT("strafe_right"), TEXT("strafe_r.onim"), true));
    Dictionary.Animations.Add(CreateAnimEntry(TEXT("strafe_forward"), TEXT("strafe_fwd.onim"), true));
    Dictionary.Animations.Add(CreateAnimEntry(TEXT("strafe_backward"), TEXT("strafe_back.onim"), true));
}

void UAnimationGroupsLoader::CreateVehicleStandardEntries(FAnimationDictionary& Dictionary)
{
    // Note: Would need to examine actual files in veh@std folder
    // For now, create basic vehicle entries
    Dictionary.Animations.Add(CreateAnimEntry(TEXT("idle"), TEXT("idle.onim"), true));
    Dictionary.Animations.Add(CreateAnimEntry(TEXT("enter"), TEXT("enter.onim"), false));
    Dictionary.Animations.Add(CreateAnimEntry(TEXT("exit"), TEXT("exit.onim"), false));
    Dictionary.Animations.Add(CreateAnimEntry(TEXT("driving"), TEXT("driving.onim"), true));
}

FAnimationEntry UAnimationGroupsLoader::CreateAnimEntry(const FString& AnimName, const FString& FileName, bool bLoop, float PlayRate)
{
    FAnimationEntry Entry;
    Entry.AnimationName = AnimName;
    Entry.FileName = FileName;
    Entry.bIsLooped = bLoop;
    Entry.DefaultPlayRate = PlayRate;
    Entry.DefaultBlendTime = 0.15f;
    return Entry;
}

TArray<FAnimationDictionaryDefinition> UAnimationGroupsLoader::GetDictionariesByEntityType(EAnimationEntityType EntityType)
{
    TArray<FAnimationDictionaryDefinition> FilteredDictionaries;
    for (const FAnimationDictionaryDefinition& Dict : LoadedDictionaries)
    {
        if (Dict.EntityType == EntityType)
        {
            FilteredDictionaries.Add(Dict);
        }
    }
    return FilteredDictionaries;
}

FAnimationDictionaryDefinition UAnimationGroupsLoader::GetDictionaryByName(const FString& DictionaryName)
{
    for (const FAnimationDictionaryDefinition& Dict : LoadedDictionaries)
    {
        if (Dict.Name == DictionaryName)
        {
            return Dict;
        }
    }
    return FAnimationDictionaryDefinition();
}

TArray<FString> UAnimationGroupsLoader::GetAllDictionaryNames()
{
    TArray<FString> Names;
    for (const FAnimationDictionaryDefinition& Dict : LoadedDictionaries)
    {
        Names.Add(Dict.Name);
    }
    return Names;
}

TArray<FString> UAnimationGroupsLoader::GetDictionaryNamesByEntityType(EAnimationEntityType EntityType)
{
    TArray<FString> Names;
    for (const FAnimationDictionaryDefinition& Dict : LoadedDictionaries)
    {
        if (Dict.EntityType == EntityType)
        {
            Names.Add(Dict.Name);
        }
    }
    return Names;
}

FString UAnimationGroupsLoader::ConvertPathToUAssetPath(const FString& GamePath)
{
    // Convert from "Game/Animations/..." to "/Game/Content/Animations/..."
    FString UAssetPath = GamePath;
    if (UAssetPath.StartsWith(TEXT("Game/")))
    {
        UAssetPath = TEXT("/Game/Content/") + UAssetPath.RightChop(5); // Remove "Game/" and add "/Game/Content/"
    }
    return UAssetPath;
}

EAnimationEntityType UAnimationGroupsLoader::StringToEntityType(const FString& EntityTypeString)
{
    if (EntityTypeString == TEXT("PED"))
        return EAnimationEntityType::PED;
    else if (EntityTypeString == TEXT("VEHICLE"))
        return EAnimationEntityType::VEHICLE;
    else if (EntityTypeString == TEXT("OBJECT"))
        return EAnimationEntityType::OBJECT;
    else if (EntityTypeString == TEXT("BUILDING"))
        return EAnimationEntityType::BUILDING;
    else
        return EAnimationEntityType::UNKNOWN;
}

FString UAnimationGroupsLoader::EntityTypeToString(EAnimationEntityType EntityType)
{
    switch (EntityType)
    {
        case EAnimationEntityType::PED:
            return TEXT("PED");
        case EAnimationEntityType::VEHICLE:
            return TEXT("VEHICLE");
        case EAnimationEntityType::OBJECT:
            return TEXT("OBJECT");
        case EAnimationEntityType::BUILDING:
            return TEXT("BUILDING");
        default:
            return TEXT("UNKNOWN");
    }
}
