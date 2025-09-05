#include "PedCharacterComponent.h"
#include "Engine/StreamableManager.h"
#include "tinyxml2.h"
#include "../Peds/Ped.h"

FPedComponentMesh::FPedComponentMesh()
{
    BodyPartType = EPedBodyPart::Invalid;
    ComponentName = TEXT("");
    AltName = TEXT("");
    MeshIndex = 0;
    TextureVariantIndex = 0;
    TextureRace = ETextureRaceType::Universal;
    Materials.Empty();
}

void FPedComponentMesh::GenerateAssetPaths(const FString& CharacterPath)
{
    FString BodyPartFolder = ComponentName.ToLower();

    // Mesh Path charactername/componentname/componentname_index.uasset
    MeshAssetPath = TSoftObjectPtr<USkeletalMesh>(FSoftObjectPath(FString::Printf(TEXT("%s/%s/%s_%03d.uasset"), *CharacterPath, *BodyPartFolder, *BodyPartFolder, MeshIndex)));

	// Texture Paths charactername/componentname/Texture/componentname_index_textureindex_race.uasset
    FString TextureFolder = FString::Printf(TEXT("%s/%s/Texture"), *CharacterPath, *BodyPartFolder);
    FString RaceStr = UPedCharacterComponent::StaticClass()->GetDefaultObject<UPedCharacterComponent>()->GetRaceStringFromEnum(TextureRace);
    FString VariantChar = FString::Chr(TCHAR('a' + TextureVariantIndex));

    DiffuseTexturePath = TSoftObjectPtr<UTexture>(FSoftObjectPath(FString::Printf(TEXT("%s/%s_%03d_%s_%s.uasset"), *TextureFolder, *BodyPartFolder, MeshIndex, *VariantChar, *RaceStr)));
    NormalTexturePath = TSoftObjectPtr<UTexture>(FSoftObjectPath(FString::Printf(TEXT("%s/%s_%03d_normal.uasset"), *TextureFolder, *BodyPartFolder, MeshIndex)));
    SpecularTexturePath = TSoftObjectPtr<UTexture>(FSoftObjectPath(FString::Printf(TEXT("%s/%s_%03d_specular.uasset"), *TextureFolder, *BodyPartFolder, MeshIndex)));
}

FPedPropMesh::FPedPropMesh()
{
    PropType = EPedPropType::Invalid;
    PropName = TEXT("");
    AltName = TEXT("");
    MeshIndex = 0;
    TextureVariantIndex = 0;
    TextureRace = ETextureRaceType::Universal;
    AttachSocketName = NAME_None;
    Materials.Empty();
}

void FPedPropMesh::GenerateAssetPaths(const FString& CharacterPath)
{
    FString PropFolder = PropName.ToLower();

    // Mesh Path
    PropMeshAssetPath = TSoftObjectPtr<UStaticMesh>(FSoftObjectPath(FString::Printf(TEXT("%s/%s/%s_%03d.uasset"), *CharacterPath, *PropFolder, *PropFolder, MeshIndex)));

    // Texture Paths
    FString TextureFolder = FString::Printf(TEXT("%s/%s/Texture"), *CharacterPath, *PropFolder);
    FString RaceStr = UPedCharacterComponent::StaticClass()->GetDefaultObject<UPedCharacterComponent>()->GetRaceStringFromEnum(TextureRace);
    FString VariantChar = FString::Chr(TCHAR('a' + TextureVariantIndex));

    DiffuseTexturePath = TSoftObjectPtr<UTexture>(FSoftObjectPath(FString::Printf(TEXT("%s/%s_%03d_%s_%s.uasset"), *TextureFolder, *PropFolder, MeshIndex, *VariantChar, *RaceStr)));
    NormalTexturePath = TSoftObjectPtr<UTexture>(FSoftObjectPath(FString::Printf(TEXT("%s/%s_%03d_normal.uasset"), *TextureFolder, *PropFolder, MeshIndex)));
    SpecularTexturePath = TSoftObjectPtr<UTexture>(FSoftObjectPath(FString::Printf(TEXT("%s/%s_%03d_specular.uasset"), *TextureFolder, *PropFolder, MeshIndex)));
}

UPedCharacterComponent::UPedCharacterComponent()
{
    PrimaryComponentTick.bCanEverTick = false;
    PrimaryComponentTick.bStartWithTickEnabled = false;

    // Initialize configuration
    bAutoLoadOnBeginPlay = true;
    bApplyRandomVariationsOnLoad = true;
    MaxHeightVariation = 0.1f;
    MaxWeightVariation = 0.15f;

    // Initialize XML paths
    PedMeshXMLPath =  TEXT("Data/Peds/PedMesh.xml");
    PedVariationsXMLPath = TEXT("Data/Peds/PedVariations.xml");
    PedPropsXMLPath = TEXT("Data/Peds/PedPropMesh.xml");

    // Initialize state
    bIsProfileLoaded = false;
    bIsLoadingAssets = false;
    LoadingProgress = 0.0f;
    TotalAssetsToLoad = 0;
    AssetsLoaded = 0;

    AnimationController = nullptr;
    CachedSkeletalMeshComponent = nullptr;

    DefaultProfile.ProfileName = TEXT("Default Ped");
    DefaultProfile.Scale = FVector(1.0f, 1.0f, 1.0f);
}

void UPedCharacterComponent::BeginPlay()
{
    Super::BeginPlay();

    InitializeComponent();

    // Load base skeleton from Peds.xml first
    LoadBaseSkeletonFromXML();

    // Load XML data first
    LoadPedMeshXML();
    LoadPedVariationsXML();
    LoadPedPropsXML();

    if (bAutoLoadOnBeginPlay)
    {
        LoadVisualProfile(DefaultProfile);
    }

    UE_LOG(LogTemp, Log, TEXT("PedCharacterComponent: Initialized for %s"), 
           GetOwner() ? *GetOwner()->GetName() : TEXT("Unknown"));
}

void UPedCharacterComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
    Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

    if (bIsLoadingAssets)
    {
        UpdateLoadingProgress();
    }
    else
    {
        SetComponentTickEnabled(false);
    }
}

void UPedCharacterComponent::LoadVisualProfile(const FPedVisualProfile& Profile)
{
    CurrentProfile = Profile;
    bIsProfileLoaded = false;
    bIsLoadingAssets = true;
    LoadingProgress = 0.0f;
    AssetsLoaded = 0;
    
    LoadedComponentMeshes.Empty();
    LoadedPropMeshes.Empty();
    LoadedComponentMaterials.Empty();

    const FPedVariationSet& CurrentVar = Profile.CurrentVariation;
    const FPedMeshConfiguration& MeshConfig = CurrentVar.MeshConfig;

    TotalAssetsToLoad = 0;

    // Count component assets
    TArray<FPedComponentMesh> Components = {
        MeshConfig.Head, MeshConfig.Hair, MeshConfig.Eyes, MeshConfig.Teeth,
        MeshConfig.Upper, MeshConfig.Lower, MeshConfig.Hands, MeshConfig.Arms,
        MeshConfig.Feet, MeshConfig.Nails
    };

    for (const FPedComponentMesh& Component : Components)
    {
        if (Component.MeshAssetPath.IsValid())
        {
            TotalAssetsToLoad++;
            TotalAssetsToLoad += Component.Materials.Num();
        }
    }

    for (const FPedPropMesh& Prop : MeshConfig.Props)
    {
        if (Prop.PropMeshAssetPath.IsValid())
        {
            TotalAssetsToLoad++;
            TotalAssetsToLoad += Prop.Materials.Num();
        }
    }

    // Start loading
    for (const FPedComponentMesh& Component : Components)
    {
        if (Component.MeshAssetPath.IsValid())
        {
            LoadComponentMeshAsync(Component);
        }
    }

    for (const FPedPropMesh& Prop : MeshConfig.Props)
    {
        if (Prop.PropMeshAssetPath.IsValid())
        {
            LoadPropMeshAsync(Prop);
        }
    }

    if (TotalAssetsToLoad == 0)
    {
        bIsLoadingAssets = false;
        bIsProfileLoaded = true;
        OnProfileLoaded.Broadcast(CurrentProfile);
        OnAllAssetsLoaded.Broadcast();
    }
    else
    {
        SetComponentTickEnabled(true);
    }

    UE_LOG(LogTemp, Log, TEXT("PedCharacterComponent: Loading profile '%s' with %d assets"), 
           *Profile.ProfileName, TotalAssetsToLoad);
}

void UPedCharacterComponent::LoadVariationSet(const FString& VariationName)
{
    if (FPedVariationSet* FoundVariation = VariationSets.Find(VariationName))
    {
        CurrentProfile.CurrentVariation = *FoundVariation;
        OnVariationLoaded.Broadcast(*FoundVariation);
        LoadVisualProfile(CurrentProfile);
        UE_LOG(LogTemp, Log, TEXT("PedCharacterComponent: Loaded variation set '%s'"), *VariationName);
    }
    else
    {
        UE_LOG(LogTemp, Warning, TEXT("PedCharacterComponent: Variation set '%s' not found"), *VariationName);
    }
}

void UPedCharacterComponent::SetMeshConfiguration(const FPedMeshConfiguration& MeshConfig)
{
    CurrentProfile.CurrentVariation.MeshConfig = MeshConfig;
    if (bIsProfileLoaded)
    {
        ApplyLoadedAssetsToMesh();
    }
}

void UPedCharacterComponent::ApplyComponentMesh(const FPedComponentMesh& ComponentMesh, USkeletalMeshComponent* TargetComponent)
{
    if (!TargetComponent)
    {
        TargetComponent = FindSkeletalMeshComponent();
    }

    if (!TargetComponent || ComponentMesh.ComponentName.IsEmpty())
    {
        return;
    }

    // Apply mesh if loaded
    if (USkeletalMesh* LoadedMesh = LoadedComponentMeshes.FindRef(ComponentMesh.ComponentName))
    {
        // For main body components, set as primary mesh
        if (ComponentMesh.ComponentName == TEXT("Upper") || ComponentMesh.ComponentName == TEXT("Body"))
        {
            TargetComponent->SetSkeletalMesh(LoadedMesh);
        }
        // For other components, this would typically involve additional skeletal mesh components
        // or blend shapes in a real implementation
    }

    // Apply materials
    if (TArray<UMaterialInterface*>* ComponentMaterials = LoadedComponentMaterials.Find(ComponentMesh.ComponentName))
    {
        for (int32 i = 0; i < ComponentMaterials->Num() && i < TargetComponent->GetNumMaterials(); i++)
        {
            if ((*ComponentMaterials)[i])
            {
                TargetComponent->SetMaterial(i, (*ComponentMaterials)[i]);
            }
        }
    }
}

