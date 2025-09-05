using System;
using GameModding;

namespace PlayerNikoExample
{
    public class PlayerNikoSpawningMod
    {
        public static void OnLoad()
        {
            Game.Log.Info("PlayerNiko Spawning Example Mod Loaded!");
            
            try
            {
                // Test basic PlayerNiko spawning
                TestPlayerNikoSpawning();
                
                // Test modular character spawning with variations
                TestModularCharacterSpawning();
                
                Game.Log.Info("PlayerNiko spawning tests completed!");
            }
            catch (Exception ex)
            {
                Game.Log.Error($"Error in PlayerNiko mod: {ex.Message}");
            }
        }
        
        private static void TestPlayerNikoSpawning()
        {
            Game.Log.Info("=== Testing PlayerNiko Basic Spawning ===");
            
            // Spawn PlayerNiko at a test location
            Vector3 spawnPos = new Vector3(0, 0, 100); // 100 units above origin
            
            var niko = Game.PedFactory.SpawnPlayerNiko(spawnPos, 0f);
            if (niko != null)
            {
                Game.Log.Info($"Successfully spawned PlayerNiko at {spawnPos}");
                Game.Log.Info($"PlayerNiko is valid: {niko.IsValid}");
                
                // Test position modification
                niko.Position = new Vector3(50, 50, 100);
                Game.Log.Info($"Moved PlayerNiko to: {niko.Position}");
            }
            else
            {
                Game.Log.Warning("Failed to spawn PlayerNiko - this is expected as C++ implementation is not complete yet");
            }
        }
        
        private static void TestModularCharacterSpawning()
        {
            Game.Log.Info("=== Testing Modular Character Spawning ===");
            
            Vector3 spawnPos = new Vector3(100, 100, 100);
            Vector3 rotation = new Vector3(0, 90, 0); // Face east
            
            // Test spawning with specific variations
            var modularNiko = Game.PedFactory.SpawnModularCharacter(
                "PlayerNiko", 
                spawnPos,
                headVariation: "000",    // Default head
                upperVariation: "000",   // Default upper body  
                lowerVariation: "000",   // Default lower body
                feetVariation: "000",    // Default feet
                handVariation: "000",    // Default hands
                rotation: rotation
            );
            
            if (modularNiko != null)
            {
                Game.Log.Info($"Successfully spawned modular PlayerNiko at {spawnPos}");
                
                // Test various operations
                modularNiko.Heading = 180f; // Turn around
                Game.Log.Info($"Set PlayerNiko heading to: {modularNiko.Heading}");
                
                // Test distance calculation
                Vector3 testPoint = new Vector3(200, 200, 100);
                float distance = modularNiko.DistanceTo(testPoint);
                Game.Log.Info($"Distance from PlayerNiko to test point: {distance}");
            }
            else
            {
                Game.Log.Warning("Failed to spawn modular PlayerNiko - this is expected as C++ implementation is not complete yet");
            }
        }
    }
}
