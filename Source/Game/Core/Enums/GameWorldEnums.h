#pragma once

#include "CoreMinimal.h"

// ========== WORLD PLACEMENT ENUMERATIONS ==========

UENUM()
enum class EWPLObjectType : uint8
{
    BUILDING            UMETA(DisplayName = "Building"),
    PROP                UMETA(DisplayName = "Prop"),
    VEGETATION          UMETA(DisplayName = "Vegetation"),
    STREET_FURNITURE    UMETA(DisplayName = "Street Furniture"),
    VEHICLE_SPAWN       UMETA(DisplayName = "Vehicle Spawn"),
    PED_SPAWN           UMETA(DisplayName = "Ped Spawn"),
    LIGHT               UMETA(DisplayName = "Light"),
    AUDIO_EMITTER       UMETA(DisplayName = "Audio Emitter"),
    COLLISION_MESH      UMETA(DisplayName = "Collision Mesh"),
    TRIGGER_VOLUME      UMETA(DisplayName = "Trigger Volume"),
    INVALID             UMETA(DisplayName = "Invalid")
};

UENUM()
enum class ELODLevel : uint8
{
    HD                  UMETA(DisplayName = "HD (0-50m)"),
    HIGH                UMETA(DisplayName = "High (50-150m)"),
    MEDIUM              UMETA(DisplayName = "Medium (150-400m)"),
    LOW                 UMETA(DisplayName = "Low (400-800m)"),
    SLOD                UMETA(DisplayName = "SLOD (800m+)"),
    INVALID             UMETA(DisplayName = "Invalid")
};

// ========== BITWISE STREAMING FLAGS ==========
UENUM(meta = (Bitflags, UseEnumValuesAsMaskValuesInEditor = "true"))
enum class EStreamingFlags : uint32
{
    NONE                = 0,
    ALWAYS_LOADED       = 1 << 0,   // 0x00000001
    MISSION_CRITICAL    = 1 << 1,   // 0x00000002
    PHYSICS_ENABLED     = 1 << 2,   // 0x00000004
    COLLISION_ENABLED   = 1 << 3,   // 0x00000008
    SHADOW_CASTER       = 1 << 4,   // 0x00000010
    DESTRUCTIBLE        = 1 << 5,   // 0x00000020
    INTERACTIVE         = 1 << 6,   // 0x00000040
    NETWORK_SYNC        = 1 << 7,   // 0x00000080
    LOD_FADE            = 1 << 8,   // 0x00000100
    CULLING_ENABLED     = 1 << 9,   // 0x00000200
    WEATHER_AFFECTED    = 1 << 10,  // 0x00000400
    TIME_DEPENDENT      = 1 << 11,  // 0x00000800
};
ENUM_CLASS_FLAGS(EStreamingFlags);

// ========== INTERIOR PLACEMENT ENUMERATIONS ==========

UENUM()
enum class EIPLType : uint8
{
    INTERIOR            UMETA(DisplayName = "Interior"),
    EXTERIOR_CUTOUT     UMETA(DisplayName = "Exterior Cutout"),
    PORTAL              UMETA(DisplayName = "Portal"),
    OCCLUSION_PLANE     UMETA(DisplayName = "Occlusion Plane"),
    INVALID             UMETA(DisplayName = "Invalid")
};

UENUM()
enum class EInteriorType : uint8
{
    RESIDENTIAL         UMETA(DisplayName = "Residential"),
    COMMERCIAL          UMETA(DisplayName = "Commercial"),
    INDUSTRIAL          UMETA(DisplayName = "Industrial"),
    OFFICE              UMETA(DisplayName = "Office"),
    RETAIL              UMETA(DisplayName = "Retail"),
    ENTERTAINMENT       UMETA(DisplayName = "Entertainment"),
    GOVERNMENT          UMETA(DisplayName = "Government"),
    MEDICAL             UMETA(DisplayName = "Medical"),
    EDUCATIONAL         UMETA(DisplayName = "Educational"),
    RELIGIOUS           UMETA(DisplayName = "Religious"),
    GARAGE              UMETA(DisplayName = "Garage"),
    WAREHOUSE           UMETA(DisplayName = "Warehouse"),
    TUNNEL              UMETA(DisplayName = "Tunnel"),
    UNDERGROUND         UMETA(DisplayName = "Underground"),
    INVALID             UMETA(DisplayName = "Invalid")
};

