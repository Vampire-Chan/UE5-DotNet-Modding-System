#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "Engine/DataTable.h"
#include "../Core/Enums/GameWorldEnums.h"
#include "PedDataComponent.generated.h"

USTRUCT(BlueprintType)
struct FPedAttributes : public FTableRowBase
{
    GENERATED_BODY()

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Attributes")
    FString PedName;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Attributes")
    EPedType PedType;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Attributes")
    float Health;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Attributes")
    float MaxHealth;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Attributes")
    float Armor;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Attributes")
    float MaxArmor;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Attributes")
    float MovementSpeed;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Attributes")
    float RunSpeed;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Attributes")
    float SprintSpeed;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Attributes")
    float Stamina;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Attributes")
    float MaxStamina;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Attributes")
    float Accuracy;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Attributes")
    float Aggression;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Attributes")
    float Intelligence;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Attributes")
    float Courage;

    FPedAttributes()
    {
        PedName = TEXT("Default Ped");
        PedType = EPedType::Civilian;
        Health = 100.0f;
        MaxHealth = 100.0f;
        Armor = 0.0f;
        MaxArmor = 100.0f;
        MovementSpeed = 150.0f;
        RunSpeed = 300.0f;
        SprintSpeed = 600.0f;
        Stamina = 100.0f;
        MaxStamina = 100.0f;
        Accuracy = 50.0f;
        Aggression = 25.0f;
        Intelligence = 50.0f;
        Courage = 50.0f;
    }
};

USTRUCT(BlueprintType)
struct FPedCombatData : public FTableRowBase
{
    GENERATED_BODY()

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Combat")
    float SightRange;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Combat")
    float HearingRange;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Combat")
    float AttackRange;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Combat")
    float FleeDistance;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Combat")
    float ReactionTime;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Combat")
    float WeaponAccuracy;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Combat")
    float WeaponRange;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Combat")
    float WeaponDamage;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Combat")
    float ShootRate;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Combat")
    int32 MaxAmmo;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Combat")
    bool bCanUseCover;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Combat")
    bool bCanStrafe;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Combat")
    bool bCanFlee;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Combat")
    TArray<FString> PreferredWeapons;

    FPedCombatData()
    {
        SightRange = 1000.0f;
        HearingRange = 500.0f;
        AttackRange = 800.0f;
        FleeDistance = 200.0f;
        ReactionTime = 0.5f;
        WeaponAccuracy = 0.7f;
        WeaponRange = 800.0f;
        WeaponDamage = 25.0f;
        ShootRate = 1.0f;
        MaxAmmo = 30;
        bCanUseCover = true;
        bCanStrafe = true;
        bCanFlee = false;
        PreferredWeapons.Empty();
    }
};

USTRUCT(BlueprintType)
struct FPedRelationshipData : public FTableRowBase
{
    GENERATED_BODY()

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Relationships")
    EPedType TargetPedType;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Relationships")
    ERelationshipType RelationshipType;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Relationships")
    float Respect;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Relationships")
    float Like;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Relationships")
    float Fear;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Relationships")
    float Hate;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Relationships")
    bool bCanAttack;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Relationships")
    bool bCanHelp;

    FPedRelationshipData()
    {
        TargetPedType = EPedType::Civilian;
        RelationshipType = ERelationshipType::Neutral;
        Respect = 0.0f;
        Like = 0.0f;
        Fear = 0.0f;
        Hate = 0.0f;
        bCanAttack = false;
        bCanHelp = false;
    }
};

USTRUCT(BlueprintType)
struct FPedBehaviorProfile
{
    GENERATED_BODY()

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Behavior")
    FString ProfileName;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Behavior")
    FPedAttributes Attributes;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Behavior")
    FPedCombatData CombatData;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Behavior")
    TArray<FPedRelationshipData> Relationships;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Behavior")
    FString AnimationGroup;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Behavior")
    FString VoiceGroup;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Behavior")
    TArray<FString> AvailableWeapons;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Behavior")
    bool bIsAggressive;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Behavior")
    bool bIsLawEnforcement;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Behavior")
    bool bIsGangMember;

    FPedBehaviorProfile()
    {
        ProfileName = TEXT("Default Behavior");
        AnimationGroup = TEXT("player_movement");
        VoiceGroup = TEXT("Default");
        AvailableWeapons.Empty();
        bIsAggressive = false;
        bIsLawEnforcement = false;
        bIsGangMember = false;
    }
};

/**
 * Component responsible for managing Ped data, attributes, and behavioral profiles.
 * Handles loading from XML data files and providing runtime data access.
 */
UCLASS(ClassGroup=(Custom), meta=(BlueprintSpawnableComponent))
class GAME_API UPedDataComponent : public UActorComponent
{
    GENERATED_BODY()

public:
    UPedDataComponent();

protected:
    virtual void BeginPlay() override;

public:
    virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

    // Data Management
    UFUNCTION(BlueprintCallable, Category = "Ped Data")
    void LoadBehaviorProfile(const FString& ProfileName);

    UFUNCTION(BlueprintCallable, Category = "Ped Data")
    void SetBehaviorProfile(const FPedBehaviorProfile& Profile);

    UFUNCTION(BlueprintCallable, Category = "Ped Data")
    void LoadFromXMLData(const FString& PedName);

    UFUNCTION(BlueprintCallable, Category = "Ped Data")
    void SaveCurrentProfileToXML();

