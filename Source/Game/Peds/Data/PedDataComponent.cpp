#include "PedDataComponent.h"
#include "Engine/DataTable.h"
#include "HAL/PlatformFilemanager.h"
#include "Misc/FileHelper.h"
#include "Engine/World.h"

UPedDataComponent::UPedDataComponent()
{
    PrimaryComponentTick.bCanEverTick = false;

    // Initialize configuration
    bAutoLoadDefaultProfile = true;
    XMLDataPath = TEXT("Data/Peds/");

    // Initialize state
    bIsProfileLoaded = false;

    // Setup default profile
    DefaultProfile.ProfileName = TEXT("Default Ped Profile");
    DefaultProfile.Attributes.PedName = TEXT("Default Ped");
    DefaultProfile.Attributes.PedType = EPedType::Civilian;
    DefaultProfile.AnimationGroup = TEXT("player_movement");
    DefaultProfile.VoiceGroup = TEXT("Default");
    DefaultProfile.bIsAggressive = false;
    DefaultProfile.bIsLawEnforcement = false;
    DefaultProfile.bIsGangMember = false;
}

void UPedDataComponent::BeginPlay()
{
    Super::BeginPlay();

    InitializeComponent();

    if (bAutoLoadDefaultProfile)
    {
        LoadDefaultProfile();
    }

    UE_LOG(LogTemp, Log, TEXT("PedDataComponent: Initialized for %s"), 
           GetOwner() ? *GetOwner()->GetName() : TEXT("Unknown"));
}

void UPedDataComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
    Super::TickComponent(DeltaTime, TickType, ThisTickFunction);
}

void UPedDataComponent::LoadBehaviorProfile(const FString& ProfileName)
{
    if (FPedBehaviorProfile* CachedProfile = LoadedProfiles.Find(ProfileName))
    {
        // Use cached profile
        SetBehaviorProfile(*CachedProfile);
        UE_LOG(LogTemp, Log, TEXT("PedDataComponent: Loaded cached profile '%s'"), *ProfileName);
        return;
    }

    // Try to load from data tables
    FPedBehaviorProfile LoadedProfile;
    if (LoadProfileFromDataTables(ProfileName, LoadedProfile))
    {
        LoadedProfiles.Add(ProfileName, LoadedProfile);
        SetBehaviorProfile(LoadedProfile);
        UE_LOG(LogTemp, Log, TEXT("PedDataComponent: Loaded profile '%s' from data tables"), *ProfileName);
        return;
    }

    // Try to load from XML
    LoadFromXMLData(ProfileName);
}

void UPedDataComponent::SetBehaviorProfile(const FPedBehaviorProfile& Profile)
{
    CurrentProfile = Profile;
    bIsProfileLoaded = true;

    // Cache the profile
    LoadedProfiles.Add(Profile.ProfileName, Profile);

    OnProfileLoaded.Broadcast(CurrentProfile);

    UE_LOG(LogTemp, Log, TEXT("PedDataComponent: Set behavior profile '%s'"), *Profile.ProfileName);
}

void UPedDataComponent::LoadFromXMLData(const FString& PedName)
{
    LoadXMLData();

    // Try to find the ped in loaded data
    if (FPedBehaviorProfile* FoundProfile = LoadedProfiles.Find(PedName))
    {
        SetBehaviorProfile(*FoundProfile);
    }
    else
    {
        UE_LOG(LogTemp, Warning, TEXT("PedDataComponent: Ped '%s' not found in XML data"), *PedName);
    }
}

void UPedDataComponent::SaveCurrentProfileToXML()
{
    if (!bIsProfileLoaded)
    {
        UE_LOG(LogTemp, Warning, TEXT("PedDataComponent: No profile loaded to save"));
        return;
    }

    // In a full implementation, this would serialize the current profile to XML
    UE_LOG(LogTemp, Log, TEXT("PedDataComponent: Saving profile '%s' to XML (not implemented)"), *CurrentProfile.ProfileName);
}

void UPedDataComponent::ModifyHealth(float Amount)
{
    float OldHealth = CurrentProfile.Attributes.Health;
    CurrentProfile.Attributes.Health = FMath::Clamp(
        CurrentProfile.Attributes.Health + Amount, 
        0.0f, 
        CurrentProfile.Attributes.MaxHealth
    );

    if (FMath::Abs(CurrentProfile.Attributes.Health - OldHealth) > 0.01f)
    {
        OnAttributeChanged.Broadcast(TEXT("Health"), CurrentProfile.Attributes.Health);
    }
}

void UPedDataComponent::ModifyArmor(float Amount)
{
    float OldArmor = CurrentProfile.Attributes.Armor;
    CurrentProfile.Attributes.Armor = FMath::Clamp(
        CurrentProfile.Attributes.Armor + Amount, 
        0.0f, 
        CurrentProfile.Attributes.MaxArmor
    );

    if (FMath::Abs(CurrentProfile.Attributes.Armor - OldArmor) > 0.01f)
    {
        OnAttributeChanged.Broadcast(TEXT("Armor"), CurrentProfile.Attributes.Armor);
    }
}