UENUM()
enum class EDoorType : uint8
{
    AUTOMATIC_SLIDING   UMETA(DisplayName = "Automatic Sliding"),
    PUSH_PULL           UMETA(DisplayName = "Push Pull"),
    REVOLVING           UMETA(DisplayName = "Revolving"),
    GARAGE_DOOR         UMETA(DisplayName = "Garage Door"),
    ELEVATOR            UMETA(DisplayName = "Elevator"),
    GATE                UMETA(DisplayName = "Gate"),
    HATCH               UMETA(DisplayName = "Hatch"),
    WINDOW              UMETA(DisplayName = "Window"),
    INVALID             UMETA(DisplayName = "Invalid")
};

// ========== COLLISION VALIDATION ENUMERATIONS ==========

UENUM()
enum class EEntryMethod : uint8
{
    DOOR_TRIGGER        UMETA(DisplayName = "Door Trigger"),
    WINDOW_BREAK        UMETA(DisplayName = "Window Break"),
    ELEVATOR           UMETA(DisplayName = "Elevator"),
    TELEPORT           UMETA(DisplayName = "Teleport"),
    MISSION_TELEPORT   UMETA(DisplayName = "Mission Teleport"),
    ADMIN_TELEPORT     UMETA(DisplayName = "Admin Teleport"),
    NOCLIP             UMETA(DisplayName = "NoClip"),
    ADMIN_NOCLIP       UMETA(DisplayName = "Admin NoClip"),
    WALL_PHASE         UMETA(DisplayName = "Wall Phase"),
    INVALID            UMETA(DisplayName = "Invalid")
};

UENUM()
enum class EValidationAction : uint8
{
    ALLOW              UMETA(DisplayName = "Allow"),
    CORRECT_POSITION   UMETA(DisplayName = "Correct Position"),
    RETURN_TO_ENTRY    UMETA(DisplayName = "Return to Entry"),
    KICK_PLAYER        UMETA(DisplayName = "Kick Player"),
    LOG_VIOLATION      UMETA(DisplayName = "Log Violation")
};

// ========== SANDBOX SYSTEM ENUMERATIONS ==========

UENUM()
enum class ESandboxElementType : uint8
{
    PHYSICS_PLAYGROUND  UMETA(DisplayName = "Physics Playground"),
    DESTRUCTION_ZONE    UMETA(DisplayName = "Destruction Zone"),
    BUILDING_ZONE       UMETA(DisplayName = "Building Zone"),
    VEHICLE_STUNT_AREA  UMETA(DisplayName = "Vehicle Stunt Area"),
    PROP_SPAWNER        UMETA(DisplayName = "Prop Spawner"),
    TOOL_STATION        UMETA(DisplayName = "Tool Station"),
    SOCIAL_HUB          UMETA(DisplayName = "Social Hub"),
    INVALID             UMETA(DisplayName = "Invalid")
};

// ========== BITWISE MULTIPLAYER FEATURE FLAGS ==========
UENUM(meta = (Bitflags, UseEnumValuesAsMaskValuesInEditor = "true"))
enum class EMultiplayerFeature : uint32
{
    NONE                = 0,
    VOICE_CHAT          = 1 << 0,   // 0x00000001
    TEXT_CHAT           = 1 << 1,   // 0x00000002
    PROP_SHARING        = 1 << 2,   // 0x00000004
    COLLABORATIVE_BUILD = 1 << 3,   // 0x00000008
    COMPETITION_MODE    = 1 << 4,   // 0x00000010
    SPECTATOR_MODE      = 1 << 5,   // 0x00000020
    ADMIN_TOOLS         = 1 << 6,   // 0x00000040
    SCREEN_SHARING      = 1 << 7,   // 0x00000080
    WHITEBOARD          = 1 << 8,   // 0x00000100
    PHYSICS_SYNC        = 1 << 9,   // 0x00000200
    REAL_TIME_EDITING   = 1 << 10,  // 0x00000400
    GROUP_PERMISSIONS   = 1 << 11,  // 0x00000800
};
ENUM_CLASS_FLAGS(EMultiplayerFeature);