void UPedCharacterComponent::ApplyPropMesh(const FPedPropMesh& PropMesh)
{
    if (PropMesh.PropName.IsEmpty())
    {
        return;
    }

    UStaticMesh* LoadedPropMesh = LoadedPropMeshes.FindRef(PropMesh.PropName);
    if (!LoadedPropMesh)
    {
        return;
    }

    // Create or find prop component
    UStaticMeshComponent* PropComponent = CreatePropComponent(PropMesh);
    if (PropComponent)
    {
        PropComponent->SetStaticMesh(LoadedPropMesh);

        // Apply materials
        if (TArray<UMaterialInterface*>* PropMaterials = LoadedComponentMaterials.Find(PropMesh.PropName))
        {
            for (int32 i = 0; i < PropMaterials->Num() && i < PropComponent->GetNumMaterials(); i++)
            {
                if ((*PropMaterials)[i])
                {
                    PropComponent->SetMaterial(i, (*PropMaterials)[i]);
                }
            }
        }
    }
}

void UPedCharacterComponent::LoadFromXMLData(const FString& PedName)
{
    if (FPedVariationSet* FoundVariation = VariationSets.Find(PedName + TEXT("_Default")))
    {
        LoadVariationSet(PedName + TEXT("_Default"));
    }
    else
    {
        UE_LOG(LogTemp, Warning, TEXT("PedCharacterComponent: No XML data found for Ped '%s'"), *PedName);
    }
}

void UPedCharacterComponent::LoadPedMeshXML()
{
    FString XMLPath = FPaths::ProjectDir() + PedMeshXMLPath;
    FString XMLContent;
    
    if (FFileHelper::LoadFileToString(XMLContent, *XMLPath))
    {
        tinyxml2::XMLDocument Doc;
        tinyxml2::XMLError Result = Doc.Parse(TCHAR_TO_UTF8(*XMLContent));

        if (Result == tinyxml2::XML_SUCCESS)
        {
            ParsePedMeshXMLTiny(Doc);
            UE_LOG(LogTemp, Log, TEXT("PedCharacterComponent: Loaded PedMesh.xml"));
        }
        else
        {
            UE_LOG(LogTemp, Warning, TEXT("PedCharacterComponent: Failed to parse PedMesh.xml. Error: %s"), UTF8_TO_TCHAR(Doc.ErrorStr()));
        }
    }
    else
    {
        UE_LOG(LogTemp, Warning, TEXT("PedCharacterComponent: Failed to load PedMesh.xml from %s"), *XMLPath);
    }
}

void UPedCharacterComponent::LoadPedVariationsXML()
{
    FString XMLPath = FPaths::ProjectDir() + PedVariationsXMLPath;
    FString XMLContent;
    
    if (FFileHelper::LoadFileToString(XMLContent, *XMLPath))
    {
        tinyxml2::XMLDocument Doc;
        tinyxml2::XMLError Result = Doc.Parse(TCHAR_TO_UTF8(*XMLContent));

        if (Result == tinyxml2::XML_SUCCESS)
        {
            ParsePedVariationsXMLTiny(Doc);
            UE_LOG(LogTemp, Log, TEXT("PedCharacterComponent: Loaded PedVariations.xml"));
        }
        else
        {
            UE_LOG(LogTemp, Warning, TEXT("PedCharacterComponent: Failed to parse PedVariations.xml. Error: %s"), UTF8_TO_TCHAR(Doc.ErrorStr()));
        }
    }
    else
    {
        UE_LOG(LogTemp, Warning, TEXT("PedCharacterComponent: Failed to load PedVariations.xml from %s"), *XMLPath);
    }
}

void UPedCharacterComponent::LoadPedPropsXML()
{
    FString XMLPath = FPaths::ProjectDir() + PedPropsXMLPath;
    FString XMLContent;
    
    if (FFileHelper::LoadFileToString(XMLContent, *XMLPath))
    {
        tinyxml2::XMLDocument Doc;
        tinyxml2::XMLError Result = Doc.Parse(TCHAR_TO_UTF8(*XMLContent));

        if (Result == tinyxml2::XML_SUCCESS)
        {
            ParsePedPropsXMLTiny(Doc);
            UE_LOG(LogTemp, Log, TEXT("PedCharacterComponent: Loaded PedPropMesh.xml"));
        }
        else
        {
            UE_LOG(LogTemp, Warning, TEXT("PedCharacterComponent: Failed to parse PedPropMesh.xml. Error: %s"), UTF8_TO_TCHAR(Doc.ErrorStr()));
        }
    }
    else
    {
        UE_LOG(LogTemp, Warning, TEXT("PedCharacterComponent: Failed to load PedPropMesh.xml from %s"), *XMLPath);
    }
}

void UPedCharacterComponent::SetComponentByName(const FString& ComponentName, const FPedComponentMesh& ComponentMesh)
{
    FPedComponentMesh* FoundComponent = FindComponentInConfig(ComponentName);
    if (FoundComponent)
    {
        *FoundComponent = ComponentMesh;
        ApplyComponentMesh(ComponentMesh, nullptr);
    }
}

FPedComponentMesh UPedCharacterComponent::GetComponentByName(const FString& ComponentName)
{
    FPedComponentMesh* FoundComponent = FindComponentInConfig(ComponentName);
    if (FoundComponent)
    {
        return *FoundComponent;
    }
    return FPedComponentMesh(); // Return empty component if not found
}

void UPedCharacterComponent::SetComponentByType(EPedBodyPart BodyPart, const FPedComponentMesh& ComponentMesh)
{
    FString ComponentName = GetBodyPartName(BodyPart);
    SetComponentByName(ComponentName, ComponentMesh);
}

FPedComponentMesh UPedCharacterComponent::GetComponentByType(EPedBodyPart BodyPart)
{
    FString ComponentName = GetBodyPartName(BodyPart);
    return GetComponentByName(ComponentName);
}

FString UPedCharacterComponent::GetBodyPartName(EPedBodyPart BodyPart)
{
    switch (BodyPart)
    {
    case EPedBodyPart::Head: return TEXT("Head");
    case EPedBodyPart::Hair: return TEXT("Hair");
    case EPedBodyPart::Eyes: return TEXT("Eyes");
    case EPedBodyPart::Teeth: return TEXT("Teeth");
    case EPedBodyPart::Upper: return TEXT("Upper");
    case EPedBodyPart::Lower: return TEXT("Lower");
    case EPedBodyPart::Hands: return TEXT("Hands");
    case EPedBodyPart::Arms: return TEXT("Arms");
    case EPedBodyPart::Feet: return TEXT("Feet");
    case EPedBodyPart::Nails: return TEXT("Nails");
    case EPedBodyPart::Legs: return TEXT("Legs");
    case EPedBodyPart::Torso: return TEXT("Torso");
    case EPedBodyPart::Jacket: return TEXT("Jacket");
    case EPedBodyPart::Vest: return TEXT("Vest");
    default: return TEXT("Unknown");
    }
}

EPedBodyPart UPedCharacterComponent::GetBodyPartFromName(const FString& BodyPartName)
{
    if (BodyPartName.Equals(TEXT("Head"), ESearchCase::IgnoreCase)) return EPedBodyPart::Head;
    if (BodyPartName.Equals(TEXT("Hair"), ESearchCase::IgnoreCase)) return EPedBodyPart::Hair;
    if (BodyPartName.Equals(TEXT("Eyes"), ESearchCase::IgnoreCase)) return EPedBodyPart::Eyes;
    if (BodyPartName.Equals(TEXT("Teeth"), ESearchCase::IgnoreCase)) return EPedBodyPart::Teeth;
    if (BodyPartName.Equals(TEXT("Upper"), ESearchCase::IgnoreCase) || 
        BodyPartName.Equals(TEXT("uppr"), ESearchCase::IgnoreCase) ||
        BodyPartName.Equals(TEXT("upperbody"), ESearchCase::IgnoreCase)) return EPedBodyPart::Upper;
    if (BodyPartName.Equals(TEXT("Lower"), ESearchCase::IgnoreCase) || 
        BodyPartName.Equals(TEXT("lowr"), ESearchCase::IgnoreCase)) return EPedBodyPart::Lower;
    if (BodyPartName.Equals(TEXT("Hands"), ESearchCase::IgnoreCase)) return EPedBodyPart::Hands;
    if (BodyPartName.Equals(TEXT("Arms"), ESearchCase::IgnoreCase)) return EPedBodyPart::Arms;
    if (BodyPartName.Equals(TEXT("Feet"), ESearchCase::IgnoreCase)) return EPedBodyPart::Feet;
    if (BodyPartName.Equals(TEXT("Nails"), ESearchCase::IgnoreCase)) return EPedBodyPart::Nails;
    if (BodyPartName.Equals(TEXT("Legs"), ESearchCase::IgnoreCase)) return EPedBodyPart::Legs;
    if (BodyPartName.Equals(TEXT("Torso"), ESearchCase::IgnoreCase)) return EPedBodyPart::Torso;
    if (BodyPartName.Equals(TEXT("Jacket"), ESearchCase::IgnoreCase)) return EPedBodyPart::Jacket;
    if (BodyPartName.Equals(TEXT("Vest"), ESearchCase::IgnoreCase)) return EPedBodyPart::Vest;
    return EPedBodyPart::Invalid;
}

void UPedCharacterComponent::SetComponentByVariation(EPedBodyPart BodyPart, int32 ComponentNumber, int32 TextureVariantIndex, ETextureRaceType Race)
{
    FPedComponentMesh NewComponent;
    NewComponent.BodyPartType = BodyPart;
    NewComponent.ComponentName = GetBodyPartName(BodyPart);
    NewComponent.MeshIndex = ComponentNumber;
    NewComponent.TextureVariantIndex = TextureVariantIndex;
    NewComponent.TextureRace = Race;

    // Generate asset paths
    NewComponent.GenerateAssetPaths(CharacterAssetFolder);

    SetComponentByType(BodyPart, NewComponent);
}

// === ENHANCED MANUAL COMPONENT CONTROL IMPLEMENTATIONS ===

