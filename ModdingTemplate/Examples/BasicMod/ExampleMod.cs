using System;
using GameModding;

namespace BasicMod
{
    /// <summary>
    /// A basic example mod showing how to use the GameModding API
    /// </summary>
    public class ExampleMod
    {
        private float _timer = 0f;
        private int _spawnedPeds = 0;

        /// <summary>
        /// Called when the mod is loaded
        /// </summary>
        public void Initialize()
        {
            Game.Log.Info("BasicMod: Hello from C#! Mod initialized successfully.");
            Game.Log.Info($"BasicMod: Current ped count in world: {Game.World.PedCount}");
            
            // Log current player position
            var playerPos = Game.World.PlayerPosition;
            Game.Log.Info($"BasicMod: Player position: X={playerPos.X:F2}, Y={playerPos.Y:F2}, Z={playerPos.Z:F2}");
        }

        /// <summary>
        /// Called every frame
        /// </summary>
        public void Tick(float deltaTime)
        {
            _timer += deltaTime;

            // Every 5 seconds, spawn a cop and log info
            if (_timer >= 5.0f)
            {
                _timer = 0f;
                SpawnExamplePed();
            }
        }

        /// <summary>
        /// Called when the mod is unloaded
        /// </summary>
        public void Cleanup()
        {
            Game.Log.Info($"BasicMod: Cleaning up. Spawned {_spawnedPeds} peds during this session.");
        }

        private void SpawnExamplePed()
        {
            var playerPos = Game.World.PlayerPosition;
            
            // Spawn a cop near the player
            var spawnPos = new Vector3(
                playerPos.X + (float)(new Random().NextDouble() - 0.5) * 1000f,
                playerPos.Y + (float)(new Random().NextDouble() - 0.5) * 1000f,
                playerPos.Z
            );

            var ped = Game.PedFactory.Spawn("m_y_cop", "default", spawnPos, 0f);
            
            if (ped != null)
            {
                _spawnedPeds++;
                Game.Log.Info($"BasicMod: Spawned cop #{_spawnedPeds} at {spawnPos.X:F0}, {spawnPos.Y:F0}, {spawnPos.Z:F0}");
            }
            else
            {
                Game.Log.Warning("BasicMod: Failed to spawn cop");
            }
        }
    }
}