void UPedDataComponent::ModifyStamina(float Amount)
{
    float OldStamina = CurrentProfile.Attributes.Stamina;
    CurrentProfile.Attributes.Stamina = FMath::Clamp(
        CurrentProfile.Attributes.Stamina + Amount, 
        0.0f, 
        CurrentProfile.Attributes.MaxStamina
    );

    if (FMath::Abs(CurrentProfile.Attributes.Stamina - OldStamina) > 0.01f)
    {
        OnAttributeChanged.Broadcast(TEXT("Stamina"), CurrentProfile.Attributes.Stamina);
    }
}

void UPedDataComponent::SetPedType(EPedType NewType)
{
    EPedType OldType = CurrentProfile.Attributes.PedType;
    CurrentProfile.Attributes.PedType = NewType;

    if (NewType != OldType)
    {
        OnAttributeChanged.Broadcast(TEXT("PedType"), (float)NewType);
    }
}

ERelationshipType UPedDataComponent::GetRelationshipWith(EPedType TargetPedType) const
{
    if (const FPedRelationshipData* RelData = FindRelationshipData(TargetPedType))
    {
        return RelData->RelationshipType;
    }

    return ERelationshipType::Neutral;
}

bool UPedDataComponent::CanAttack(EPedType TargetPedType) const
{
    if (const FPedRelationshipData* RelData = FindRelationshipData(TargetPedType))
    {
        return RelData->bCanAttack;
    }

    return false;
}

bool UPedDataComponent::CanHelp(EPedType TargetPedType) const
{
    if (const FPedRelationshipData* RelData = FindRelationshipData(TargetPedType))
    {
        return RelData->bCanHelp;
    }

    return false;
}

void UPedDataComponent::AddRelationship(const FPedRelationshipData& RelationshipData)
{
    // Remove existing relationship with this ped type
    CurrentProfile.Relationships.RemoveAll([RelationshipData](const FPedRelationshipData& Existing)
    {
        return Existing.TargetPedType == RelationshipData.TargetPedType;
    });

    // Add new relationship
    CurrentProfile.Relationships.Add(RelationshipData);

    OnRelationshipChanged.Broadcast(RelationshipData.TargetPedType, RelationshipData.RelationshipType);
}

void UPedDataComponent::ModifyRelationship(EPedType TargetPedType, ERelationshipType NewRelationship)
{
    FPedRelationshipData* RelData = FindRelationshipData(TargetPedType);
    
    if (RelData)
    {
        RelData->RelationshipType = NewRelationship;
    }
    else
    {
        // Create new relationship
        FPedRelationshipData NewRelData;
        NewRelData.TargetPedType = TargetPedType;
        NewRelData.RelationshipType = NewRelationship;
        CurrentProfile.Relationships.Add(NewRelData);
    }

    OnRelationshipChanged.Broadcast(TargetPedType, NewRelationship);
}

bool UPedDataComponent::HasWeapon(const FString& WeaponName) const
{
    return CurrentProfile.AvailableWeapons.Contains(WeaponName);
}

void UPedDataComponent::AddWeapon(const FString& WeaponName)
{
    if (!CurrentProfile.AvailableWeapons.Contains(WeaponName))
    {
        CurrentProfile.AvailableWeapons.Add(WeaponName);
        OnWeaponAdded.Broadcast(WeaponName);
    }
}

void UPedDataComponent::RemoveWeapon(const FString& WeaponName)
{
    if (CurrentProfile.AvailableWeapons.Remove(WeaponName) > 0)
    {
        OnWeaponRemoved.Broadcast(WeaponName);
    }
}

TArray<FString> UPedDataComponent::GetAvailableWeapons() const
{
    return CurrentProfile.AvailableWeapons;
}

// Private Functions
void UPedDataComponent::InitializeComponent()
{
    // Initialize any required state
}

void UPedDataComponent::LoadDefaultProfile()
{
    SetBehaviorProfile(DefaultProfile);
}

bool UPedDataComponent::LoadProfileFromDataTables(const FString& ProfileName, FPedBehaviorProfile& OutProfile)
{
    OutProfile.ProfileName = ProfileName;

    // Try to load attributes
    if (!LoadAttributesFromTable(ProfileName, OutProfile.Attributes))
    {
        return false;
    }

    // Try to load combat data
    if (!LoadCombatDataFromTable(ProfileName, OutProfile.CombatData))
    {
        // Use default combat data if not found
        OutProfile.CombatData = FPedCombatData();
    }

    // Load relationships
    LoadRelationshipsFromTable(ProfileName, OutProfile.Relationships);

    return true;
}

bool UPedDataComponent::LoadAttributesFromTable(const FString& PedName, FPedAttributes& OutAttributes)
{
    if (!PedAttributesTable)
    {
        return false;
    }

    FPedAttributes* FoundAttributes = PedAttributesTable->FindRow<FPedAttributes>(FName(*PedName), TEXT("PedDataComponent"));
    if (FoundAttributes)
    {
        OutAttributes = *FoundAttributes;
        return true;
    }

    return false;
}