// ========== ENTITY SYSTEM ENUMERATIONS ==========

UENUM(BlueprintType)
enum class EEntityType : uint8
{
    WorldObject         UMETA(DisplayName = "World Object"),
    Ped                 UMETA(DisplayName = "Ped"),
    Vehicle             UMETA(DisplayName = "Vehicle"),
    Weapon              UMETA(DisplayName = "Weapon"),
    Pickup              UMETA(DisplayName = "Pickup"),
    Projectile          UMETA(DisplayName = "Projectile"),
    Trigger             UMETA(DisplayName = "Trigger"),
    Invalid             UMETA(DisplayName = "Invalid")
};

UENUM(BlueprintType)
enum class EEntityState : uint8
{
    Idle                UMETA(DisplayName = "Idle"),
    Active              UMETA(DisplayName = "Active"),
    Moving              UMETA(DisplayName = "Moving"),
    Combat              UMETA(DisplayName = "Combat"),
    Damaged             UMETA(DisplayName = "Damaged"),
    Dead                UMETA(DisplayName = "Dead"),
    Destroyed           UMETA(DisplayName = "Destroyed"),
    Disabled            UMETA(DisplayName = "Disabled"),
    Spawning            UMETA(DisplayName = "Spawning"),
    Despawning          UMETA(DisplayName = "Despawning"),
    Invalid             UMETA(DisplayName = "Invalid")
};

// ========== PED SYSTEM ENUMERATIONS ==========

UENUM(BlueprintType)
enum class EPedType : uint8
{
    Player              UMETA(DisplayName = "Player"),
    Civilian            UMETA(DisplayName = "Civilian"),
    Police              UMETA(DisplayName = "Police"),
    FireDept            UMETA(DisplayName = "Fire Department"),
    Medic               UMETA(DisplayName = "Medic"),
    Gang                UMETA(DisplayName = "Gang"),
    Criminal            UMETA(DisplayName = "Criminal"),
    Security            UMETA(DisplayName = "Security"),
    Military            UMETA(DisplayName = "Military"),
    Animal              UMETA(DisplayName = "Animal"),
    Invalid             UMETA(DisplayName = "Invalid")
};

UENUM(BlueprintType)
enum class ERelationshipType : uint8
{
    Ally                UMETA(DisplayName = "Ally"),
    Friendly            UMETA(DisplayName = "Friendly"),
    Neutral             UMETA(DisplayName = "Neutral"),
    Dislike             UMETA(DisplayName = "Dislike"),
    Hostile             UMETA(DisplayName = "Hostile"),
    Enemy               UMETA(DisplayName = "Enemy"),
    Invalid             UMETA(DisplayName = "Invalid")
};

// ========== PED BODY PARTS ENUMERATIONS ==========

UENUM(BlueprintType)
enum class EPedBodyPart : uint8
{
    Head                UMETA(DisplayName = "Head"),
    Hair                UMETA(DisplayName = "Hair"),
    Eyes                UMETA(DisplayName = "Eyes"),
    Teeth               UMETA(DisplayName = "Teeth"),
    Upper               UMETA(DisplayName = "Upper Body"),
    Lower               UMETA(DisplayName = "Lower Body"),
    Hands               UMETA(DisplayName = "Hands"),
    Arms                UMETA(DisplayName = "Arms"),
    Feet                UMETA(DisplayName = "Feet"),
    Nails               UMETA(DisplayName = "Nails"),
    Legs                UMETA(DisplayName = "Legs"),
    Torso               UMETA(DisplayName = "Torso"),
    Jacket              UMETA(DisplayName = "Jacket"),
    Vest                UMETA(DisplayName = "Vest"),
    Invalid             UMETA(DisplayName = "Invalid")
};

