#pragma once

#include "CoreMinimal.h"

// ========== BITWISE AUDIO FLAGS ==========

UENUM(meta = (Bitflags, UseEnumValuesAsMaskValuesInEditor = "true"))
enum class EAudioFlags : uint32
{
    NONE                = 0,
    LOOP                = 1 << 0,   // 0x00000001 - Loop audio
    SPATIAL_3D          = 1 << 1,   // 0x00000002 - 3D spatial audio
    SPATIAL_2D          = 1 << 2,   // 0x00000004 - 2D flat audio
    REVERB_ENABLED      = 1 << 3,   // 0x00000008 - Apply reverb
    ECHO_ENABLED        = 1 << 4,   // 0x00000010 - Apply echo
    WALL_MUFFLED        = 1 << 5,   // 0x00000020 - Muffled by walls
    DISTANCE_FADE       = 1 << 6,   // 0x00000040 - Fade with distance
    HIGH_PRIORITY       = 1 << 7,   // 0x00000080 - High priority sound
    LOW_PRIORITY        = 1 << 8,   // 0x00000100 - Low priority sound
    RANDOMIZE_PITCH     = 1 << 9,   // 0x00000200 - Random pitch variation
    RANDOMIZE_VOLUME    = 1 << 10,  // 0x00000400 - Random volume variation
    UNDERWATER          = 1 << 11,  // 0x00000800 - Underwater filter
    INTERIOR            = 1 << 12,  // 0x00001000 - Interior reverb
    EXTERIOR            = 1 << 13,  // 0x00002000 - Exterior ambience
    COMBAT_FOCUS        = 1 << 14,  // 0x00004000 - Combat audio focus
    STEALTH_MODE        = 1 << 15,  // 0x00008000 - Stealth audio mode
    ATTACHED_TO_OBJECT  = 1 << 16,  // 0x00010000 - Sound follows attached object
    STOP_ON_DESTROY     = 1 << 17,  // 0x00020000 - Stop when attached object destroyed
    DETACH_ON_DISTANCE  = 1 << 18,  // 0x00040000 - Detach if object moves too far
    FOLLOW_OBJECT       = 1 << 19,  // 0x00080000 - Sound position follows object transform
    FADE_ON_DETACH      = 1 << 20,  // 0x00100000 - Fade out when detaching from object
    RESTART_ON_ATTACH   = 1 << 21,  // 0x00200000 - Restart sound when attaching to new object
};
ENUM_CLASS_FLAGS(EAudioFlags);

// ========== WEAPON AUDIO ENUMERATIONS ==========

UENUM()
enum class EWeaponType : uint8
{
    MELEE               UMETA(DisplayName = "Melee"),
    HANDGUN             UMETA(DisplayName = "Handgun"),
    RIFLE               UMETA(DisplayName = "Rifle"),
    SHOTGUN             UMETA(DisplayName = "Shotgun"),
    SNIPER              UMETA(DisplayName = "Sniper"),
    MACHINE_GUN         UMETA(DisplayName = "Machine Gun"),
    LAUNCHER            UMETA(DisplayName = "Launcher"),
    THROWABLE           UMETA(DisplayName = "Throwable"),
    SPECIAL             UMETA(DisplayName = "Special"),
    INVALID             UMETA(DisplayName = "Invalid")
};

UENUM()
enum class EWeaponAction : uint8
{
    FIRE                UMETA(DisplayName = "Fire"),
    RELOAD              UMETA(DisplayName = "Reload"),
    DRAW                UMETA(DisplayName = "Draw"),
    HOLSTER             UMETA(DisplayName = "Holster"),
    DRYFIRE             UMETA(DisplayName = "Dry Fire"),
    CHARGE              UMETA(DisplayName = "Charge"),
    COCK                UMETA(DisplayName = "Cock"),
    SAFETY              UMETA(DisplayName = "Safety"),
    IMPACT              UMETA(DisplayName = "Impact"),
    WHIZBY              UMETA(DisplayName = "Whiz By"),
    INVALID             UMETA(DisplayName = "Invalid")
};

