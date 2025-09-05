using System;

namespace GameModding
{
    /// <summary>
    /// SIMPLE game modding API - like GTA ScriptHook pattern
    /// Clean, easy-to-use C# wrapper around the C++ plugin exports
    /// </summary>
    public static class Game
    {
        /// <summary>
        /// Game logging system for debugging
        /// </summary>
        public static class Log
        {
            public static void Info(string message) => GameImports.Game_Log($"[MOD] {message}");
            public static void Warning(string message) => GameImports.Game_LogWarning($"[MOD] {message}");
            public static void Error(string message) => GameImports.Game_LogError($"[MOD] {message}");
        }
        
        /// <summary>
        /// Ped (NPC) management factory
        /// </summary>
        public static class PedFactory
        {
            /// <summary>
            /// Spawn a new ped in the world
            /// </summary>
            /// <param name="characterName">Character type (e.g., "m_y_cop")</param>
            /// <param name="variation">Character variation</param>
            /// <param name="position">World position</param>
            /// <param name="heading">Rotation (yaw)</param>
            /// <returns>Ped instance or null if failed</returns>
            public static Ped? Spawn(string characterName, string variation, Vector3 position, float heading = 0f)
            {
                var pedPtr = GameImports.PedFactory_Spawn(characterName, variation, 
                    position.X, position.Y, position.Z, heading);
                    
                return pedPtr == IntPtr.Zero ? null : new Ped(pedPtr);
            }
            
            /// <summary>
            /// Spawn a modular character (like PlayerNiko) with specific component variations
            /// </summary>
            /// <param name="characterPath">Base character path (e.g., "PlayerNiko")</param>
            /// <param name="position">World position</param>
            /// <param name="headVariation">Head variation (default: "000")</param>
            /// <param name="upperVariation">Upper body variation (default: "000")</param>
            /// <param name="lowerVariation">Lower body variation (default: "000")</param>
            /// <param name="feetVariation">Feet variation (default: "000")</param>
            /// <param name="handVariation">Hand variation (default: "000")</param>
            /// <param name="rotation">Full rotation (pitch, yaw, roll)</param>
            /// <returns>Ped instance or null if failed</returns>
            public static Ped? SpawnModularCharacter(string characterPath, Vector3 position,
                                                   string headVariation = "000",
                                                   string upperVariation = "000", 
                                                   string lowerVariation = "000",
                                                   string feetVariation = "000",
                                                   string handVariation = "000",
                                                   Vector3? rotation = null)
            {
                var rot = rotation ?? new Vector3(0, 0, 0);
                var pedPtr = GameImports.PedFactory_SpawnModularCharacter(characterPath,
                    headVariation, upperVariation, lowerVariation, feetVariation, handVariation,
                    position.X, position.Y, position.Z, rot.X, rot.Y, rot.Z);
                    
                return pedPtr == IntPtr.Zero ? null : new Ped(pedPtr);
            }
            
            /// <summary>
            /// Spawn PlayerNiko with default variations at specified position
            /// </summary>
            /// <param name="position">World position</param>
            /// <param name="heading">Rotation (yaw only)</param>
            /// <returns>Ped instance or null if failed</returns>
            public static Ped? SpawnPlayerNiko(Vector3 position, float heading = 0f)
            {
                return SpawnModularCharacter("PlayerNiko", position, 
                                           rotation: new Vector3(0, heading, 0));
            }
            
            /// <summary>
            /// Remove a ped from the world
            /// </summary>
            public static bool Remove(Ped? ped)
            {
                if (ped?.IsValid != true) return false;
                return GameImports.PedFactory_Remove(ped.Handle);
            }
            
            /// <summary>
            /// Take control of a ped (possess it)
            /// </summary>
            public static bool Possess(Ped? ped)
            {
                if (ped?.IsValid != true) return false;
                return GameImports.PedFactory_Possess(ped.Handle);
            }
            
            /// <summary>
            /// Stop controlling any ped
            /// </summary>
            public static bool Unpossess() => GameImports.PedFactory_Unpossess();
            