UENUM(BlueprintType)
enum class EPedPropType : uint8
{
    Hat                 UMETA(DisplayName = "Hat"),
    Glasses             UMETA(DisplayName = "Glasses"),
    Earrings            UMETA(DisplayName = "Earrings"),
    Watch               UMETA(DisplayName = "Watch"),
    Bracelet            UMETA(DisplayName = "Bracelet"),
    Necklace            UMETA(DisplayName = "Necklace"),
    Gloves              UMETA(DisplayName = "Gloves"),
    Bag                 UMETA(DisplayName = "Bag"),
    Weapon              UMETA(DisplayName = "Weapon"),
    Custom              UMETA(DisplayName = "Custom"),
    Invalid             UMETA(DisplayName = "Invalid")
};

UENUM(BlueprintType)
enum class EPedGender : uint8
{
    Male                UMETA(DisplayName = "Male"),
    Female              UMETA(DisplayName = "Female"),
    Invalid             UMETA(DisplayName = "Invalid")
};

UENUM(BlueprintType)
enum class EPedRace : uint8
{
    Universal           UMETA(DisplayName = "Universal"),
    Black               UMETA(DisplayName = "Black"),
    White               UMETA(DisplayName = "White"),
    Arab                UMETA(DisplayName = "Arab"),
    Jew                 UMETA(DisplayName = "Jew"),
    Indian              UMETA(DisplayName = "Indian"),
    Chinese             UMETA(DisplayName = "Chinese"),
    Japanese            UMETA(DisplayName = "Japanese"),
    Russian             UMETA(DisplayName = "Russian"),
    Custom              UMETA(DisplayName = "Custom"),
    Invalid             UMETA(DisplayName = "Invalid")
};

// ========== COMPONENT VARIATION SYSTEM ==========



UENUM(BlueprintType)
enum class ETextureRaceType : uint8
{
    Universal           UMETA(DisplayName = "Universal"),
    Black               UMETA(DisplayName = "Black"),
    White               UMETA(DisplayName = "White"),
    Arab                UMETA(DisplayName = "Arab"),
    Jew                 UMETA(DisplayName = "Jew"),
    Indian              UMETA(DisplayName = "Indian"),
    Chinese             UMETA(DisplayName = "Chinese"),
    Japanese            UMETA(DisplayName = "Japanese"),
    Russian             UMETA(DisplayName = "Russian"),
    Custom              UMETA(DisplayName = "Custom"),
    Invalid             UMETA(DisplayName = "Invalid")
};

// ========== VEHICLE SYSTEM ENUMERATIONS ==========

UENUM(BlueprintType)
enum class EPedVehicleType : uint8
{
    Compact             UMETA(DisplayName = "Compact"),
    Sedan               UMETA(DisplayName = "Sedan"),
    SUV                 UMETA(DisplayName = "SUV"),
    Sports              UMETA(DisplayName = "Sports"),
    Coupe               UMETA(DisplayName = "Coupe"),
    Muscle              UMETA(DisplayName = "Muscle"),
    Offroad             UMETA(DisplayName = "Offroad"),
    Motorcycle          UMETA(DisplayName = "Motorcycle"),
    Truck               UMETA(DisplayName = "Truck"),
    Van                 UMETA(DisplayName = "Van"),
    Bus                 UMETA(DisplayName = "Bus"),
    Emergency           UMETA(DisplayName = "Emergency"),
    Military            UMETA(DisplayName = "Military"),
    Aircraft            UMETA(DisplayName = "Aircraft"),
    Boat                UMETA(DisplayName = "Boat"),
    Bicycle             UMETA(DisplayName = "Bicycle"),
    Train               UMETA(DisplayName = "Train"),
    Invalid             UMETA(DisplayName = "Invalid")
};

