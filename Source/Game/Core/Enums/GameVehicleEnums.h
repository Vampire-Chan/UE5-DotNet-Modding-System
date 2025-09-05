#pragma once

#include "CoreMinimal.h"
#include "GameVehicleEnums.generated.h"

UENUM(BlueprintType)
enum class EVehicleEngineSoundKey : uint8
{
    // --- Core Engine States ---
    EngineStart         UMETA(DisplayName = "Engine Start"),
    EngineStop          UMETA(DisplayName = "Engine Stop"),
    StartingFailed      UMETA(DisplayName = "Starting Failed"),
    IdleLoop            UMETA(DisplayName = "Idle Loop"),
    Stall               UMETA(DisplayName = "Stall"),

    // --- RPM Loops ---
    LowRPMLoop          UMETA(DisplayName = "Low RPM Loop"),
    MidRPMLoop          UMETA(DisplayName = "Mid RPM Loop"),
    HighRPMLoop         UMETA(DisplayName = "High RPM Loop"),

    // --- Effects & Damage ---
    Backfire            UMETA(DisplayName = "Backfire"),
    EngineDamage        UMETA(DisplayName = "Engine Damage Loop"),
    FuelPump            UMETA(DisplayName = "Fuel Pump Loop"),

    // --- Bike Specific ---
    ExhaustPop          UMETA(DisplayName = "Exhaust Pop"),
    ChainNoise          UMETA(DisplayName = "Chain Noise Loop"),
    SuspensionBottomOut UMETA(DisplayName = "Suspension Bottom Out"),

    // --- Helicopter Specific ---
    RotorWash           UMETA(DisplayName = "Rotor Wash Loop"),
    TailRotor           UMETA(DisplayName = "Tail Rotor Loop"),
    TurbineWhine        UMETA(DisplayName = "Turbine Whine Loop"),
    SkidScrape          UMETA(DisplayName = "Skid Scrape"),

    // --- Plane Specific ---
    PropellerWash       UMETA(DisplayName = "Propeller Wash Loop"),
    JetEngineWhine      UMETA(DisplayName = "Jet Engine Whine Loop"),
    Afterburner         UMETA(DisplayName = "Afterburner"),
    LandingGear         UMETA(DisplayName = "Landing Gear"),
    Flaps               UMETA(DisplayName = "Flaps"),
    Touchdown           UMETA(DisplayName = "Touchdown"),

    Unknown             UMETA(DisplayName = "Unknown")
};
