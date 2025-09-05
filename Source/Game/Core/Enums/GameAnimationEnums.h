#pragma once

#include "CoreMinimal.h"

// ========== ENTITY ANIMATION ENUMERATIONS ==========

UENUM()
enum class EEntityAnimationType : uint8
{
    PED                 UMETA(DisplayName = "Ped/Character"),
    VEHICLE             UMETA(DisplayName = "Vehicle"),
    OBJECT              UMETA(DisplayName = "Object/Prop"),
    BUILDING            UMETA(DisplayName = "Building"),
    INVALID             UMETA(DisplayName = "Invalid")
};

// ========== BITWISE ANIMATION FLAGS ==========
UENUM(meta = (Bitflags, UseEnumValuesAsMaskValuesInEditor = "true"))
enum class EAnimationFlags : uint32
{
    NONE                = 0,
    LOOP                = 1 << 0,   // 0x00000001
    HOLD_LAST_FRAME     = 1 << 1,   // 0x00000002
    FREEZE_LAST_FRAME   = 1 << 2,   // 0x00000004
    UPPER_BODY_ONLY     = 1 << 3,   // 0x00000008
    LOWER_BODY_ONLY     = 1 << 4,   // 0x00000010
    CANCEL_ABLE         = 1 << 5,   // 0x00000020
    NOT_INTERRUPTIBLE   = 1 << 6,   // 0x00000040
    BLEND_IN            = 1 << 7,   // 0x00000080
    BLEND_OUT           = 1 << 8,   // 0x00000100
    SECONDARY           = 1 << 9,   // 0x00000200
    FACIAL              = 1 << 10,  // 0x00000400
    RAGDOLL_ON_END      = 1 << 11,  // 0x00000800
    ADDITIVE            = 1 << 12,  // 0x00001000
    OVERRIDE_PHYSICS    = 1 << 13,  // 0x00002000
    ROOT_MOTION         = 1 << 14,  // 0x00004000
    MIRROR_ANIMATION    = 1 << 15,  // 0x00008000
};
ENUM_CLASS_FLAGS(EAnimationFlags);

// ========== PED ANIMATION ENUMERATIONS ==========

UENUM()
enum class EPedAnimationType : uint8
{
    IDLE                UMETA(DisplayName = "Idle"),
    WALK                UMETA(DisplayName = "Walk"),
    RUN                 UMETA(DisplayName = "Run"),
    SPRINT              UMETA(DisplayName = "Sprint"),
    JUMP                UMETA(DisplayName = "Jump"),
    CLIMB               UMETA(DisplayName = "Climb"),
    CROUCH              UMETA(DisplayName = "Crouch"),
    CRAWL               UMETA(DisplayName = "Crawl"),
    COMBAT_IDLE         UMETA(DisplayName = "Combat Idle"),
    COMBAT_STRAFE       UMETA(DisplayName = "Combat Strafe"),
    ATTACK              UMETA(DisplayName = "Attack"),
    RELOAD              UMETA(DisplayName = "Reload"),
    DEATH               UMETA(DisplayName = "Death"),
    EMOTE               UMETA(DisplayName = "Emote"),
    INTERACTION         UMETA(DisplayName = "Interaction"),
    VEHICLE_ENTER       UMETA(DisplayName = "Vehicle Enter"),
    VEHICLE_EXIT        UMETA(DisplayName = "Vehicle Exit"),
    FALL                UMETA(DisplayName = "Fall"),
    LAND                UMETA(DisplayName = "Land")
};

// ========== VEHICLE ANIMATION ENUMERATIONS ==========

UENUM()
enum class EVehicleAnimationType : uint8
{
    HOOD_OPEN           UMETA(DisplayName = "Hood Open"),
    HOOD_CLOSE          UMETA(DisplayName = "Hood Close"),
    DOOR_OPEN           UMETA(DisplayName = "Door Open"),
    DOOR_CLOSE          UMETA(DisplayName = "Door Close"),
    TRUNK_OPEN          UMETA(DisplayName = "Trunk Open"),
    TRUNK_CLOSE         UMETA(DisplayName = "Trunk Close"),
    POPUP_LIGHTS_UP     UMETA(DisplayName = "Popup Lights Up"),
    POPUP_LIGHTS_DOWN   UMETA(DisplayName = "Popup Lights Down"),
    LANDING_GEAR_UP     UMETA(DisplayName = "Landing Gear Up"),
    LANDING_GEAR_DOWN   UMETA(DisplayName = "Landing Gear Down"),
    WING_FOLD           UMETA(DisplayName = "Wing Fold"),
    WING_DEPLOY         UMETA(DisplayName = "Wing Deploy"),
    ROTOR_SPIN_UP       UMETA(DisplayName = "Rotor Spin Up"),
    ROTOR_SPIN_DOWN     UMETA(DisplayName = "Rotor Spin Down"),
    SUSPENSION_LOWER    UMETA(DisplayName = "Suspension Lower"),
    SUSPENSION_RAISE    UMETA(DisplayName = "Suspension Raise")
};

UENUM()
enum class EVehicleClass : uint8
{
    COMPACT             UMETA(DisplayName = "Compact"),
    SEDAN               UMETA(DisplayName = "Sedan"),
    SUV                 UMETA(DisplayName = "SUV"),
    COUPE               UMETA(DisplayName = "Coupe"),
    MUSCLE              UMETA(DisplayName = "Muscle"),
    SPORTS_CLASSIC      UMETA(DisplayName = "Sports Classic"),
    SPORTS              UMETA(DisplayName = "Sports"),
    SUPER               UMETA(DisplayName = "Super"),
    MOTORCYCLE          UMETA(DisplayName = "Motorcycle"),
    OFF_ROAD            UMETA(DisplayName = "Off Road"),
    INDUSTRIAL          UMETA(DisplayName = "Industrial"),
    UTILITY             UMETA(DisplayName = "Utility"),
    VAN                 UMETA(DisplayName = "Van"),
    BIKE                UMETA(DisplayName = "Bike"),
    BOAT                UMETA(DisplayName = "Boat"),
    HELICOPTER          UMETA(DisplayName = "Helicopter"),
    PLANE               UMETA(DisplayName = "Plane"),
    SERVICE             UMETA(DisplayName = "Service"),
    EMERGENCY           UMETA(DisplayName = "Emergency"),
    MILITARY            UMETA(DisplayName = "Military"),
    COMMERCIAL          UMETA(DisplayName = "Commercial"),
    TRAIN               UMETA(DisplayName = "Train"),
    INVALID             UMETA(DisplayName = "Invalid")
};

// ========== OBJECT ANIMATION ENUMERATIONS ==========

UENUM()
enum class EObjectAnimationType : uint8
{
    IDLE                UMETA(DisplayName = "Idle"),
    OPEN                UMETA(DisplayName = "Open"),
    CLOSE               UMETA(DisplayName = "Close"),
    ACTIVATE            UMETA(DisplayName = "Activate"),
    DEACTIVATE          UMETA(DisplayName = "Deactivate"),
    BREAK               UMETA(DisplayName = "Break"),
    EXPLODE             UMETA(DisplayName = "Explode"),
    ROTATE              UMETA(DisplayName = "Rotate"),
    SCALE               UMETA(DisplayName = "Scale"),
    MOVE                UMETA(DisplayName = "Move"),
    PULSE               UMETA(DisplayName = "Pulse"),
    FLICKER             UMETA(DisplayName = "Flicker"),
    INVALID             UMETA(DisplayName = "Invalid")
};