UENUM(BlueprintType)
enum class EPedVehicleClass : uint8
{
    Economy             UMETA(DisplayName = "Economy"),
    Standard            UMETA(DisplayName = "Standard"),
    Premium             UMETA(DisplayName = "Premium"),
    Luxury              UMETA(DisplayName = "Luxury"),
    Super               UMETA(DisplayName = "Super"),
    Invalid             UMETA(DisplayName = "Invalid")
};

// ========== ANIMATION & MOVEMENT ENUMERATIONS ==========

UENUM(BlueprintType)
enum class EPedMovementState : uint8
{
    Idle            UMETA(DisplayName = "Idle"),
    Walking         UMETA(DisplayName = "Walking"),
    Running         UMETA(DisplayName = "Running"),
    Sprinting       UMETA(DisplayName = "Sprinting"),
    Crouching       UMETA(DisplayName = "Crouching"),
    Jumping         UMETA(DisplayName = "Jumping"),
    InAir           UMETA(DisplayName = "In Air"),
    Landing         UMETA(DisplayName = "Landing"),
    Combat          UMETA(DisplayName = "Combat"),
    Cover           UMETA(DisplayName = "Cover"),
    Climbing        UMETA(DisplayName = "Climbing"),
    InVehicle       UMETA(DisplayName = "In Vehicle"),
    Turning         UMETA(DisplayName = "Turning")
};

UENUM(BlueprintType)
enum class EPedAnimationRegion : uint8
{
    OnFoot          UMETA(DisplayName = "On Foot"),
    Jump            UMETA(DisplayName = "Jump"),
    Crouch          UMETA(DisplayName = "Crouch"),
    Combat          UMETA(DisplayName = "Combat"),
    Cover           UMETA(DisplayName = "Cover"),
    InVehicle       UMETA(DisplayName = "In Vehicle"),
    Interaction     UMETA(DisplayName = "Interaction"),
    Emote           UMETA(DisplayName = "Emote")
};

UENUM(BlueprintType)
enum class EPedWeaponType : uint8
{
    None            UMETA(DisplayName = "No Weapon"),
    Pistol          UMETA(DisplayName = "Pistol"),
    Rifle           UMETA(DisplayName = "Rifle"),
    Shotgun         UMETA(DisplayName = "Shotgun"),
    SMG             UMETA(DisplayName = "SMG"),
    Sniper          UMETA(DisplayName = "Sniper Rifle"),
    Melee           UMETA(DisplayName = "Melee Weapon"),
    Thrown          UMETA(DisplayName = "Thrown Weapon"),
    Heavy           UMETA(DisplayName = "Heavy Weapon")
};

UENUM(BlueprintType)
enum class EPedCombatState : uint8
{
    Relaxed         UMETA(DisplayName = "Relaxed"),
    Alert           UMETA(DisplayName = "Alert"),
    Aiming          UMETA(DisplayName = "Aiming"),
    Firing          UMETA(DisplayName = "Firing"),
    Reloading       UMETA(DisplayName = "Reloading"),
    Melee           UMETA(DisplayName = "Melee Combat"),
    Throwing        UMETA(DisplayName = "Throwing"),
    TakingCover     UMETA(DisplayName = "Taking Cover")
};

UENUM(BlueprintType)
enum class EPedStanceState : uint8
{
    Standing        UMETA(DisplayName = "Standing"),
    Crouched        UMETA(DisplayName = "Crouched"),
    Combat          UMETA(DisplayName = "Combat"),
    Prone           UMETA(DisplayName = "Prone"),
    Sitting         UMETA(DisplayName = "Sitting"),
    Climbing        UMETA(DisplayName = "Climbing")
};