bool UPedDataComponent::LoadCombatDataFromTable(const FString& PedName, FPedCombatData& OutCombatData)
{
    if (!PedCombatTable)
    {
        return false;
    }

    FPedCombatData* FoundCombatData = PedCombatTable->FindRow<FPedCombatData>(FName(*PedName), TEXT("PedDataComponent"));
    if (FoundCombatData)
    {
        OutCombatData = *FoundCombatData;
        return true;
    }

    return false;
}

void UPedDataComponent::LoadRelationshipsFromTable(const FString& PedName, TArray<FPedRelationshipData>& OutRelationships)
{
    if (!PedRelationshipsTable)
    {
        return;
    }

    // In a full implementation, this would load all relationships for this ped type
    // For now, just clear the array
    OutRelationships.Empty();
}

void UPedDataComponent::LoadXMLData()
{
    FString XMLPath = FPaths::ProjectDir() + XMLDataPath;

    // Load different XML files
    LoadXMLFile(XMLPath + TEXT("PedAttributes.xml"), [this](const FString& Content) { ParsePedAttributesXML(Content); });
    LoadXMLFile(XMLPath + TEXT("PedCombat.xml"), [this](const FString& Content) { ParsePedCombatXML(Content); });
    LoadXMLFile(XMLPath + TEXT("PedRelationships.xml"), [this](const FString& Content) { ParsePedRelationshipsXML(Content); });
}

void UPedDataComponent::LoadXMLFile(const FString& FilePath, TFunction<void(const FString&)> ParseFunction)
{
    FString XMLContent;
    if (FFileHelper::LoadFileToString(XMLContent, *FilePath))
    {
        ParseFunction(XMLContent);
        UE_LOG(LogTemp, Log, TEXT("PedDataComponent: Loaded XML file %s"), *FilePath);
    }
    else
    {
        UE_LOG(LogTemp, Warning, TEXT("PedDataComponent: Failed to load XML file %s"), *FilePath);
    }
}

void UPedDataComponent::ParsePedAttributesXML(const FString& XMLContent)
{
    // Basic XML parsing - in production, use proper XML parser
    UE_LOG(LogTemp, Log, TEXT("PedDataComponent: Parsing PedAttributes.xml (basic implementation)"));
    
    // Create some default profiles for demonstration
    FPedBehaviorProfile NikoProfile;
    NikoProfile.ProfileName = TEXT("Niko");
    NikoProfile.Attributes.PedName = TEXT("Niko");
    NikoProfile.Attributes.PedType = EPedType::Player;
    NikoProfile.Attributes.Health = 200.0f;
    NikoProfile.Attributes.MaxHealth = 200.0f;
    NikoProfile.AnimationGroup = TEXT("Player_Move");
    NikoProfile.bIsLawEnforcement = false;
    
    LoadedProfiles.Add(TEXT("Niko"), NikoProfile);

    FPedBehaviorProfile CopProfile;
    CopProfile.ProfileName = TEXT("Cop");
    CopProfile.Attributes.PedName = TEXT("Cop");
    CopProfile.Attributes.PedType = EPedType::Police;
    CopProfile.Attributes.Health = 150.0f;
    CopProfile.Attributes.MaxHealth = 150.0f;
    CopProfile.AnimationGroup = TEXT("Move_Cop");
    CopProfile.bIsLawEnforcement = true;
    
    LoadedProfiles.Add(TEXT("Cop"), CopProfile);
}

void UPedDataComponent::ParsePedCombatXML(const FString& XMLContent)
{
    UE_LOG(LogTemp, Log, TEXT("PedDataComponent: Parsing PedCombat.xml (basic implementation)"));
    
    // In a full implementation, this would parse combat data and update existing profiles
}

void UPedDataComponent::ParsePedRelationshipsXML(const FString& XMLContent)
{
    UE_LOG(LogTemp, Log, TEXT("PedDataComponent: Parsing PedRelationships.xml (basic implementation)"));
    
    // Create some default relationships
    if (FPedBehaviorProfile* CopProfile = LoadedProfiles.Find(TEXT("Cop")))
    {
        FPedRelationshipData CopToPlayer;
        CopToPlayer.TargetPedType = EPedType::Player;
        CopToPlayer.RelationshipType = ERelationshipType::Neutral;
        CopToPlayer.bCanAttack = true;
        CopToPlayer.bCanHelp = true;
        
        CopProfile->Relationships.Add(CopToPlayer);
    }
}

FPedRelationshipData* UPedDataComponent::FindRelationshipData(EPedType TargetPedType)
{
    for (FPedRelationshipData& RelData : CurrentProfile.Relationships)
    {
        if (RelData.TargetPedType == TargetPedType)
        {
            return &RelData;
        }
    }

    return nullptr;
}

const FPedRelationshipData* UPedDataComponent::FindRelationshipData(EPedType TargetPedType) const
{
    for (const FPedRelationshipData& RelData : CurrentProfile.Relationships)
    {
        if (RelData.TargetPedType == TargetPedType)
        {
            return &RelData;
        }
    }

    return nullptr;
}