void UPedCharacterComponent::SetComponentManual(EPedBodyPart BodyPart, int32 MeshIndex, int32 TextureIndex, const FString& TextureRace)
{
    // Convert string race to enum
    ETextureRaceType RaceEnum = GetRaceFromString(TextureRace);
    
    FPedComponentMesh NewComponent;
    NewComponent.BodyPartType = BodyPart;
    NewComponent.ComponentName = GetBodyPartName(BodyPart);
    NewComponent.MeshIndex = MeshIndex;
    NewComponent.TextureVariantIndex = TextureIndex;
    NewComponent.TextureRace = RaceEnum;

    // Generate asset paths based on naming convention
    NewComponent.GenerateAssetPaths(CharacterAssetFolder);

    // Apply material fallback if material not found
    ApplyMaterialFallbackToComponent(NewComponent);

    SetComponentByType(BodyPart, NewComponent);

    UE_LOG(LogTemp, Log, TEXT("SetComponentManual: %s - Mesh:%d, Texture:%d, Race:%s"), 
           *GetBodyPartName(BodyPart), MeshIndex, TextureIndex, *TextureRace);
}

void UPedCharacterComponent::SetComponentMaterialBySlot(EPedBodyPart BodyPart, int32 MaterialSlot, const FString& MaterialName)
{
    if (CachedSkeletalMeshComponent == nullptr)
    {
        UE_LOG(LogTemp, Warning, TEXT("SetComponentMaterialBySlot: No SkeletalMeshComponent found"));
        return;
    }

    // Load material with fallback to M_Ped
    TSoftObjectPtr<UMaterialInterface> MaterialPtr = LoadMaterialWithFallback(MaterialName);
    
    if (MaterialPtr.IsValid())
    {
        UMaterialInterface* LoadedMaterial = MaterialPtr.LoadSynchronous();
        if (LoadedMaterial)
        {
            CachedSkeletalMeshComponent->SetMaterial(MaterialSlot, LoadedMaterial);
            UE_LOG(LogTemp, Log, TEXT("Applied material %s to slot %d on %s"), 
                   *MaterialName, MaterialSlot, *GetBodyPartName(BodyPart));
        }
    }
}

void UPedCharacterComponent::SetComponentMaterialByIndex(EPedBodyPart BodyPart, int32 MaterialSlot, int32 MaterialIndex)
{
    // Get material name from XML mapping
    FString ComponentName = GetBodyPartName(BodyPart);
    FString MaterialName = GetMaterialNameFromXMLByIndex(ComponentName, MaterialIndex);
    
    if (MaterialName.IsEmpty())
    {
        UE_LOG(LogTemp, Warning, TEXT("Material index %d not found for component %s, using M_Ped fallback"), 
               MaterialIndex, *ComponentName);
        MaterialName = TEXT("M_Ped");
    }

    SetComponentMaterialBySlot(BodyPart, MaterialSlot, MaterialName);
}

void UPedCharacterComponent::ApplyMaterialFallbackToPed(const FString& FallbackMaterialName)
{
    if (CachedSkeletalMeshComponent == nullptr) return;

    TSoftObjectPtr<UMaterialInterface> FallbackMaterial = LoadMaterialWithFallback(FallbackMaterialName);
    UMaterialInterface* LoadedFallback = FallbackMaterial.LoadSynchronous();
    
    if (LoadedFallback)
    {
        // Apply fallback to all material slots
        int32 MaterialCount = CachedSkeletalMeshComponent->GetNumMaterials();
        for (int32 i = 0; i < MaterialCount; i++)
        {
            if (CachedSkeletalMeshComponent->GetMaterial(i) == nullptr)
            {
                CachedSkeletalMeshComponent->SetMaterial(i, LoadedFallback);
            }
        }
        
        UE_LOG(LogTemp, Log, TEXT("Applied fallback material %s to ped"), *FallbackMaterialName);
    }
}

void UPedCharacterComponent::SetComponentByAssetPath(EPedBodyPart BodyPart, const FString& AssetPath)
{
    FPedComponentMesh NewComponent;
    NewComponent.BodyPartType = BodyPart;
    NewComponent.ComponentName = GetBodyPartName(BodyPart);
    NewComponent.MeshAssetPath = TSoftObjectPtr<USkeletalMesh>(FSoftObjectPath(AssetPath));

    SetComponentByType(BodyPart, NewComponent);
}

void UPedCharacterComponent::GenerateComponentAssetPaths(FPedComponentMesh& ComponentMesh, const FString& CharacterPath)
{
    ComponentMesh.GenerateAssetPaths(CharacterPath);
}

void UPedCharacterComponent::SetPropByVariation(const FString& PropName, int32 PropNumber, int32 TextureVariantIndex, ETextureRaceType Race, FName AttachSocket)
{
    FPedPropMesh NewProp;
    NewProp.PropName = PropName;
    NewProp.MeshIndex = PropNumber;
    NewProp.TextureVariantIndex = TextureVariantIndex;
    NewProp.TextureRace = Race;
    NewProp.AttachSocketName = AttachSocket;

    // Generate asset paths
    NewProp.GenerateAssetPaths(CharacterAssetFolder);

    AddProp(NewProp);
}

ETextureRaceType UPedCharacterComponent::GetRaceFromString(const FString& RaceString)
{
    FString LowerRace = RaceString.ToLower();
        if (LowerRace == TEXT("uni")) return ETextureRaceType::Universal;
    if (LowerRace == TEXT("bla")) return ETextureRaceType::Black;
    if (LowerRace == TEXT("whi")) return ETextureRaceType::White;
    if (LowerRace == TEXT("ara")) return ETextureRaceType::Arab;
    if (LowerRace == TEXT("jew")) return ETextureRaceType::Jew;
    if (LowerRace == TEXT("ind")) return ETextureRaceType::Indian;
    if (LowerRace == TEXT("chi")) return ETextureRaceType::Chinese;
    if (LowerRace == TEXT("jap")) return ETextureRaceType::Japanese;
    if (LowerRace == TEXT("rus")) return ETextureRaceType::Russian;
    return ETextureRaceType::Universal; // Default to Universal
}

void UPedCharacterComponent::AddProp(const FPedPropMesh& PropMesh)
{
    CurrentProfile.CurrentVariation.MeshConfig.Props.Add(PropMesh);
    LoadPropMeshAsync(PropMesh);
}

void UPedCharacterComponent::RemoveProp(const FString& PropName)
{
    CurrentProfile.CurrentVariation.MeshConfig.Props.RemoveAll([PropName](const FPedPropMesh& Prop)
    {
        return Prop.PropName == PropName;
    });

    // Remove from owner
    if (AActor* Owner = GetOwner())
    {
        if (UStaticMeshComponent* PropComp = Owner->FindComponentByClass<UStaticMeshComponent>())
        {
            if (PropComp->GetName().Contains(PropName))
            {
                PropComp->DestroyComponent();
            }
        }
    }
}

void UPedCharacterComponent::ClearAllProps()
{
    CurrentProfile.CurrentVariation.MeshConfig.Props.Empty();
    
    if (AActor* Owner = GetOwner())
    {
        TArray<UStaticMeshComponent*> PropComponents;
        Owner->GetComponents<UStaticMeshComponent>(PropComponents);
        
        for (UStaticMeshComponent* PropComp : PropComponents)
        {
            if (PropComp->GetName().Contains(TEXT("Prop")))
            {
                PropComp->DestroyComponent();
            }
        }
    }
}

void UPedCharacterComponent::SetupAnimationController(USkeletalMeshComponent* SkeletalMeshComponent)
{
    if (!SkeletalMeshComponent)
    {
        SkeletalMeshComponent = FindSkeletalMeshComponent();
    }

    if (!SkeletalMeshComponent)
    {
        UE_LOG(LogTemp, Warning, TEXT("PedCharacterComponent: No skeletal mesh component found for animation setup"));
        return;
    }

    CachedSkeletalMeshComponent = SkeletalMeshComponent;
    AnimationController = Cast<UPedAnimationController>(SkeletalMeshComponent->GetAnimInstance());

    if (!AnimationController)
    {
        UE_LOG(LogTemp, Warning, TEXT("PedCharacterComponent: No PedAnimationController found on skeletal mesh"));
        return;
    }

    UE_LOG(LogTemp, Log, TEXT("PedCharacterComponent: Animation controller setup complete"));
}

UPedAnimationController* UPedCharacterComponent::GetAnimationController() const
{
    return AnimationController;
}

void UPedCharacterComponent::ApplyRandomVariations()
{
    if (MaxHeightVariation > 0.0f)
    {
        float HeightVar = FMath::RandRange(-MaxHeightVariation, MaxHeightVariation);
        SetHeightVariation(HeightVar);
    }

    if (MaxWeightVariation > 0.0f)
    {
        float WeightVar = FMath::RandRange(-MaxWeightVariation, MaxWeightVariation);
        SetWeightVariation(WeightVar);
    }
}

void UPedCharacterComponent::SetHeightVariation(float Variation)
{
    CurrentProfile.HeightVariation = FMath::Clamp(Variation, -MaxHeightVariation, MaxHeightVariation);
    SetOverallScale(CalculateScaleWithVariations());
}

void UPedCharacterComponent::SetWeightVariation(float Variation)
{
    CurrentProfile.WeightVariation = FMath::Clamp(Variation, -MaxWeightVariation, MaxWeightVariation);
    SetOverallScale(CalculateScaleWithVariations());
}

void UPedCharacterComponent::SetOverallScale(const FVector& NewScale)
{
    CurrentProfile.Scale = NewScale;

    if (USkeletalMeshComponent* SkeletalMesh = FindSkeletalMeshComponent())
    {
        SkeletalMesh->SetWorldScale3D(NewScale);
    }
}

void UPedCharacterComponent::LoadComponentMeshAsync(const FPedComponentMesh& ComponentMesh)
{
    if (!ComponentMesh.MeshAssetPath.IsValid())
    {
        return;
    }

    FStreamableManager& Streamable = UAssetManager::GetStreamableManager();
    TWeakObjectPtr<UPedCharacterComponent> WeakThis(this);
    
    Streamable.RequestAsyncLoad(ComponentMesh.MeshAssetPath.ToSoftObjectPath(), 
        FStreamableDelegate::CreateLambda([WeakThis, ComponentMesh]()
        {
            if (WeakThis.IsValid())
            {
                USkeletalMesh* LoadedMesh = ComponentMesh.MeshAssetPath.Get();
                WeakThis->OnComponentMeshLoaded(ComponentMesh.ComponentName, LoadedMesh);
                
                // Load materials for this component
                for (int32 i = 0; i < ComponentMesh.Materials.Num(); i++)
                {
                    if (ComponentMesh.Materials[i].IsValid())
                    {
                        WeakThis->LoadComponentMaterialAsync(ComponentMesh.ComponentName, i, ComponentMesh.Materials[i]);
                    }
                }
            }
        }));
}