            /// <summary>
            /// Get the currently controlled ped
            /// </summary>
            public static Ped? GetPlayerPed()
            {
                var pedPtr = GameImports.PedFactory_GetPlayerPed();
                return pedPtr == IntPtr.Zero ? null : new Ped(pedPtr);
            }
        }
        
        /// <summary>
        /// World and player information
        /// </summary>
        public static class World
        {
            /// <summary>
            /// Get the current player position
            /// </summary>
            public static Vector3 PlayerPosition
            {
                get
                {
                    GameImports.World_GetPlayerPosition(out float x, out float y, out float z);
                    return new Vector3(x, y, z);
                }
                set => GameImports.World_SetPlayerPosition(value.X, value.Y, value.Z);
            }
            
            /// <summary>
            /// Get the total number of peds in the world
            /// </summary>
            public static int PedCount => GameImports.World_GetPedCount();
        }
        
        /// <summary>
        /// Utility math functions
        /// </summary>
        public static class Math
        {
            /// <summary>
            /// Calculate 3D distance between two points
            /// </summary>
            public static float Distance(Vector3 pos1, Vector3 pos2)
            {
                return GameImports.Math_Distance(pos1.X, pos1.Y, pos1.Z, pos2.X, pos2.Y, pos2.Z);
            }
            
            /// <summary>
            /// Calculate 2D distance between two points (ignoring Z)
            /// </summary>
            public static float Distance2D(Vector3 pos1, Vector3 pos2)
            {
                return GameImports.Math_Distance2D(pos1.X, pos1.Y, pos2.X, pos2.Y);
            }
            
            /// <summary>
            /// Get a random position around a center point
            /// </summary>
            public static Vector3 RandomPosition(Vector3 center, float radius)
            {
                GameImports.Math_RandomPosition(center.X, center.Y, center.Z, radius, 
                    out float x, out float y, out float z);
                return new Vector3(x, y, z);
            }
        }
    }

    /// <summary>
    /// Represents a ped (NPC) in the game world
    /// </summary>
    public class Ped
    {
        internal IntPtr Handle { get; }

        internal Ped(IntPtr handle)
        {
            Handle = handle;
        }

        /// <summary>
        /// Check if this ped handle is still valid
        /// </summary>
        public bool IsValid => GameImports.PedFactory_IsValid(Handle);

        /// <summary>
        /// Get or set the ped's position in the world
        /// </summary>
        public Vector3 Position
        {
            get
            {
                GameImports.Ped_GetPosition(Handle, out float x, out float y, out float z);
                return new Vector3(x, y, z);
            }
            set => GameImports.Ped_SetPosition(Handle, value.X, value.Y, value.Z);
        }

        /// <summary>
        /// Get or set the ped's heading (rotation around Z axis)
        /// </summary>
        public float Heading
        {
            get => GameImports.Ped_GetHeading(Handle);
            set => GameImports.Ped_SetHeading(Handle, value);
        }

        /// <summary>
        /// Remove this ped from the world
        /// </summary>
        public bool Remove()
        {
            return Game.PedFactory.Remove(this);
        }

        /// <summary>
        /// Calculate distance to another position
        /// </summary>
        public float DistanceTo(Vector3 position)
        {
            return Game.Math.Distance(Position, position);
        }

        /// <summary>
        /// Calculate distance to another ped
        /// </summary>
        public float DistanceTo(Ped other)
        {
            return Game.Math.Distance(Position, other.Position);
        }
    }

    /// <summary>
    /// Simple 3D vector structure
    /// </summary>
    public struct Vector3
    {
        public float X, Y, Z;

        public Vector3(float x, float y, float z)
        {
            X = x;
            Y = y;
            Z = z;
        }

        public static Vector3 Zero => new Vector3(0, 0, 0);

        public override string ToString() => $"({X:F2}, {Y:F2}, {Z:F2})";

        // Basic vector operations
        public static Vector3 operator +(Vector3 a, Vector3 b) => new Vector3(a.X + b.X, a.Y + b.Y, a.Z + b.Z);
        public static Vector3 operator -(Vector3 a, Vector3 b) => new Vector3(a.X - b.X, a.Y - b.Y, a.Z - b.Z);
        public static Vector3 operator *(Vector3 v, float scale) => new Vector3(v.X * scale, v.Y * scale, v.Z * scale);
    }
}