UENUM(BlueprintType)
enum class ETurnDirection : uint8
{
    None            UMETA(DisplayName = "None"),
    Left            UMETA(DisplayName = "Left"),
    Right           UMETA(DisplayName = "Right"),
    Around180       UMETA(DisplayName = "Around 180"),
    Full360         UMETA(DisplayName = "Full 360")
};

// ========== WORLD OBJECT ENUMERATIONS ==========

UENUM(BlueprintType)
enum class EWorldObjectType : uint8
{
    Static              UMETA(DisplayName = "Static Object"),
    Interactive         UMETA(DisplayName = "Interactive Object"),
    Destructible        UMETA(DisplayName = "Destructible Object"),
    Pickup              UMETA(DisplayName = "Pickup"),
    Container           UMETA(DisplayName = "Container"),
    Door                UMETA(DisplayName = "Door"),
    Window              UMETA(DisplayName = "Window"),
    Light               UMETA(DisplayName = "Light"),
    Trigger             UMETA(DisplayName = "Trigger"),
    Spawner             UMETA(DisplayName = "Spawner"),
    Decoration          UMETA(DisplayName = "Decoration"),
    Furniture           UMETA(DisplayName = "Furniture"),
    Invalid             UMETA(DisplayName = "Invalid")
};

// ========== ENTITY DAMAGE SYSTEM ENUMERATIONS ==========

UENUM(BlueprintType)
enum class EEntityDamageType : uint8
{
    None                UMETA(DisplayName = "None"),
    Physical            UMETA(DisplayName = "Physical"),
    Bullet              UMETA(DisplayName = "Bullet"),
    Explosion           UMETA(DisplayName = "Explosion"),
    Fire                UMETA(DisplayName = "Fire"),
    Fall                UMETA(DisplayName = "Fall"),
    Drowning            UMETA(DisplayName = "Drowning"),
    Poison              UMETA(DisplayName = "Poison"),
    Electric            UMETA(DisplayName = "Electric"),
    Collision           UMETA(DisplayName = "Collision"),
    Melee               UMETA(DisplayName = "Melee"),
    Environmental       UMETA(DisplayName = "Environmental"),
    Invalid             UMETA(DisplayName = "Invalid")
};

UENUM(BlueprintType)
enum class EHitBodyPart : uint8
{
    Head                UMETA(DisplayName = "Head"),
    Neck                UMETA(DisplayName = "Neck"),
    Torso               UMETA(DisplayName = "Torso"),
    LeftArm             UMETA(DisplayName = "Left Arm"),
    RightArm            UMETA(DisplayName = "Right Arm"),
    LeftLeg             UMETA(DisplayName = "Left Leg"),
    RightLeg            UMETA(DisplayName = "Right Leg"),
    LeftHand            UMETA(DisplayName = "Left Hand"),
    RightHand           UMETA(DisplayName = "Right Hand"),
    LeftFoot            UMETA(DisplayName = "Left Foot"),
    RightFoot           UMETA(DisplayName = "Right Foot"),
    Unknown             UMETA(DisplayName = "Unknown"),
    Invalid             UMETA(DisplayName = "Invalid")
};

// ========== PED PERSONALITY SYSTEM ==========

UENUM(BlueprintType)
enum class EPedPersonalityType : uint8
{
    Neutral             UMETA(DisplayName = "Neutral"),
    Aggressive          UMETA(DisplayName = "Aggressive"),
    Passive             UMETA(DisplayName = "Passive"),
    Friendly            UMETA(DisplayName = "Friendly"),
    Hostile             UMETA(DisplayName = "Hostile"),
    Coward              UMETA(DisplayName = "Coward"),
    Brave               UMETA(DisplayName = "Brave"),
    Curious             UMETA(DisplayName = "Curious"),
    Paranoid            UMETA(DisplayName = "Paranoid"),
    Calm                UMETA(DisplayName = "Calm"),
    Erratic             UMETA(DisplayName = "Erratic"),
    Invalid             UMETA(DisplayName = "Invalid")
};
