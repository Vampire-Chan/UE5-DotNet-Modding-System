#pragma once

#include "CoreMinimal.h"

// ========== WEAPON CLASSIFICATION ENUMERATIONS ==========

UENUM()
enum class EWeaponClass : uint8
{
    MELEE               UMETA(DisplayName = "Melee"),
    HANDGUN             UMETA(DisplayName = "Handgun"),
    SUBMACHINE_GUN      UMETA(DisplayName = "Submachine Gun"),
    SHOTGUN             UMETA(DisplayName = "Shotgun"),
    ASSAULT_RIFLE       UMETA(DisplayName = "Assault Rifle"),
    SNIPER_RIFLE        UMETA(DisplayName = "Sniper Rifle"),
    LIGHT_MACHINE_GUN   UMETA(DisplayName = "Light Machine Gun"),
    HEAVY_WEAPON        UMETA(DisplayName = "Heavy Weapon"),
    LAUNCHER            UMETA(DisplayName = "Launcher"),
    THROWABLE           UMETA(DisplayName = "Throwable"),
    SPECIAL             UMETA(DisplayName = "Special"),
    INVALID             UMETA(DisplayName = "Invalid")
};

UENUM()
enum class EWeaponSubClass : uint8
{
    // Melee Sub-classes
    KNIFE               UMETA(DisplayName = "Knife"),
    BAT                 UMETA(DisplayName = "Bat"),
    SWORD               UMETA(DisplayName = "Sword"),
    HAMMER              UMETA(DisplayName = "Hammer"),
    
    // Handgun Sub-classes
    PISTOL              UMETA(DisplayName = "Pistol"),
    REVOLVER            UMETA(DisplayName = "Revolver"),
    HEAVY_PISTOL        UMETA(DisplayName = "Heavy Pistol"),
    
    // Rifle Sub-classes
    CARBINE             UMETA(DisplayName = "Carbine"),
    BATTLE_RIFLE        UMETA(DisplayName = "Battle Rifle"),
    MARKSMAN_RIFLE      UMETA(DisplayName = "Marksman Rifle"),
    
    // Shotgun Sub-classes
    PUMP_ACTION         UMETA(DisplayName = "Pump Action"),
    SEMI_AUTO           UMETA(DisplayName = "Semi Auto"),
    COMBAT_SHOTGUN      UMETA(DisplayName = "Combat Shotgun"),
    
    // Special Sub-classes
    ENERGY_WEAPON       UMETA(DisplayName = "Energy Weapon"),
    EXPERIMENTAL        UMETA(DisplayName = "Experimental"),
    
    INVALID             UMETA(DisplayName = "Invalid")
};

UENUM()
enum class EWeaponFireMode : uint8
{
    SINGLE              UMETA(DisplayName = "Single"),
    BURST               UMETA(DisplayName = "Burst"),
    FULL_AUTO           UMETA(DisplayName = "Full Auto"),
    CHARGE              UMETA(DisplayName = "Charge"),
    PUMP                UMETA(DisplayName = "Pump"),
    BOLT_ACTION         UMETA(DisplayName = "Bolt Action"),
    INVALID             UMETA(DisplayName = "Invalid")
};

UENUM()
enum class EAmmoType : uint8
{
    NINE_MM             UMETA(DisplayName = "9mm"),
    FORTY_FIVE_ACP      UMETA(DisplayName = ".45 ACP"),
    FIVE_SEVEN_MM       UMETA(DisplayName = "5.7mm"),
    FIVE_FIVE_SIX_MM    UMETA(DisplayName = "5.56mm"),
    SEVEN_SIX_TWO_MM    UMETA(DisplayName = "7.62mm"),
    THREE_ZERO_EIGHT    UMETA(DisplayName = ".308"),
    FIFTY_CAL           UMETA(DisplayName = ".50 Cal"),
    TWELVE_GAUGE        UMETA(DisplayName = "12 Gauge"),
    TWENTY_GAUGE        UMETA(DisplayName = "20 Gauge"),
    FORTY_MM_GRENADE    UMETA(DisplayName = "40mm Grenade"),
    ROCKET              UMETA(DisplayName = "Rocket"),
    ENERGY_CELL         UMETA(DisplayName = "Energy Cell"),
    ARROW               UMETA(DisplayName = "Arrow"),
    BOLT                UMETA(DisplayName = "Bolt"),
    INVALID             UMETA(DisplayName = "Invalid")
};

// ========== WEAPON STATE ENUMERATIONS ==========

UENUM()
enum class EWeaponState : uint8
{
    HOLSTERED           UMETA(DisplayName = "Holstered"),
    DRAWING             UMETA(DisplayName = "Drawing"),
    IDLE                UMETA(DisplayName = "Idle"),
    FIRING              UMETA(DisplayName = "Firing"),
    RELOADING           UMETA(DisplayName = "Reloading"),
    CHARGING            UMETA(DisplayName = "Charging"),
    JAMMED              UMETA(DisplayName = "Jammed"),
    HOLSTERING          UMETA(DisplayName = "Holstering"),
    MELEE_ATTACK        UMETA(DisplayName = "Melee Attack"),
    INVALID             UMETA(DisplayName = "Invalid")
};

UENUM()
enum class EWeaponCondition : uint8
{
    PRISTINE            UMETA(DisplayName = "Pristine"),
    EXCELLENT           UMETA(DisplayName = "Excellent"),
    GOOD                UMETA(DisplayName = "Good"),
    FAIR                UMETA(DisplayName = "Fair"),
    POOR                UMETA(DisplayName = "Poor"),
    BROKEN              UMETA(DisplayName = "Broken"),
    INVALID             UMETA(DisplayName = "Invalid")
};