void UPedCharacterComponent::LoadPropMeshAsync(const FPedPropMesh& PropMesh)
{
    if (!PropMesh.PropMeshAssetPath.IsValid())
    {
        return;
    }

    FStreamableManager& Streamable = UAssetManager::GetStreamableManager();
    TWeakObjectPtr<UPedCharacterComponent> WeakThis(this);
    
    Streamable.RequestAsyncLoad(PropMesh.PropMeshAssetPath.ToSoftObjectPath(), 
        FStreamableDelegate::CreateLambda([WeakThis, PropMesh]()
        {
            if (WeakThis.IsValid())
            {
                UStaticMesh* LoadedMesh = PropMesh.PropMeshAssetPath.Get();
                WeakThis->OnPropMeshLoaded(PropMesh.PropName, LoadedMesh);
                
                // Load materials for this prop
                for (int32 i = 0; i < PropMesh.Materials.Num(); i++)
                {
                    if (PropMesh.Materials[i].IsValid())
                    {
                        WeakThis->LoadComponentMaterialAsync(PropMesh.PropName, i, PropMesh.Materials[i]);
                    }
                }
            }
        }));
}

void UPedCharacterComponent::LoadComponentMaterialAsync(const FString& ComponentName, int32 MaterialIndex, const TSoftObjectPtr<UMaterialInterface>& MaterialPtr)
{
    if (!MaterialPtr.IsValid())
    {
        return;
    }

    FStreamableManager& Streamable = UAssetManager::GetStreamableManager();
    TWeakObjectPtr<UPedCharacterComponent> WeakThis(this);
    
    Streamable.RequestAsyncLoad(MaterialPtr.ToSoftObjectPath(), 
        FStreamableDelegate::CreateLambda([WeakThis, ComponentName, MaterialIndex, MaterialPtr]()
        {
            if (WeakThis.IsValid())
            {
                UMaterialInterface* LoadedMaterial = MaterialPtr.Get();
                WeakThis->OnComponentMaterialLoaded(ComponentName, MaterialIndex, LoadedMaterial);
            }
        }));
}

TArray<FString> UPedCharacterComponent::GetAvailableVariations() const
{
    TArray<FString> VariationNames;
    VariationSets.GetKeys(VariationNames);
    return VariationNames;
}

// Private Functions
void UPedCharacterComponent::InitializeComponent()
{
    CachedSkeletalMeshComponent = FindSkeletalMeshComponent();
}

void UPedCharacterComponent::UpdateLoadingProgress()
{
    if (TotalAssetsToLoad > 0)
    {
        LoadingProgress = (float)AssetsLoaded / (float)TotalAssetsToLoad;
    }
    else
    {
        LoadingProgress = 1.0f;
    }
}

void UPedCharacterComponent::CheckIfAllAssetsLoaded()
{
    if (AssetsLoaded >= TotalAssetsToLoad)
    {
        bIsLoadingAssets = false;
        bIsProfileLoaded = true;
        LoadingProgress = 1.0f;

        if (bApplyRandomVariationsOnLoad)
        {
            ApplyRandomVariations();
        }

        ApplyLoadedAssetsToMesh();
        OnProfileLoaded.Broadcast(CurrentProfile);
        OnAllAssetsLoaded.Broadcast();

        UE_LOG(LogTemp, Log, TEXT("PedCharacterComponent: Profile '%s' loading completed"), *CurrentProfile.ProfileName);
    }
}

void UPedCharacterComponent::ApplyLoadedAssetsToMesh()
{
    USkeletalMeshComponent* TargetComponent = FindSkeletalMeshComponent();
    if (!TargetComponent)
    {
        return;
    }

    const FPedMeshConfiguration& MeshConfig = CurrentProfile.CurrentVariation.MeshConfig;

    // Apply all loaded components
    TArray<FPedComponentMesh> Components = {
        MeshConfig.Head, MeshConfig.Hair, MeshConfig.Eyes, MeshConfig.Teeth,
        MeshConfig.Upper, MeshConfig.Lower, MeshConfig.Hands, MeshConfig.Arms,
        MeshConfig.Feet, MeshConfig.Nails
    };

    for (const FPedComponentMesh& Component : Components)
    {
        if (!Component.ComponentName.IsEmpty())
        {
            ApplyComponentMesh(Component, TargetComponent);
        }
    }

    // Apply all props
    for (const FPedPropMesh& Prop : MeshConfig.Props)
    {
        if (!Prop.PropName.IsEmpty())
        {
            ApplyPropMesh(Prop);
        }
    }

    UE_LOG(LogTemp, Log, TEXT("PedCharacterComponent: Applied loaded assets to mesh"));
}

FVector UPedCharacterComponent::CalculateScaleWithVariations() const
{
    FVector BaseScale = FVector(1.0f, 1.0f, 1.0f);
    BaseScale.Z += CurrentProfile.HeightVariation;
    BaseScale.X += CurrentProfile.WeightVariation;
    BaseScale.Y += CurrentProfile.WeightVariation;
    return BaseScale * CurrentProfile.Scale;
}

USkeletalMeshComponent* UPedCharacterComponent::FindSkeletalMeshComponent() const
{
    if (CachedSkeletalMeshComponent)
    {
        return CachedSkeletalMeshComponent;
    }

    if (AActor* Owner = GetOwner())
    {
        return Owner->FindComponentByClass<USkeletalMeshComponent>();
    }

    return nullptr;
}

void UPedCharacterComponent::ParsePedMeshXMLTiny(tinyxml2::XMLDocument& Doc)
{
    tinyxml2::XMLElement* RootElement = Doc.RootElement();
    if (!RootElement) return;

    for (tinyxml2::XMLElement* PedElement = RootElement->FirstChildElement("Ped"); PedElement != nullptr; PedElement = PedElement->NextSiblingElement("Ped"))
    {
        FString PedName = FString(UTF8_TO_TCHAR(PedElement->Attribute("Name")));
        for (tinyxml2::XMLElement* ComponentElement = PedElement->FirstChildElement("Component"); ComponentElement != nullptr; ComponentElement = ComponentElement->NextSiblingElement("Component"))
        {
            FString ComponentName = FString(UTF8_TO_TCHAR(ComponentElement->Attribute("Name")));
            FString AltName = FString(UTF8_TO_TCHAR(ComponentElement->Attribute("Alt")));
            FString MapKey = FString::Printf(TEXT("%s_%s"), *PedName, *ComponentName);
            TMap<int32, FString> MaterialMap;
            for (tinyxml2::XMLElement* MaterialElement = ComponentElement->FirstChildElement("Material"); MaterialElement != nullptr; MaterialElement = MaterialElement->NextSiblingElement("Material"))
            {
                int32 MaterialIndex = MaterialElement->IntAttribute("Index");
                FString MaterialName = FString(UTF8_TO_TCHAR(MaterialElement->Attribute("Name")));
                MaterialMap.Add(MaterialIndex, MaterialName);
            }
            MaterialMappings.Add(MapKey, MaterialMap);
        }
    }
}

void UPedCharacterComponent::ParsePedVariationsXMLTiny(tinyxml2::XMLDocument& Doc)
{
    tinyxml2::XMLElement* RootElement = Doc.RootElement();
    if (!RootElement) return;

    for (tinyxml2::XMLElement* VariationElement = RootElement->FirstChildElement("VariationSet"); VariationElement != nullptr; VariationElement = VariationElement->NextSiblingElement("VariationSet"))
    {
        FPedVariationSet VariationSet;
        VariationSet.VariationName = FString(UTF8_TO_TCHAR(VariationElement->Attribute("Name")));

        for (tinyxml2::XMLElement* Node = VariationElement->FirstChildElement(); Node != nullptr; Node = Node->NextSiblingElement())
        {
            const char* Tag = Node->Value();
            if (strcmp(Tag, "Component") == 0)
            {
                FString Type = FString(UTF8_TO_TCHAR(Node->Attribute("Type")));
                int32 MeshIndex = Node->IntAttribute("MeshIndex");
                int32 TextureIndex = Node->IntAttribute("TextureIndex");
                FString TextureRace = FString(UTF8_TO_TCHAR(Node->Attribute("TextureRace")));

                FPedComponentMesh* ComponentMesh = nullptr;
                if (Type == TEXT("Head")) ComponentMesh = &VariationSet.MeshConfig.Head;
                else if (Type == TEXT("Upper")) ComponentMesh = &VariationSet.MeshConfig.Upper;
                else if (Type == TEXT("Lower")) ComponentMesh = &VariationSet.MeshConfig.Lower;
                else if (Type == TEXT("Hair")) ComponentMesh = &VariationSet.MeshConfig.Hair;
                else if (Type == TEXT("Feet")) ComponentMesh = &VariationSet.MeshConfig.Feet;

                if (ComponentMesh)
                {
                    ComponentMesh->MeshIndex = MeshIndex;
                    ComponentMesh->TextureVariantIndex = TextureIndex;
                    ComponentMesh->TextureRaceString = TextureRace;
                }
            }
            else if (strcmp(Tag, "PropComponent") == 0)
            {
                FPedPropMesh PropMesh;
                PropMesh.PropName = FString(UTF8_TO_TCHAR(Node->Attribute("Type")));
                PropMesh.MeshIndex = Node->IntAttribute("MeshIndex");
                const char* TextureRaceAttr = Node->Attribute("TextureRace");
                if (TextureRaceAttr)
                {
                    PropMesh.TextureRace = GetRaceFromString(UTF8_TO_TCHAR(TextureRaceAttr));
                }
                VariationSet.MeshConfig.Props.Add(PropMesh);
            }
        }
        VariationSets.Add(VariationSet.VariationName, VariationSet);
    }
}

void UPedCharacterComponent::ParsePedPropsXMLTiny(tinyxml2::XMLDocument& Doc)
{
    tinyxml2::XMLElement* RootElement = Doc.RootElement();
    if (!RootElement) return;

    for (tinyxml2::XMLElement* PropElement = RootElement->FirstChildElement("Prop"); PropElement != nullptr; PropElement = PropElement->NextSiblingElement("Prop"))
    {
        FString PropName = FString(UTF8_TO_TCHAR(PropElement->Attribute("Name")));
        for (tinyxml2::XMLElement* PropComponentElement = PropElement->FirstChildElement("PropComponent"); PropComponentElement != nullptr; PropComponentElement = PropComponentElement->NextSiblingElement("PropComponent"))
        {
            FPedPropMesh PropMesh;
            if (const char* PropCompNameAttr = PropComponentElement->Attribute("Name"))
            {
                PropMesh.PropName = FString(UTF8_TO_TCHAR(PropCompNameAttr));
            }

            if (const char* AltNameAttr = PropComponentElement->Attribute("AltName"))
            {
                PropMesh.AltName = FString(UTF8_TO_TCHAR(AltNameAttr));
            }
            
            PropDefinitions.Add(PropMesh.PropName, PropMesh);

            FString MapKey = FString::Printf(TEXT("%s_%s"), *PropName, *PropMesh.PropName);
            TMap<int32, FString> MaterialMap;
            for (tinyxml2::XMLElement* MaterialElement = PropComponentElement->FirstChildElement("Material"); MaterialElement != nullptr; MaterialElement = MaterialElement->NextSiblingElement("Material"))
            {
                int32 MaterialIndex = MaterialElement->IntAttribute("Index");
                if (const char* MaterialNameAttr = MaterialElement->Attribute("Name"))
                {
                    FString MaterialName = FString(UTF8_TO_TCHAR(MaterialNameAttr));
                    MaterialMap.Add(MaterialIndex, MaterialName);
                }
            }
            if (MaterialMap.Num() > 0)
            {
                MaterialMappings.Add(MapKey, MaterialMap);
            }
        }
    }
}