UENUM()
enum class EMaterialType : uint8
{
    CONCRETE            UMETA(DisplayName = "Concrete"),
    METAL               UMETA(DisplayName = "Metal"),
    WOOD                UMETA(DisplayName = "Wood"),
    GLASS               UMETA(DisplayName = "Glass"),
    FLESH               UMETA(DisplayName = "Flesh"),
    WATER               UMETA(DisplayName = "Water"),
    DIRT                UMETA(DisplayName = "Dirt"),
    GRASS               UMETA(DisplayName = "Grass"),
    SAND                UMETA(DisplayName = "Sand"),
    FABRIC              UMETA(DisplayName = "Fabric"),
    PLASTIC             UMETA(DisplayName = "Plastic"),
    RUBBER              UMETA(DisplayName = "Rubber"),
    CERAMIC             UMETA(DisplayName = "Ceramic"),
    SNOW                UMETA(DisplayName = "Snow"),
    ICE                 UMETA(DisplayName = "Ice"),
    INVALID             UMETA(DisplayName = "Invalid")
};

UENUM()
enum class EEnvironmentType : uint8
{
    INDOOR_SMALL        UMETA(DisplayName = "Indoor Small"),
    INDOOR_LARGE        UMETA(DisplayName = "Indoor Large"),
    OUTDOOR_OPEN        UMETA(DisplayName = "Outdoor Open"),
    OUTDOOR_URBAN       UMETA(DisplayName = "Outdoor Urban"),
    TUNNEL              UMETA(DisplayName = "Tunnel"),
    SEWER               UMETA(DisplayName = "Sewer"),
    FOREST              UMETA(DisplayName = "Forest"),
    DESERT              UMETA(DisplayName = "Desert"),
    MOUNTAIN            UMETA(DisplayName = "Mountain"),
    CAVE                UMETA(DisplayName = "Cave"),
    UNDERWATER          UMETA(DisplayName = "Underwater"),
    INVALID             UMETA(DisplayName = "Invalid")
};

// ========== AUDIO PLAYBACK ENUMERATIONS ==========

UENUM()
enum class EAudioPlaybackMode : uint8
{
    DIRECT_PLAY         UMETA(DisplayName = "Direct Play"),
    SOUND_CUE           UMETA(DisplayName = "Sound Cue"),
    AUDIO_COMPONENT     UMETA(DisplayName = "Audio Component"),
    SPATIAL_3D          UMETA(DisplayName = "Spatial 3D"),
    UI_2D               UMETA(DisplayName = "UI 2D"),
    INVALID             UMETA(DisplayName = "Invalid")
};

UENUM()
enum class EAudioPriority : uint8
{
    CRITICAL            UMETA(DisplayName = "Critical"),
    HIGH                UMETA(DisplayName = "High"),
    MEDIUM              UMETA(DisplayName = "Medium"),
    LOW                 UMETA(DisplayName = "Low"),
    BACKGROUND          UMETA(DisplayName = "Background"),
    INVALID             UMETA(DisplayName = "Invalid")
};

UENUM()
enum class EAudioCategory : uint8
{
    MASTER              UMETA(DisplayName = "Master"),
    SFX                 UMETA(DisplayName = "SFX"),
    MUSIC               UMETA(DisplayName = "Music"),
    VOICE               UMETA(DisplayName = "Voice"),
    AMBIENCE            UMETA(DisplayName = "Ambience"),
    UI                  UMETA(DisplayName = "UI"),
    ENGINE              UMETA(DisplayName = "Engine"),
    WEAPON              UMETA(DisplayName = "Weapon"),
    FOOTSTEP            UMETA(DisplayName = "Footstep"),
    INVALID             UMETA(DisplayName = "Invalid")
};