    // Attribute Access
    UFUNCTION(BlueprintPure, Category = "Ped Data")
    const FPedBehaviorProfile& GetCurrentProfile() const { return CurrentProfile; }

    UFUNCTION(BlueprintPure, Category = "Ped Data")
    const FPedAttributes& GetAttributes() const { return CurrentProfile.Attributes; }

    UFUNCTION(BlueprintPure, Category = "Ped Data")
    const FPedCombatData& GetCombatData() const { return CurrentProfile.CombatData; }

    UFUNCTION(BlueprintPure, Category = "Ped Data")
    EPedType GetPedType() const { return CurrentProfile.Attributes.PedType; }

    UFUNCTION(BlueprintPure, Category = "Ped Data")
    bool IsProfileLoaded() const { return bIsProfileLoaded; }

    // Attribute Modification
    UFUNCTION(BlueprintCallable, Category = "Ped Data")
    void ModifyHealth(float Amount);

    UFUNCTION(BlueprintCallable, Category = "Ped Data")
    void ModifyArmor(float Amount);

    UFUNCTION(BlueprintCallable, Category = "Ped Data")
    void ModifyStamina(float Amount);

    UFUNCTION(BlueprintCallable, Category = "Ped Data")
    void SetPedType(EPedType NewType);

    // Relationship System
    UFUNCTION(BlueprintPure, Category = "Ped Data")
    ERelationshipType GetRelationshipWith(EPedType TargetPedType) const;

    UFUNCTION(BlueprintPure, Category = "Ped Data")
    bool CanAttack(EPedType TargetPedType) const;

    UFUNCTION(BlueprintPure, Category = "Ped Data")
    bool CanHelp(EPedType TargetPedType) const;

    UFUNCTION(BlueprintCallable, Category = "Ped Data")
    void AddRelationship(const FPedRelationshipData& RelationshipData);

    UFUNCTION(BlueprintCallable, Category = "Ped Data")
    void ModifyRelationship(EPedType TargetPedType, ERelationshipType NewRelationship);

    // Weapon System
    UFUNCTION(BlueprintPure, Category = "Ped Data")
    bool HasWeapon(const FString& WeaponName) const;

    UFUNCTION(BlueprintCallable, Category = "Ped Data")
    void AddWeapon(const FString& WeaponName);

    UFUNCTION(BlueprintCallable, Category = "Ped Data")
    void RemoveWeapon(const FString& WeaponName);

    UFUNCTION(BlueprintPure, Category = "Ped Data")
    TArray<FString> GetAvailableWeapons() const;

    // Data Tables
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Data Tables")
    UDataTable* PedAttributesTable;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Data Tables")
    UDataTable* PedCombatTable;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Data Tables")
    UDataTable* PedRelationshipsTable;

    // Configuration
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Configuration")
    FPedBehaviorProfile DefaultProfile;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Configuration")
    bool bAutoLoadDefaultProfile;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Configuration")
    FString XMLDataPath;

    // Events
    DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnProfileLoaded, const FPedBehaviorProfile&, LoadedProfile);
    DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FOnAttributeChanged, const FString&, AttributeName, float, NewValue);
    DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FOnRelationshipChanged, EPedType, TargetType, ERelationshipType, NewRelationship);
    DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnWeaponAdded, const FString&, WeaponName);
    DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnWeaponRemoved, const FString&, WeaponName);

    UPROPERTY(BlueprintAssignable, Category = "Events")
    FOnProfileLoaded OnProfileLoaded;

    UPROPERTY(BlueprintAssignable, Category = "Events")
    FOnAttributeChanged OnAttributeChanged;

    UPROPERTY(BlueprintAssignable, Category = "Events")
    FOnRelationshipChanged OnRelationshipChanged;

    UPROPERTY(BlueprintAssignable, Category = "Events")
    FOnWeaponAdded OnWeaponAdded;

    UPROPERTY(BlueprintAssignable, Category = "Events")
    FOnWeaponRemoved OnWeaponRemoved;

private:
    // Current state
    UPROPERTY()
    FPedBehaviorProfile CurrentProfile;

    bool bIsProfileLoaded;

    // Runtime data
    TMap<FString, FPedBehaviorProfile> LoadedProfiles;

    // Internal functions
    void InitializeComponent();
    void LoadDefaultProfile();
    bool LoadProfileFromDataTables(const FString& ProfileName, FPedBehaviorProfile& OutProfile);
    bool LoadAttributesFromTable(const FString& PedName, FPedAttributes& OutAttributes);
    bool LoadCombatDataFromTable(const FString& PedName, FPedCombatData& OutCombatData);
    void LoadRelationshipsFromTable(const FString& PedName, TArray<FPedRelationshipData>& OutRelationships);
    
    // XML processing
    void LoadXMLData();
    void ParsePedAttributesXML(const FString& XMLContent);
    void ParsePedCombatXML(const FString& XMLContent);
    void ParsePedRelationshipsXML(const FString& XMLContent);
    
    // XML file loading
    void LoadXMLFile(const FString& FilePath, TFunction<void(const FString&)> ParseFunction);
    
    // Utility functions
    FPedRelationshipData* FindRelationshipData(EPedType TargetPedType);
    const FPedRelationshipData* FindRelationshipData(EPedType TargetPedType) const;
};