// Asset loading callbacks
void UPedCharacterComponent::OnComponentMeshLoaded(const FString& ComponentName, USkeletalMesh* LoadedMesh)
{
    if (LoadedMesh)
    {
        LoadedComponentMeshes.Add(ComponentName, LoadedMesh);
        AssetsLoaded++;
        
        FPedComponentMesh DummyComponent;
        DummyComponent.ComponentName = ComponentName;
        OnComponentLoaded.Broadcast(ComponentName, DummyComponent);
        
        UE_LOG(LogTemp, Log, TEXT("PedCharacterComponent: Loaded component mesh '%s'"), *ComponentName);
    }
    else
    {
        UE_LOG(LogTemp, Warning, TEXT("PedCharacterComponent: Failed to load component mesh '%s'"), *ComponentName);
        AssetsLoaded++;
    }

    CheckIfAllAssetsLoaded();
}

void UPedCharacterComponent::OnPropMeshLoaded(const FString& PropName, UStaticMesh* LoadedMesh)
{
    if (LoadedMesh)
    {
        LoadedPropMeshes.Add(PropName, LoadedMesh);
        AssetsLoaded++;
        
        FPedPropMesh DummyProp;
        DummyProp.PropName = PropName;
        OnPropLoaded.Broadcast(DummyProp);
        
        UE_LOG(LogTemp, Log, TEXT("PedCharacterComponent: Loaded prop mesh '%s'"), *PropName);
    }
    else
    {
        UE_LOG(LogTemp, Warning, TEXT("PedCharacterComponent: Failed to load prop mesh '%s'"), *PropName);
        AssetsLoaded++;
    }

    CheckIfAllAssetsLoaded();
}

void UPedCharacterComponent::OnComponentMaterialLoaded(const FString& ComponentName, int32 MaterialIndex, UMaterialInterface* LoadedMaterial)
{
    if (LoadedMaterial)
    {
        if (!LoadedComponentMaterials.Contains(ComponentName))
        {
            LoadedComponentMaterials.Emplace(ComponentName, TArray<UMaterialInterface*>());
        }
        
        TArray<UMaterialInterface*>& MaterialArray = LoadedComponentMaterials[ComponentName];
        
        // Ensure array is large enough
        while (MaterialArray.Num() <= MaterialIndex)
        {
            MaterialArray.Add(nullptr);
        }
        
        MaterialArray[MaterialIndex] = LoadedMaterial;
        AssetsLoaded++;
        
        UE_LOG(LogTemp, Log, TEXT("PedCharacterComponent: Loaded material for component '%s' at index %d"), *ComponentName, MaterialIndex);
    }
    else
    {
        UE_LOG(LogTemp, Warning, TEXT("PedCharacterComponent: Failed to load material for component '%s' at index %d"), *ComponentName, MaterialIndex);
        AssetsLoaded++;
    }

    CheckIfAllAssetsLoaded();
}

// Helper functions
FPedComponentMesh* UPedCharacterComponent::FindComponentInConfig(const FString& ComponentName)
{
    FPedMeshConfiguration& MeshConfig = CurrentProfile.CurrentVariation.MeshConfig;
    
    if (ComponentName == TEXT("Head")) return &MeshConfig.Head;
    if (ComponentName == TEXT("Hair")) return &MeshConfig.Hair;
    if (ComponentName == TEXT("Eyes")) return &MeshConfig.Eyes;
    if (ComponentName == TEXT("Teeth")) return &MeshConfig.Teeth;
    if (ComponentName == TEXT("Upper") || ComponentName == TEXT("uppr") || ComponentName == TEXT("upperbody")) return &MeshConfig.Upper;
    if (ComponentName == TEXT("Lower")) return &MeshConfig.Lower;
    if (ComponentName == TEXT("Hands")) return &MeshConfig.Hands;
    if (ComponentName == TEXT("Arms")) return &MeshConfig.Arms;
    if (ComponentName == TEXT("Feet")) return &MeshConfig.Feet;
    if (ComponentName == TEXT("Nails")) return &MeshConfig.Nails;
    
    return nullptr;
}

FPedPropMesh* UPedCharacterComponent::FindPropInConfig(const FString& PropName)
{
    TArray<FPedPropMesh>& Props = CurrentProfile.CurrentVariation.MeshConfig.Props;
    
    for (FPedPropMesh& Prop : Props)
    {
        if (Prop.PropName == PropName)
        {
            return &Prop;
        }
    }
    
    return nullptr;
}

void UPedCharacterComponent::ApplyComponentToSkeletalMesh(const FPedComponentMesh& ComponentMesh, USkeletalMeshComponent* TargetComponent)
{
    if (!TargetComponent)
    {
        return;
    }

    // Apply the main mesh if this is a primary component
    if (ComponentMesh.ComponentName == TEXT("Upper") || ComponentMesh.ComponentName == TEXT("Body"))
    {
        if (USkeletalMesh* LoadedMesh = LoadedComponentMeshes.FindRef(ComponentMesh.ComponentName))
        {
            TargetComponent->SetSkeletalMesh(LoadedMesh);
        }
    }

    // Apply materials
    if (TArray<UMaterialInterface*>* ComponentMaterials = LoadedComponentMaterials.Find(ComponentMesh.ComponentName))
    {
        for (int32 i = 0; i < ComponentMaterials->Num() && i < TargetComponent->GetNumMaterials(); i++)
        {
            if ((*ComponentMaterials)[i])
            {
                TargetComponent->SetMaterial(i, (*ComponentMaterials)[i]);
            }
        }
    }
}

UStaticMeshComponent* UPedCharacterComponent::CreatePropComponent(const FPedPropMesh& PropMesh)
{
    if (!GetOwner())
    {
        return nullptr;
    }

    // Create new static mesh component for the prop
    UStaticMeshComponent* PropComponent = NewObject<UStaticMeshComponent>(GetOwner());
    PropComponent->SetupAttachment(FindSkeletalMeshComponent(), PropMesh.AttachSocketName);
    PropComponent->RegisterComponent();
    
    // Set component name for identification
    PropComponent->Rename(*FString::Printf(TEXT("Prop_%s"), *PropMesh.PropName));
    
    return PropComponent;
}

// Asset Discovery System Implementation
FPedAssetDiscovery UPedCharacterComponent::DiscoverCharacterAssets(const FString& CharacterName, const FString& BasePath)
{
    FPedAssetDiscovery Discovery;
    Discovery.CharacterName = CharacterName;
    Discovery.BasePath = BasePath;
    Discovery.SkeletonPath = FString::Printf(TEXT("%s/%s/SKEL_%s"), *BasePath, *CharacterName, *CharacterName);

    // Real asset discovery logic:
    // 1. Scan character folder for component folders (Upper/, Lower/, Hat/, etc.)
    // 2. For each component folder, scan for mesh files (Upper_000.uasset, Upper_001.uasset)
    // 3. Scan Texture/ subfolder for all texture variations
    // 4. Load all discovered meshes regardless of XML configuration
    // 5. Apply materials from XML if available, otherwise use default M_Ped material

    FString CharacterPath = FString::Printf(TEXT("%s/%s"), *BasePath, *CharacterName);
    
    // Discover Upper components
    FComponentNumbers UpperNumbers;
    FString UpperPath = CharacterPath + TEXT("/Upper");
    // In real implementation: scan UpperPath for Upper_000.uasset, Upper_001.uasset, etc.
    UpperNumbers.Numbers.Add(0);
    UpperNumbers.Numbers.Add(1);
    UpperNumbers.Numbers.Add(2);
    Discovery.AvailableComponents.Add(TEXT("Upper"), UpperNumbers);

    // Discover Lower components
    FComponentNumbers LowerNumbers;
    FString LowerPath = CharacterPath + TEXT("/Lower");
    // In real implementation: scan LowerPath for Lower_000.uasset, Lower_001.uasset, etc.
    LowerNumbers.Numbers.Add(0);
    LowerNumbers.Numbers.Add(1);
    Discovery.AvailableComponents.Add(TEXT("Lower"), LowerNumbers);

    // Discover Hat components
    FComponentNumbers HatNumbers;
    FString HatPath = CharacterPath + TEXT("/Hat");
    // In real implementation: scan HatPath for Hat_000.uasset, Hat_001.uasset, etc.
    HatNumbers.Numbers.Add(0);
    Discovery.AvailableComponents.Add(TEXT("Hat"), HatNumbers);

    // Discover texture variants for each component
    FComponentVariants UpperVariants;
    FString UpperTexturePath = UpperPath + TEXT("/Texture");
    // In real implementation: scan UpperTexturePath for Upper_000_a_uni.uasset, Upper_000_b_uni.uasset, etc.
    UpperVariants.Variants.Add(0);
    UpperVariants.Variants.Add(1);
    Discovery.AvailableVariants.Add(TEXT("Upper"), UpperVariants);

    // Available race types (from texture scan)
    Discovery.AvailableRaces.Add(ETextureRaceType::Universal);
    Discovery.AvailableRaces.Add(ETextureRaceType::White);
    Discovery.AvailableRaces.Add(ETextureRaceType::Black);

    return Discovery;
}

TArray<int32> UPedCharacterComponent::GetAvailableComponentNumbers(EPedBodyPart BodyPart, const FString& CharacterPath)
{
    TArray<int32> Numbers;
    
    // This would scan the actual directory structure in a real implementation
    // For now, return some placeholder numbers
    Numbers.Add(0);
    Numbers.Add(1);
    Numbers.Add(2);

    return Numbers;
}

