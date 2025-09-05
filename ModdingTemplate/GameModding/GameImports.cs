using System;
using System.Runtime.InteropServices;

namespace GameModding
{
    /// <summary>
    /// SIMPLE C# imports from the C++ plugin DLL
    /// Just like GTA ScriptHook pattern - direct DLL imports!
    /// </summary>
    internal static class GameImports
    {
        // The DLL name that contains our exported functions
        private const string GameDLL = "UnrealEditor-DotNetScripting"; // The plugin DLL

        // ═══════════════════════════════════════════════════════════════
        // LOGGING FUNCTIONS - With proper string marshaling
        // ═══════════════════════════════════════════════════════════════
        
        [DllImport(GameDLL, CallingConvention = CallingConvention.Cdecl, CharSet = CharSet.Ansi)]
        private static extern void Game_Log_Native(IntPtr message);
        
        [DllImport(GameDLL, CallingConvention = CallingConvention.Cdecl, CharSet = CharSet.Ansi)]
        private static extern void Game_LogWarning_Native(IntPtr message);
        
        [DllImport(GameDLL, CallingConvention = CallingConvention.Cdecl, CharSet = CharSet.Ansi)]
        private static extern void Game_LogError_Native(IntPtr message);

        // Safe string wrappers
        internal static void Game_Log(string message)
        {
            TypeConversions.WithCString(message, ptr => Game_Log_Native(ptr));
        }

        internal static void Game_LogWarning(string message)
        {
            TypeConversions.WithCString(message, ptr => Game_LogWarning_Native(ptr));
        }

        internal static void Game_LogError(string message)
        {
            TypeConversions.WithCString(message, ptr => Game_LogError_Native(ptr));
        }

        // ═══════════════════════════════════════════════════════════════
        // WORLD/PLAYER FUNCTIONS - With proper UE type handling
        // ═══════════════════════════════════════════════════════════════
        
        [DllImport(GameDLL, CallingConvention = CallingConvention.Cdecl)]
        private static extern void World_GetPlayerPosition_Native(out TypeConversions.FVector3f position);
        
        [DllImport(GameDLL, CallingConvention = CallingConvention.Cdecl)]
        private static extern void World_SetPlayerPosition_Native(TypeConversions.FVector3f position);
        
        [DllImport(GameDLL, CallingConvention = CallingConvention.Cdecl)]
        internal static extern int World_GetPedCount();

        // Safe wrappers with type conversion
        internal static void World_GetPlayerPosition(out float x, out float y, out float z)
        {
            World_GetPlayerPosition_Native(out TypeConversions.FVector3f pos);
            x = pos.X;
            y = pos.Y;
            z = pos.Z;
        }

        internal static void World_SetPlayerPosition(float x, float y, float z)
        {
            World_SetPlayerPosition_Native(new TypeConversions.FVector3f(x, y, z));
        }

        // ═══════════════════════════════════════════════════════════════
        // PED FACTORY FUNCTIONS - With proper string and handle conversion
        // ═══════════════════════════════════════════════════════════════
        
        [DllImport(GameDLL, CallingConvention = CallingConvention.Cdecl)]
        private static extern IntPtr PedFactory_Spawn_Native(IntPtr characterName, IntPtr variation, 
                                                           TypeConversions.FVector3f position, 
                                                           TypeConversions.FRotator rotation);
        
        [DllImport(GameDLL, CallingConvention = CallingConvention.Cdecl)]
        private static extern IntPtr PedFactory_SpawnModularCharacter_Native(IntPtr characterPath,
                                                                            IntPtr headVariation,
                                                                            IntPtr upperVariation,
                                                                            IntPtr lowerVariation,
                                                                            IntPtr feetVariation,
                                                                            IntPtr handVariation,
                                                                            TypeConversions.FVector3f position,
                                                                            TypeConversions.FRotator rotation);
        
        [DllImport(GameDLL, CallingConvention = CallingConvention.Cdecl)]
        [return: MarshalAs(UnmanagedType.I1)]
        internal static extern bool PedFactory_Remove(IntPtr pedHandle);
        
        [DllImport(GameDLL, CallingConvention = CallingConvention.Cdecl)]
        [return: MarshalAs(UnmanagedType.I1)]
        internal static extern bool PedFactory_IsValid(IntPtr pedHandle);
        
        [DllImport(GameDLL, CallingConvention = CallingConvention.Cdecl)]
        [return: MarshalAs(UnmanagedType.I1)]
        internal static extern bool PedFactory_Possess(IntPtr pedHandle);
        
        [DllImport(GameDLL, CallingConvention = CallingConvention.Cdecl)]
        [return: MarshalAs(UnmanagedType.I1)]
        internal static extern bool PedFactory_Unpossess();
        
        [DllImport(GameDLL, CallingConvention = CallingConvention.Cdecl)]
        internal static extern IntPtr PedFactory_GetPlayerPed();

        // Safe wrapper with string and type conversion
        internal static IntPtr PedFactory_Spawn(string characterName, string variation, 
                                              float x, float y, float z, float heading)
        {
            IntPtr result = IntPtr.Zero;
            
            TypeConversions.WithCString(characterName, namePtr =>
            {
                TypeConversions.WithCString(variation, varPtr =>
                {
                    var position = new TypeConversions.FVector3f(x, y, z);
                    var rotation = new TypeConversions.FRotator(0, heading, 0);
                    result = PedFactory_Spawn_Native(namePtr, varPtr, position, rotation);
                });
            });
            
            return result;
        }

