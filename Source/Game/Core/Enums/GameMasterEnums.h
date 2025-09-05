#pragma once

#include "CoreMinimal.h"

// ========== MASTER GAME ENUMERATIONS ==========
// This file includes all enumeration headers for easy access

#include "GameAudioEnums.h"
#include "GameAnimationEnums.h"
#include "GameWorldEnums.h"
#include "GameWeaponEnums.h"
#include "GameVehicleEnums.h"

#include "GameMasterEnums.generated.h"

// ========== GENERAL GAME STATE ENUMERATIONS ==========

UENUM()
enum class EGameMode : uint8
{
    SANDBOX             UMETA(DisplayName = "Sandbox"),
    FREEMODE            UMETA(DisplayName = "Freemode"),
    MISSION             UMETA(DisplayName = "Mission"),
    DEATHMATCH          UMETA(DisplayName = "Deathmatch"),
    RACE                UMETA(DisplayName = "Race"),
    HEIST               UMETA(DisplayName = "Heist"),
    SURVIVAL            UMETA(DisplayName = "Survival"),
    CREATIVE            UMETA(DisplayName = "Creative"),
    INVALID             UMETA(DisplayName = "Invalid")
};

UENUM()
enum class EPlayerState : uint8
{
    SPAWNING            UMETA(DisplayName = "Spawning"),
    ALIVE               UMETA(DisplayName = "Alive"),
    DEAD                UMETA(DisplayName = "Dead"),
    SPECTATING          UMETA(DisplayName = "Spectating"),
    IN_VEHICLE          UMETA(DisplayName = "In Vehicle"),
    IN_BUILDING         UMETA(DisplayName = "In Building"),
    IN_WATER            UMETA(DisplayName = "In Water"),
    PARACHUTING         UMETA(DisplayName = "Parachuting"),
    RAGDOLL             UMETA(DisplayName = "Ragdoll"),
    INVALID             UMETA(DisplayName = "Invalid")
};

UENUM()
enum class ENetworkRole : uint8
{
    HOST                UMETA(DisplayName = "Host"),
    CLIENT              UMETA(DisplayName = "Client"),
    ADMIN               UMETA(DisplayName = "Admin"),
    MODERATOR           UMETA(DisplayName = "Moderator"),
    PLAYER              UMETA(DisplayName = "Player"),
    SPECTATOR           UMETA(DisplayName = "Spectator"),
    BANNED              UMETA(DisplayName = "Banned"),
    INVALID             UMETA(DisplayName = "Invalid")
};

// ========== PHYSICS & INTERACTION ENUMERATIONS ==========

UENUM()
enum class EPhysicsInteractionType : uint8
{
    NONE                UMETA(DisplayName = "None"),
    PICKUP              UMETA(DisplayName = "Pickup"),
    PUSH                UMETA(DisplayName = "Push"),
    PULL                UMETA(DisplayName = "Pull"),
    THROW               UMETA(DisplayName = "Throw"),
    WELD                UMETA(DisplayName = "Weld"),
    FREEZE              UMETA(DisplayName = "Freeze"),
    DUPLICATE           UMETA(DisplayName = "Duplicate"),
    DELETE              UMETA(DisplayName = "Delete"),
    INVALID             UMETA(DisplayName = "Invalid")
};

UENUM()
enum class EToolType : uint8
{
    PHYSICS_GUN         UMETA(DisplayName = "Physics Gun"),
    WELDING_TOOL        UMETA(DisplayName = "Welding Tool"),
    THRUSTER_TOOL       UMETA(DisplayName = "Thruster Tool"),
    WIRE_TOOL           UMETA(DisplayName = "Wire Tool"),
    DUPLICATOR          UMETA(DisplayName = "Duplicator"),
    PAINT_TOOL          UMETA(DisplayName = "Paint Tool"),
    MATERIAL_TOOL       UMETA(DisplayName = "Material Tool"),
    REMOVER_TOOL        UMETA(DisplayName = "Remover Tool"),
    CONSTRAINT_TOOL     UMETA(DisplayName = "Constraint Tool"),
    BALLOON_TOOL        UMETA(DisplayName = "Balloon Tool"),
    INVALID             UMETA(DisplayName = "Invalid")
};

// ========== SYSTEM STATUS ENUMERATIONS ==========

UENUM()
enum class ESystemStatus : uint8
{
    OFFLINE             UMETA(DisplayName = "Offline"),
    INITIALIZING        UMETA(DisplayName = "Initializing"),
    LOADING             UMETA(DisplayName = "Loading"),
    READY               UMETA(DisplayName = "Ready"),
    RUNNING             UMETA(DisplayName = "Running"),
    PAUSED              UMETA(DisplayName = "Paused"),
    ERROR               UMETA(DisplayName = "Error"),
    SHUTTING_DOWN       UMETA(DisplayName = "Shutting Down"),
    INVALID             UMETA(DisplayName = "Invalid")
};

UENUM()
enum class ELogLevel : uint8
{
    VERBOSE             UMETA(DisplayName = "Verbose"),
    LOG                 UMETA(DisplayName = "Log"),
    WARNING             UMETA(DisplayName = "Warning"),
    ERROR               UMETA(DisplayName = "Error"),
    FATAL               UMETA(DisplayName = "Fatal"),
    INVALID             UMETA(DisplayName = "Invalid")
};