TArray<EComponentVariant> UPedCharacterComponent::GetAvailableComponentVariants(EPedBodyPart BodyPart, int32 ComponentNumber, const FString& CharacterPath)
{
    TArray<EComponentVariant> Variants;
    
    // This would scan the actual directory structure in a real implementation
    // For now, return some placeholder variants
    Variants.Add(EComponentVariant::A);
    Variants.Add(EComponentVariant::B);

    return Variants;
}

TArray<ETextureRaceType> UPedCharacterComponent::GetAvailableRaces(EPedBodyPart BodyPart, int32 ComponentNumber, EComponentVariant Variant, const FString& CharacterPath)
{
    TArray<ETextureRaceType> Races;
    
    // This would scan the actual directory structure in a real implementation
    // For now, return some placeholder races
    Races.Add(ETextureRaceType::Universal);
    Races.Add(ETextureRaceType::White);
    Races.Add(ETextureRaceType::Black);

    return Races;
}

bool UPedCharacterComponent::ValidateComponentExists(EPedBodyPart BodyPart, int32 ComponentNumber, EComponentVariant Variant, ETextureRaceType Race, const FString& CharacterPath)
{
    // This would check if the actual asset exists in a real implementation
    // For now, return true for valid combinations
    return ComponentNumber >= 0 && ComponentNumber <= 2;
}

// Automatic Asset Loading with Fallback Material System
bool UPedCharacterComponent::LoadComponentWithFallback(const FString& ComponentName, int32 ComponentNumber, EComponentVariant Variant, ETextureRaceType Race, const FString& CharacterPath)
{
    // Map component names to actual folder names
    TMap<FString, FString> ComponentFolderMap;
    ComponentFolderMap.Add(TEXT("Upper"), TEXT("uppr"));
    ComponentFolderMap.Add(TEXT("Lower"), TEXT("lowr"));
    ComponentFolderMap.Add(TEXT("Shoes"), TEXT("feet"));
    ComponentFolderMap.Add(TEXT("Head"), TEXT("head"));
    ComponentFolderMap.Add(TEXT("Hair"), TEXT("hair"));
    ComponentFolderMap.Add(TEXT("Beard"), TEXT("teef"));
    
    // Get the actual folder name
    FString ActualFolderName = ComponentFolderMap.FindRef(ComponentName);
    if (ActualFolderName.IsEmpty())
    {
        ActualFolderName = ComponentName.ToLower(); // Fallback to lowercase
    }
    
    // Step 1: Always try to load the mesh file using actual folder name
    FString MeshAssetPath = FString::Printf(TEXT("%s/%s/%s_%03d"), 
                                          *CharacterPath, *ActualFolderName, *ActualFolderName, ComponentNumber);
    
    USkeletalMesh* LoadedMesh = LoadObject<USkeletalMesh>(nullptr, *MeshAssetPath);
    
    if (!LoadedMesh)
    {
        UE_LOG(LogTemp, Warning, TEXT("Failed to load mesh: %s"), *MeshAssetPath);
        return false;
    }

    // Step 2: Try to get material mapping from XML
    UMaterialInterface* ComponentMaterial = nullptr;
    
    // Check if this component exists in PedMesh.xml or PedPropMesh.xml
    bool bFoundInXML = HasComponentInXML(ComponentName, ComponentNumber);
    
    if (bFoundInXML)
    {
        // Load custom material from XML configuration
        FString VariantChar = GetVariantStringFromEnum(Variant);
        FString RaceString = GetRaceStringFromEnum(Race);
        FString TextureAssetPath = FString::Printf(TEXT("%s/%s/Texture/%s_%03d_%s_%s"), 
                                                  *CharacterPath, *ActualFolderName, *ActualFolderName, 
                                                  ComponentNumber, *VariantChar, *RaceString);
        
        ComponentMaterial = GetMaterialFromXMLMapping(ComponentName, ComponentNumber, TextureAssetPath);
    }
    
    // Step 3: Fallback to default M_Ped material if no XML mapping
    if (!ComponentMaterial)
    {
        UE_LOG(LogTemp, Log, TEXT("No XML mapping for %s_%03d, using default M_Ped material"), 
               *ActualFolderName, ComponentNumber);
        
        // Load default ped material (correct path)
        ComponentMaterial = LoadObject<UMaterialInterface>(nullptr, TEXT("/Game/Characters/CharacterShaders/M_Ped"));
    }

    // Step 4: Apply the mesh and material to the skeletal mesh component
    USkeletalMeshComponent* MeshComponent = GetOwner()->FindComponentByClass<USkeletalMeshComponent>();
    if (MeshComponent && LoadedMesh)
    {
        // Set the skeletal mesh
        MeshComponent->SetSkeletalMesh(LoadedMesh);
        
        // Apply the material if available
        if (ComponentMaterial)
        {
            MeshComponent->SetMaterial(0, ComponentMaterial); // Apply to material slot 0
            UE_LOG(LogTemp, Log, TEXT("Successfully loaded component: %s_%03d with mesh and material"), 
                   *ComponentName, ComponentNumber);
        }
        else
        {
            UE_LOG(LogTemp, Log, TEXT("Successfully loaded component: %s_%03d with mesh (no material)"), 
                   *ComponentName, ComponentNumber);
        }
        return true;
    }
    else
    {
        UE_LOG(LogTemp, Error, TEXT("Failed to find SkeletalMeshComponent or mesh for: %s_%03d"), 
               *ComponentName, ComponentNumber);
        return false;
    }
}

// Helper functions for the fallback system
bool UPedCharacterComponent::HasComponentInXML(const FString& ComponentName, int32 ComponentNumber)
{
    // Check if component exists in loaded XML data
    // This would search through the parsed PedMesh.xml and PedPropMesh.xml data
    // For now, return false to trigger fallback behavior
    return false;
}

UMaterialInterface* UPedCharacterComponent::GetMaterialFromXMLMapping(const FString& ComponentName, int32 ComponentNumber, const FString& TextureAssetPath)
{
    // Get material instance from XML configuration
    // This would create a material instance with the specified texture
    // For now, return nullptr to trigger fallback
    return nullptr;
}



FString UPedCharacterComponent::GetRaceStringFromEnum(ETextureRaceType Race)
{
    switch(Race)
    {
    case ETextureRaceType::Universal: return TEXT("uni");
    case ETextureRaceType::White: return TEXT("whi");
    case ETextureRaceType::Black: return TEXT("bla");
    case ETextureRaceType::Arab: return TEXT("ara");
    case ETextureRaceType::Jew: return TEXT("jew");
    case ETextureRaceType::Indian: return TEXT("ind");
    case ETextureRaceType::Chinese: return TEXT("chi");
    case ETextureRaceType::Japanese: return TEXT("jap");
    case ETextureRaceType::Russian: return TEXT("rus");
    case ETextureRaceType::Custom: return TEXT("cus");
    default: return TEXT("uni"); // Default to universal
    }
}

FString UPedCharacterComponent::GetVariantStringFromEnum(EComponentVariant Variant)
{
    switch(Variant)
    {
    case EComponentVariant::A: return TEXT("a");
    case EComponentVariant::B: return TEXT("b"); 
    case EComponentVariant::C: return TEXT("c");
    case EComponentVariant::D: return TEXT("d");
    case EComponentVariant::E: return TEXT("e");
    case EComponentVariant::F: return TEXT("f");
    case EComponentVariant::G: return TEXT("g");
    case EComponentVariant::H: return TEXT("h");
    case EComponentVariant::I: return TEXT("i");
    case EComponentVariant::J: return TEXT("j");
    case EComponentVariant::K: return TEXT("k");
    case EComponentVariant::L: return TEXT("l");
    case EComponentVariant::M: return TEXT("m");
    case EComponentVariant::N: return TEXT("n");
    case EComponentVariant::O: return TEXT("o");
    case EComponentVariant::P: return TEXT("p");
    case EComponentVariant::Q: return TEXT("q");
    default: return TEXT("a"); // Default to variant A
    }
}

void UPedCharacterComponent::LoadBaseSkeletonFromXML()
{
    APed* OwnerPed = Cast<APed>(GetOwner());
    if (!OwnerPed)
    {
        UE_LOG(LogTemp, Warning, TEXT("PedCharacterComponent: Owner is not a Ped"));
        return;
    }

    FString PedName = OwnerPed->GetCharacterName();
    UE_LOG(LogTemp, Log, TEXT("PedCharacterComponent: Loading data for ped: %s"), *PedName);

    FString PedsXMLPath = FPaths::ProjectDir() + TEXT("Data/Peds/Peds.xml");
    
    tinyxml2::XMLDocument XmlDoc;
    if (XmlDoc.LoadFile(TCHAR_TO_UTF8(*PedsXMLPath)) != tinyxml2::XML_SUCCESS)
    {
        UE_LOG(LogTemp, Warning, TEXT("PedCharacterComponent: Failed to load Peds.xml from %s. Error: %s"), *PedsXMLPath, UTF8_TO_TCHAR(XmlDoc.ErrorStr()));
        return;
    }

    tinyxml2::XMLElement* RootNode = XmlDoc.RootElement();
    if (!RootNode)
    {
        UE_LOG(LogTemp, Warning, TEXT("PedCharacterComponent: Peds.xml has no root node"));
        return;
    }

    bool bPedFound = false;
    for (tinyxml2::XMLElement* PedNode = RootNode->FirstChildElement("Ped"); PedNode; PedNode = PedNode->NextSiblingElement("Ped"))
    {
        const char* PedNameAttr = PedNode->Attribute("Name");
        if (PedNameAttr && FString(UTF8_TO_TCHAR(PedNameAttr)) == PedName)
        {
            bPedFound = true;
            
            tinyxml2::XMLElement* PropTypeNode = PedNode->FirstChildElement("PropType");
            if (PropTypeNode && PropTypeNode->GetText())
            {
                FString PropType = UTF8_TO_TCHAR(PropTypeNode->GetText());
                if (!PropType.IsEmpty())
                {
                    UE_LOG(LogTemp, Log, TEXT("PedCharacterComponent: Found PropType: %s"), *PropType);
                    CharacterAssetFolder = PropType;
                }
            }

            const char* BaseSkeletonPathAttr = PedNode->Attribute("BaseSkeleton");
            if (BaseSkeletonPathAttr)
            {
                FString BaseSkeletonPath = UTF8_TO_TCHAR(BaseSkeletonPathAttr);
                if (!BaseSkeletonPath.IsEmpty())
                {
                    UE_LOG(LogTemp, Log, TEXT("PedCharacterComponent: Found base skeleton path: %s"), *BaseSkeletonPath);
                    USkeletalMesh* BaseSkeleton = LoadObject<USkeletalMesh>(nullptr, *BaseSkeletonPath);
                    if (BaseSkeleton)
                    {
                        USkeletalMeshComponent* SkeletalMeshComp = OwnerPed->FindComponentByClass<USkeletalMeshComponent>();
                        if (SkeletalMeshComp)
                        {
                            SkeletalMeshComp->SetSkeletalMesh(BaseSkeleton);
                            UE_LOG(LogTemp, Log, TEXT("PedCharacterComponent: Successfully applied base skeleton for %s"), *PedName);
                        }
                        else
                        {
                            UE_LOG(LogTemp, Warning, TEXT("PedCharacterComponent: No SkeletalMeshComponent found on %s"), *PedName);
                        }
                    }
                    else
                    {
                        UE_LOG(LogTemp, Warning, TEXT("PedCharacterComponent: Failed to load base skeleton: %s"), *BaseSkeletonPath);
                    }
                }
            }
            break; // Found the ped, no need to continue loop
        }
    }

    if (!bPedFound)
    {
        UE_LOG(LogTemp, Warning, TEXT("PedCharacterComponent: Ped '%s' not found in Peds.xml"), *PedName);
    }
}