        // Safe wrapper for modular character spawning
        internal static IntPtr PedFactory_SpawnModularCharacter(string characterPath,
                                                              string headVariation = "000",
                                                              string upperVariation = "000", 
                                                              string lowerVariation = "000",
                                                              string feetVariation = "000",
                                                              string handVariation = "000",
                                                              float x = 0, float y = 0, float z = 0,
                                                              float pitch = 0, float yaw = 0, float roll = 0)
        {
            IntPtr result = IntPtr.Zero;
            
            TypeConversions.WithCString(characterPath, pathPtr =>
            {
                TypeConversions.WithCString(headVariation, headPtr =>
                {
                    TypeConversions.WithCString(upperVariation, upperPtr =>
                    {
                        TypeConversions.WithCString(lowerVariation, lowerPtr =>
                        {
                            TypeConversions.WithCString(feetVariation, feetPtr =>
                            {
                                TypeConversions.WithCString(handVariation, handPtr =>
                                {
                                    var position = new TypeConversions.FVector3f(x, y, z);
                                    var rotation = new TypeConversions.FRotator(pitch, yaw, roll);
                                    result = PedFactory_SpawnModularCharacter_Native(pathPtr, headPtr, upperPtr, 
                                                                                   lowerPtr, feetPtr, handPtr, 
                                                                                   position, rotation);
                                });
                            });
                        });
                    });
                });
            });
            
            return result;
        }
        // ═══════════════════════════════════════════════════════════════
        // PED CONTROL FUNCTIONS - With UE type conversions
        // ═══════════════════════════════════════════════════════════════
        
        [DllImport(GameDLL, CallingConvention = CallingConvention.Cdecl)]
        private static extern void Ped_GetPosition_Native(IntPtr pedHandle, out TypeConversions.FVector3f position);
        
        [DllImport(GameDLL, CallingConvention = CallingConvention.Cdecl)]
        private static extern void Ped_SetPosition_Native(IntPtr pedHandle, TypeConversions.FVector3f position);
        
        [DllImport(GameDLL, CallingConvention = CallingConvention.Cdecl)]
        private static extern TypeConversions.FRotator Ped_GetRotation_Native(IntPtr pedHandle);
        
        [DllImport(GameDLL, CallingConvention = CallingConvention.Cdecl)]
        private static extern void Ped_SetRotation_Native(IntPtr pedHandle, TypeConversions.FRotator rotation);

        // Safe wrappers with type conversion
        internal static void Ped_GetPosition(IntPtr pedHandle, out float x, out float y, out float z)
        {
            Ped_GetPosition_Native(pedHandle, out TypeConversions.FVector3f pos);
            x = pos.X;
            y = pos.Y;
            z = pos.Z;
        }

        internal static void Ped_SetPosition(IntPtr pedHandle, float x, float y, float z)
        {
            Ped_SetPosition_Native(pedHandle, new TypeConversions.FVector3f(x, y, z));
        }

        internal static float Ped_GetHeading(IntPtr pedHandle)
        {
            TypeConversions.FRotator rotation = Ped_GetRotation_Native(pedHandle);
            return (float)rotation.Yaw;
        }

        internal static void Ped_SetHeading(IntPtr pedHandle, float heading)
        {
            Ped_SetRotation_Native(pedHandle, new TypeConversions.FRotator(0, heading, 0));
        }

        // ═══════════════════════════════════════════════════════════════
        // UTILITY FUNCTIONS - With proper type conversions
        // ═══════════════════════════════════════════════════════════════
        
        [DllImport(GameDLL, CallingConvention = CallingConvention.Cdecl)]
        private static extern float Math_Distance_Native(TypeConversions.FVector3f pos1, TypeConversions.FVector3f pos2);
        
        [DllImport(GameDLL, CallingConvention = CallingConvention.Cdecl)]
        private static extern float Math_Distance2D_Native(TypeConversions.FVector3f pos1, TypeConversions.FVector3f pos2);
        
        [DllImport(GameDLL, CallingConvention = CallingConvention.Cdecl)]
        private static extern TypeConversions.FVector3f Math_RandomPosition_Native(TypeConversions.FVector3f center, float radius);

        // Safe wrappers
        internal static float Math_Distance(float x1, float y1, float z1, float x2, float y2, float z2)
        {
            var pos1 = new TypeConversions.FVector3f(x1, y1, z1);
            var pos2 = new TypeConversions.FVector3f(x2, y2, z2);
            return Math_Distance_Native(pos1, pos2);
        }

        internal static float Math_Distance2D(float x1, float y1, float x2, float y2)
        {
            var pos1 = new TypeConversions.FVector3f(x1, y1, 0);
            var pos2 = new TypeConversions.FVector3f(x2, y2, 0);
            return Math_Distance2D_Native(pos1, pos2);
        }

        internal static void Math_RandomPosition(float centerX, float centerY, float centerZ, 
                                               float radius, out float outX, out float outY, out float outZ)
        {
            var center = new TypeConversions.FVector3f(centerX, centerY, centerZ);
            TypeConversions.FVector3f result = Math_RandomPosition_Native(center, radius);
            outX = result.X;
            outY = result.Y;
            outZ = result.Z;
        }
    }
}