// ========== ATTACHMENT ENUMERATIONS ==========

UENUM(meta = (Bitflags, UseEnumValuesAsMaskValuesInEditor = "true"))
enum class EWeaponAttachmentType : uint32
{
    NONE                = 0,
    SCOPE               = 1 << 0,   // 0x00000001 UMETA(DisplayName = "Scope"),
    SIGHT               = 1 << 1,   // 0x00000002 UMETA(DisplayName = "Sight"),
    SUPPRESSOR          = 1 << 2,   // 0x00000004 UMETA(DisplayName = "Suppressor"),
    MUZZLE_BRAKE        = 1 << 3,   // 0x00000008 UMETA(DisplayName = "Muzzle Brake"),
    COMPENSATOR         = 1 << 4,   // 0x00000010 UMETA(DisplayName = "Compensator"),
    FOREGRIP            = 1 << 5,   // 0x00000020 UMETA(DisplayName = "Foregrip"),
    BIPOD               = 1 << 6,   // 0x00000040 UMETA(DisplayName = "Bipod"),
    FLASHLIGHT          = 1 << 7,   // 0x00000080 UMETA(DisplayName = "Flashlight"),
    LASER               = 1 << 8,   // 0x00000100 UMETA(DisplayName = "Laser"),
    EXTENDED_MAG        = 1 << 9,   // 0x00000200 UMETA(DisplayName = "Extended Mag"),
    DRUM_MAG            = 1 << 10,  // 0x00000400 UMETA(DisplayName = "Drum Mag"),
    STOCK               = 1 << 11,  // 0x00000800 UMETA(DisplayName = "Stock"),
};
ENUM_CLASS_FLAGS(EWeaponAttachmentType);

UENUM()
enum class EAttachmentSlot : uint8
{
    MUZZLE              UMETA(DisplayName = "Muzzle"),
    BARREL              UMETA(DisplayName = "Barrel"),
    SIGHT               UMETA(DisplayName = "Sight"),
    GRIP                UMETA(DisplayName = "Grip"),
    MAGAZINE            UMETA(DisplayName = "Magazine"),
    STOCK               UMETA(DisplayName = "Stock"),
    ACCESSORY           UMETA(DisplayName = "Accessory"),
    UNDERBARREL         UMETA(DisplayName = "Underbarrel"),
    INVALID             UMETA(DisplayName = "Invalid")
};

// ========== DAMAGE & BALLISTICS ENUMERATIONS ==========

UENUM()
enum class EDamageType : uint8
{
    KINETIC             UMETA(DisplayName = "Kinetic"),
    EXPLOSIVE           UMETA(DisplayName = "Explosive"),
    INCENDIARY          UMETA(DisplayName = "Incendiary"),
    ARMOR_PIERCING      UMETA(DisplayName = "Armor Piercing"),
    HOLLOW_POINT        UMETA(DisplayName = "Hollow Point"),
    ENERGY              UMETA(DisplayName = "Energy"),
    POISON              UMETA(DisplayName = "Poison"),
    ELECTRIC            UMETA(DisplayName = "Electric"),
    FREEZE              UMETA(DisplayName = "Freeze"),
    BLUNT_FORCE         UMETA(DisplayName = "Blunt Force"),
    CUTTING             UMETA(DisplayName = "Cutting"),
    PIERCING            UMETA(DisplayName = "Piercing"),
    INVALID             UMETA(DisplayName = "Invalid")
};

UENUM()
enum class EHitLocation : uint8
{
    HEAD                UMETA(DisplayName = "Head"),
    NECK                UMETA(DisplayName = "Neck"),
    CHEST               UMETA(DisplayName = "Chest"),
    STOMACH             UMETA(DisplayName = "Stomach"),
    LEFT_ARM            UMETA(DisplayName = "Left Arm"),
    RIGHT_ARM           UMETA(DisplayName = "Right Arm"),
    LEFT_LEG            UMETA(DisplayName = "Left Leg"),
    RIGHT_LEG           UMETA(DisplayName = "Right Leg"),
    HANDS               UMETA(DisplayName = "Hands"),
    FEET                UMETA(DisplayName = "Feet"),
    INVALID             UMETA(DisplayName = "Invalid")
};

// ========== RELOAD SYSTEM ENUMERATIONS ==========

UENUM()
enum class EReloadType : uint8
{
    MAGAZINE            UMETA(DisplayName = "Magazine"),
    INDIVIDUAL_ROUNDS   UMETA(DisplayName = "Individual Rounds"),
    BREAK_ACTION        UMETA(DisplayName = "Break Action"),
    STRIP_CLIP          UMETA(DisplayName = "Strip Clip"),
    BELT_FED            UMETA(DisplayName = "Belt Fed"),
    ENERGY_CELL         UMETA(DisplayName = "Energy Cell"),
    INVALID             UMETA(DisplayName = "Invalid")
};

UENUM()
enum class EReloadStage : uint8
{
    NONE                UMETA(DisplayName = "None"),
    EJECT_MAG           UMETA(DisplayName = "Eject Magazine"),
    INSERT_MAG          UMETA(DisplayName = "Insert Magazine"),
    CHAMBER_ROUND       UMETA(DisplayName = "Chamber Round"),
    CHECK_CHAMBER       UMETA(DisplayName = "Check Chamber"),
    TACTICAL_RELOAD     UMETA(DisplayName = "Tactical Reload"),
    EMERGENCY_RELOAD    UMETA(DisplayName = "Emergency Reload"),
    INVALID             UMETA(DisplayName = "Invalid")
};