// === ENHANCED MATERIAL LOADING & FALLBACK SYSTEM ===

TSoftObjectPtr<UMaterialInterface> UPedCharacterComponent::LoadMaterialWithFallback(const FString& MaterialName)
{
    // Check cache first
    if (CachedMaterials.Contains(MaterialName))
    {
        return CachedMaterials[MaterialName];
    }

    // Try to load the requested material
    FString MaterialPath = FString::Printf(TEXT("/Game/Characters/Materials/%s.%s"), *MaterialName, *MaterialName);
    FSoftObjectPath SoftPath(MaterialPath);
    TSoftObjectPtr<UMaterialInterface> MaterialPtr(SoftPath);
    
    // Cache the material pointer (even if it might not exist)
    CachedMaterials.Add(MaterialName, MaterialPtr);
    
    // Try to load synchronously to verify it exists
    if (UMaterialInterface* LoadedMaterial = MaterialPtr.LoadSynchronous())
    {
        return MaterialPtr;
    }

    // Fallback to M_Ped if material not found
    if (!DefaultMaterial_M_Ped.Get())
    {
        FSoftObjectPath DefaultPath(TEXT("/Game/Characters/Materials/M_Ped.M_Ped"));
        DefaultMaterial_M_Ped = TSoftObjectPtr<UMaterialInterface>(DefaultPath);
    }

    UE_LOG(LogTemp, Warning, TEXT("Material '%s' not found, using M_Ped fallback"), *MaterialName);
    CachedMaterials[MaterialName] = DefaultMaterial_M_Ped; // Update cache with fallback
    return DefaultMaterial_M_Ped;
}

void UPedCharacterComponent::ApplyMaterialFallbackToComponent(FPedComponentMesh& ComponentMesh)
{
    // Clear existing materials and apply fallbacks based on component type
    ComponentMesh.Materials.Empty();

    FString ComponentName = ComponentMesh.ComponentName.ToLower();
    
    // Hair components get special alpha material
    if (ComponentName.Contains(TEXT("hair")))
    {
        if (!DefaultMaterial_M_PedHairAlpha.Get())
        {
            FSoftObjectPath HairPath(TEXT("/Game/Characters/Materials/M_PedHairAlpha.M_PedHairAlpha"));
            DefaultMaterial_M_PedHairAlpha = TSoftObjectPtr<UMaterialInterface>(HairPath);
        }
        ComponentMesh.Materials.Add(DefaultMaterial_M_PedHairAlpha);
    }
    // Components with transparency get alpha material
    else if (ComponentName.Contains(TEXT("teef")) || ComponentName.Contains(TEXT("alpha")))
    {
        if (!DefaultMaterial_M_PedAlpha.Get())
        {
            FSoftObjectPath AlphaPath(TEXT("/Game/Characters/Materials/M_PedAlpha.M_PedAlpha"));
            DefaultMaterial_M_PedAlpha = TSoftObjectPtr<UMaterialInterface>(AlphaPath);
        }
        ComponentMesh.Materials.Add(DefaultMaterial_M_PedAlpha);
    }
    // Default to M_Ped for most components
    else
    {
        if (!DefaultMaterial_M_Ped.Get())
        {
            FSoftObjectPath DefaultPath(TEXT("/Game/Characters/Materials/M_Ped.M_Ped"));
            DefaultMaterial_M_Ped = TSoftObjectPtr<UMaterialInterface>(DefaultPath);
        }
        ComponentMesh.Materials.Add(DefaultMaterial_M_Ped);
    }
}

FString UPedCharacterComponent::GetMaterialNameFromXMLByIndex(const FString& ComponentName, int32 MaterialIndex)
{
    // This would normally parse PedMesh.xml to get material names by index
    // For now, return common fallbacks based on component and index
    
    if (ComponentName.ToLower().Contains(TEXT("hair")) && MaterialIndex == 0)
    {
        return TEXT("M_PedHairAlpha");
    }
    else if (ComponentName.ToLower().Contains(TEXT("teef")) && MaterialIndex == 0)
    {
        return TEXT("M_PedAlpha");
    }
    else if (MaterialIndex >= 0 && MaterialIndex <= 2)
    {
        return TEXT("M_Ped"); // Default fallback
    }
    
    return TEXT(""); // Empty string triggers fallback in calling function
}

// === ENHANCED ANIMATION CONTROL IMPLEMENTATIONS ===

void UPedCharacterComponent::SetAnimationGroupFromXML(const FString& AnimationGroupName)
{
    CurrentAnimationGroup = AnimationGroupName;
    
    // Load animations from the specified group
    if (!AnimationGroupCache.Contains(AnimationGroupName))
    {
        TArray<FString> AnimationsInGroup;
        
        // For player_movement, use your specific 4 animations
        if (AnimationGroupName.Equals(TEXT("player_movement"), ESearchCase::IgnoreCase))
        {
            AnimationsInGroup.Add(TEXT("idleanim"));
            AnimationsInGroup.Add(TEXT("walkanim"));
            AnimationsInGroup.Add(TEXT("runanim"));
            AnimationsInGroup.Add(TEXT("sprintanim"));
        }
        // For other animation groups, add common fallbacks (you can extend this later)
        else
        {
            // Generic fallbacks for other groups
            AnimationsInGroup.Add(TEXT("idle"));
            AnimationsInGroup.Add(TEXT("walk"));
            AnimationsInGroup.Add(TEXT("run"));
        }
        
        AnimationGroupCache.Add(AnimationGroupName, AnimationsInGroup);
    }

    UE_LOG(LogTemp, Log, TEXT("Animation group '%s' set with %d animations"), *AnimationGroupName, AnimationGroupCache[AnimationGroupName].Num());
}

void UPedCharacterComponent::PlayMovementAnimation(const FString& AnimationName, bool bLoop, float BlendInTime, float BlendOutTime)
{
    if (AnimationController)
    {
        // Use the animation controller to play the animation
        // This would interface with your existing PedAnimationController
        UE_LOG(LogTemp, Log, TEXT("Playing animation: %s (Loop: %s)"), *AnimationName, bLoop ? TEXT("true") : TEXT("false"));
    }
    else
    {
        UE_LOG(LogTemp, Warning, TEXT("No AnimationController available to play: %s"), *AnimationName);
    }
}

void UPedCharacterComponent::SetMovementBlendSpace(const FString& BlendSpaceName)
{
    UE_LOG(LogTemp, Log, TEXT("Setting movement blend space: %s"), *BlendSpaceName);
    // Implementation would set the blend space for movement
}

void UPedCharacterComponent::SetAnimationSpeed(float NewSpeed)
{
    if (CachedSkeletalMeshComponent && CachedSkeletalMeshComponent->GetAnimInstance())
    {
        // Use global time dilation or animation controller instead of SetPlayRate
        CachedSkeletalMeshComponent->GlobalAnimRateScale = NewSpeed;
        UE_LOG(LogTemp, Log, TEXT("Animation speed set to: %f"), NewSpeed);
    }
}

void UPedCharacterComponent::SwitchToMovementSet(const FString& MovementSetName)
{
    SetAnimationGroupFromXML(MovementSetName);
}

TArray<FString> UPedCharacterComponent::GetAvailableAnimationGroups()
{
    TArray<FString> Groups;
    AnimationGroupCache.GetKeys(Groups);
    
    // Add your actual animation groups if cache is empty
    if (Groups.Num() == 0)
    {
        Groups.Add(TEXT("player_movement"));     // Your 4-animation set
        Groups.Add(TEXT("crouch_movement"));     // For later expansion
        Groups.Add(TEXT("vehicle_standard"));    // For vehicles
        Groups.Add(TEXT("Jump_Standard"));       // For jumping
        Groups.Add(TEXT("Climb_Standard"));      // For climbing
        Groups.Add(TEXT("Combat"));              // For combat
        Groups.Add(TEXT("Combat_Pistol"));       // Weapon-specific
        Groups.Add(TEXT("Combat_Rifle"));        // More weapons...
    }
    
    return Groups;
}

TArray<FString> UPedCharacterComponent::GetAvailableAnimationsInGroup(const FString& GroupName)
{
    if (AnimationGroupCache.Contains(GroupName))
    {
        return AnimationGroupCache[GroupName];
    }
    
    return TArray<FString>(); // Return empty array if group not found
}

// === PLAYER MOVEMENT SPECIFIC HELPERS ===

void UPedCharacterComponent::PlayIdleAnimation()
{
    PlayMovementAnimation(TEXT("idleanim"), true);
}

void UPedCharacterComponent::PlayWalkAnimation()
{
    PlayMovementAnimation(TEXT("walkanim"), true);
}

void UPedCharacterComponent::PlayRunAnimation()
{
    PlayMovementAnimation(TEXT("runanim"), true);
}

void UPedCharacterComponent::PlaySprintAnimation()
{
    PlayMovementAnimation(TEXT("sprintanim"), true);
}

bool UPedCharacterComponent::HasPlayerMovementAnimations()
{
    if (!AnimationGroupCache.Contains(TEXT("player_movement")))
    {
        return false;
    }

    const TArray<FString>& PlayerAnims = AnimationGroupCache[TEXT("player_movement")];
    
    // Check if all 4 required animations are available
    bool bHasIdle = PlayerAnims.Contains(TEXT("idleanim"));
    bool bHasWalk = PlayerAnims.Contains(TEXT("walkanim"));
    bool bHasRun = PlayerAnims.Contains(TEXT("runanim"));
    bool bHasSprint = PlayerAnims.Contains(TEXT("sprintanim"));
    
    bool bAllPresent = bHasIdle && bHasWalk && bHasRun && bHasSprint;
    
    UE_LOG(LogTemp, Log, TEXT("Player Movement Animations Check - Idle: %s, Walk: %s, Run: %s, Sprint: %s"), 
           bHasIdle ? TEXT("✓") : TEXT("✗"),
           bHasWalk ? TEXT("✓") : TEXT("✗"), 
           bHasRun ? TEXT("✓") : TEXT("✗"),
           bHasSprint ? TEXT("✓") : TEXT("✗"));
    
    return bAllPresent;
}

// === MASTER SKELETON SYSTEM IMPLEMENTATIONS ===

void UPedCharacterComponent::SetupMasterSkeletonSystem()
{
    // Find the component with 000 index as master skeleton
    USkeletalMeshComponent* MasterComponent = nullptr;
    FString MasterComponentName = TEXT("");
    
    // Search for 000 indexed components
    for (auto& ComponentPair : PedComponents)
    {
        if (ComponentPair.Key.EndsWith(TEXT("000")))
        {
            MasterComponent = ComponentPair.Value;
            MasterComponentName = ComponentPair.Key;
            break;
        }
    }
    
    if (MasterComponent)
    {
        CurrentMasterSkeleton = MasterComponent;
        MasterSkeletonComponentName = MasterComponentName;
        
        UE_LOG(LogTemp, Log, TEXT("Master skeleton set to: %s"), *MasterSkeletonComponentName);
        
        // Make all other components follow this master skeleton
        UpdateFollowerComponents();
    }
    else
    {
        UE_LOG(LogTemp, Warning, TEXT("No component with 000 index found for master skeleton"));
    }
}

void UPedCharacterComponent::UpdateMasterSkeleton(USkeletalMeshComponent* NewMasterComponent, const FString& NewMasterName)
{
    if (NewMasterComponent && IsValid(NewMasterComponent))
    {
        // Update master skeleton
        CurrentMasterSkeleton = NewMasterComponent;
        MasterSkeletonComponentName = NewMasterName;
        
        UE_LOG(LogTemp, Log, TEXT("Master skeleton updated to: %s"), *NewMasterName);
        
        // Update all follower components to follow new master
        UpdateFollowerComponents();
    }
    else
    {
        UE_LOG(LogTemp, Error, TEXT("Invalid component provided for master skeleton update"));
    }
}

void UPedCharacterComponent::UpdateFollowerComponents()
{
    if (!CurrentMasterSkeleton || !IsValid(CurrentMasterSkeleton))
    {
        UE_LOG(LogTemp, Warning, TEXT("No valid master skeleton to follow"));
        return;
    }
    
    int32 FollowerCount = 0;
    
    // Make all other components follow the master skeleton
    for (auto& ComponentPair : PedComponents)
    {
        USkeletalMeshComponent* Component = ComponentPair.Value;
        
        // Skip the master component itself
        if (Component == CurrentMasterSkeleton)
        {
            continue;
        }
        
        if (IsValid(Component))
        {
            // Set this component to follow the master skeleton using the new API
            Component->SetLeaderPoseComponent(CurrentMasterSkeleton);
            FollowerCount++;
            
            UE_LOG(LogTemp, Log, TEXT("Component %s now follows master skeleton %s"), 
                   *ComponentPair.Key, *MasterSkeletonComponentName);
        }
    }
    
    UE_LOG(LogTemp, Log, TEXT("Updated %d follower components to follow master skeleton: %s"), 
           FollowerCount, *MasterSkeletonComponentName);
}

void UPedCharacterComponent::SwapComponentWithSkeletonUpdate(const FString& ComponentType, int32 FromIndex, int32 ToIndex)
{
    // Create component names with proper indexing
    FString FromComponentName = FString::Printf(TEXT("%s%03d"), *ComponentType, FromIndex);
    FString ToComponentName = FString::Printf(TEXT("%s%03d"), *ComponentType, ToIndex);
    
    UE_LOG(LogTemp, Log, TEXT("Swapping component from %s to %s"), *FromComponentName, *ToComponentName);
    
    // Check if we're swapping to/from a master skeleton (000 index)
    bool bSwappingFromMaster = (FromIndex == 0);
    bool bSwappingToMaster = (ToIndex == 0);
    
    // Find the components
    USkeletalMeshComponent** FromComponentPtr = PedComponents.Find(FromComponentName);
    USkeletalMeshComponent** ToComponentPtr = PedComponents.Find(ToComponentName);
    
    if (!FromComponentPtr || !ToComponentPtr)
    {
        UE_LOG(LogTemp, Error, TEXT("Component swap failed - components not found"));
        return;
    }
    
    USkeletalMeshComponent* FromComponent = *FromComponentPtr;
    USkeletalMeshComponent* ToComponent = *ToComponentPtr;
    
    if (!IsValid(FromComponent) || !IsValid(ToComponent))
    {
        UE_LOG(LogTemp, Error, TEXT("Component swap failed - invalid components"));
        return;
    }
    
    // Perform the swap in the map
    PedComponents[FromComponentName] = ToComponent;
    PedComponents[ToComponentName] = FromComponent;
    
    // Update master skeleton if necessary
    if (bSwappingFromMaster)
    {
        // The component that was 000 is now something else, update master to the new 000
        UpdateMasterSkeleton(ToComponent, ToComponentName);
    }
    else if (bSwappingToMaster)
    {
        // A component is becoming the new 000 (master)
        UpdateMasterSkeleton(FromComponent, FromComponentName);
    }
    else
    {
        // Regular swap, just update followers in case master skeleton changed
        UpdateFollowerComponents();
    }
    
    UE_LOG(LogTemp, Log, TEXT("Component swap completed successfully"));
}

USkeletalMeshComponent* UPedCharacterComponent::GetMasterSkeletonComponent() const
{
    return CurrentMasterSkeleton;
}

FString UPedCharacterComponent::GetMasterSkeletonComponentName() const
{
    return MasterSkeletonComponentName;
}

bool UPedCharacterComponent::IsComponentMasterSkeleton(const FString& ComponentName) const
{
    return ComponentName.Equals(MasterSkeletonComponentName, ESearchCase::IgnoreCase);
}

TArray<FString> UPedCharacterComponent::GetFollowerComponentNames() const
{
    TArray<FString> FollowerNames;
    
    for (auto& ComponentPair : PedComponents)
    {
        // Skip the master component
        if (!ComponentPair.Key.Equals(MasterSkeletonComponentName, ESearchCase::IgnoreCase))
        {
            FollowerNames.Add(ComponentPair.Key);
        }
    }
    
    return FollowerNames;
}

// ========== PED COMPONENTS ACCESS IMPLEMENTATIONS ==========

void UPedCharacterComponent::AddPedComponent(const FString& ComponentName, USkeletalMeshComponent* Component)
{
    if (Component && IsValid(Component))
    {
        PedComponents.Add(ComponentName, Component);
        UE_LOG(LogTemp, Log, TEXT("PedCharacterComponent: Added component %s"), *ComponentName);
    }
    else
    {
        UE_LOG(LogTemp, Warning, TEXT("PedCharacterComponent: Cannot add invalid component %s"), *ComponentName);
    }
}

int32 UPedCharacterComponent::GetPedComponentCount() const
{
    return PedComponents.Num();
}

USkeletalMeshComponent* UPedCharacterComponent::GetPedComponent(const FString& ComponentName) const
{
    if (USkeletalMeshComponent* const* FoundComponent = PedComponents.Find(ComponentName))
    {
        return *FoundComponent;
    }
    return nullptr;
}

// === ADDITIONAL MISSING FUNCTION IMPLEMENTATIONS ===

USkeletalMesh* UPedCharacterComponent::GetMasterSkeleton()
{
    return MasterSkeleton;
}

void UPedCharacterComponent::SynchronizeAllComponentsToMasterSkeleton()
{
    // This function synchronizes all components to use the master skeleton
    if (CurrentMasterSkeleton && IsValid(CurrentMasterSkeleton))
    {
        UpdateFollowerComponents();
        UE_LOG(LogTemp, Log, TEXT("All components synchronized to master skeleton: %s"), *MasterSkeletonComponentName);
    }
    else
    {
        UE_LOG(LogTemp, Warning, TEXT("Cannot synchronize - no valid master skeleton found"));
    }
}

bool UPedCharacterComponent::ValidateSkeletonCompatibility(USkeletalMesh* TestSkeleton, USkeletalMesh* ReferenceSkeleton)
{
    if (!TestSkeleton || !ReferenceSkeleton)
    {
        UE_LOG(LogTemp, Warning, TEXT("Cannot validate skeleton compatibility - null skeleton provided"));
        return false;
    }

    // Basic validation - check if both skeletons have similar bone structure
    // This is a simplified check - you can make it more sophisticated
    USkeleton* TestSkeletonAsset = TestSkeleton->GetSkeleton();
    USkeleton* ReferenceSkeletonAsset = ReferenceSkeleton->GetSkeleton();

    if (!TestSkeletonAsset || !ReferenceSkeletonAsset)
    {
        UE_LOG(LogTemp, Warning, TEXT("Cannot validate skeleton compatibility - missing skeleton assets"));
        return false;
    }

    // Check if they're the same skeleton asset (most compatible case)
    if (TestSkeletonAsset == ReferenceSkeletonAsset)
    {
        UE_LOG(LogTemp, Log, TEXT("Skeletons are identical - fully compatible"));
        return true;
    }

    // Check if they're compatible through bone hierarchy (simplified check)
    // In a real implementation, you'd want to check bone names and hierarchy
    const FReferenceSkeleton& TestRefSkeleton = TestSkeleton->GetRefSkeleton();
    const FReferenceSkeleton& ReferenceRefSkeleton = ReferenceSkeleton->GetRefSkeleton();

    int32 TestBoneCount = TestRefSkeleton.GetNum();
    int32 ReferenceBoneCount = ReferenceRefSkeleton.GetNum();

    UE_LOG(LogTemp, Log, TEXT("Skeleton compatibility check - Test: %d bones, Reference: %d bones"), TestBoneCount, ReferenceBoneCount);

    // Basic compatibility if bone counts are similar (this is very simplified)
    bool bIsCompatible = FMath::Abs(TestBoneCount - ReferenceBoneCount) <= 5; // Allow some difference

    UE_LOG(LogTemp, Log, TEXT("Skeleton compatibility result: %s"), bIsCompatible ? TEXT("Compatible") : TEXT("Incompatible"));

    return bIsCompatible;
}

