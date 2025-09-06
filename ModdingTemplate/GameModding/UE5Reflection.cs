using System;
using System.Collections.Generic;
using System.Runtime.InteropServices;
using System.Text;

namespace GameModding.Reflection
{
    /// <summary>
    /// UE5 Reflection API Bridge for C# - Provides real-time access to Unreal Engine's reflection system
    /// </summary>
    public static class UE5Reflection
    {
        // =================================================================================
        // NATIVE STRUCTURES
        // =================================================================================

        /// <summary>
        /// Represents a UE5 property accessible from C#
        /// </summary>
        [StructLayout(LayoutKind.Sequential, CharSet = CharSet.Ansi)]
        public struct ReflectionProperty
        {
            [MarshalAs(UnmanagedType.ByValTStr, SizeConst = 256)]
            public string Name;
            [MarshalAs(UnmanagedType.ByValTStr, SizeConst = 128)]
            public string TypeName;
            public ReflectionPropertyType PropertyType;
            public int Offset;
            public int Size;
            [MarshalAs(UnmanagedType.Bool)]
            public bool IsArray;
            [MarshalAs(UnmanagedType.Bool)]
            public bool IsPointer;
            [MarshalAs(UnmanagedType.Bool)]
            public bool IsStruct;
            public IntPtr PropertyPtr;
        }

        /// <summary>
        /// Represents a UE5 function callable from C#
        /// </summary>
        [StructLayout(LayoutKind.Sequential, CharSet = CharSet.Ansi)]
        public struct ReflectionFunction
        {
            [MarshalAs(UnmanagedType.ByValTStr, SizeConst = 256)]
            public string Name;
            [MarshalAs(UnmanagedType.ByValTStr, SizeConst = 128)]
            public string ReturnTypeName;
            public int NumParameters;
            public int ParamsSize;
            [MarshalAs(UnmanagedType.Bool)]
            public bool IsStatic;
            [MarshalAs(UnmanagedType.Bool)]
            public bool IsBlueprintCallable;
            public IntPtr FunctionPtr;
        }

        /// <summary>
        /// Represents a UE5 class accessible from C#
        /// </summary>
        [StructLayout(LayoutKind.Sequential, CharSet = CharSet.Ansi)]
        public struct ReflectionClass
        {
            [MarshalAs(UnmanagedType.ByValTStr, SizeConst = 256)]
            public string Name;
            [MarshalAs(UnmanagedType.ByValTStr, SizeConst = 256)]
            public string ParentName;
            public int ClassSize;
            public int NumProperties;
            public int NumFunctions;
            [MarshalAs(UnmanagedType.Bool)]
            public bool IsActor;
            [MarshalAs(UnmanagedType.Bool)]
            public bool IsComponent;
            [MarshalAs(UnmanagedType.Bool)]
            public bool IsBlueprintable;
            public IntPtr ClassPtr;
        }

        /// <summary>
        /// Property types for reflection
        /// </summary>
        public enum ReflectionPropertyType : int
        {
            Bool = 0,
            Int8,
            Int16,
            Int32,
            Int64,
            UInt8,
            UInt16,
            UInt32,
            UInt64,
            Float,
            Double,
            String,
            Text,
            Name,
            Vector,
            Rotator,
            Transform,
            Color,
            Object,
            Class,
            Struct,
            Array,
            Map,
            Set,
            Enum,
            Unknown
        }

        // =================================================================================
        // NATIVE FUNCTION IMPORTS
        // =================================================================================

        [DllImport("UnrealEditor-DotNetScripting.dll", CallingConvention = CallingConvention.Cdecl)]
        private static extern bool InitializeReflectionSystem();

        [DllImport("UnrealEditor-DotNetScripting.dll", CallingConvention = CallingConvention.Cdecl)]
        private static extern void ShutdownReflectionSystem();

        [DllImport("UnrealEditor-DotNetScripting.dll", CallingConvention = CallingConvention.Cdecl)]
        private static extern int GetAllClasses(
            [Out] ReflectionClass[] outClasses, 
            int maxClasses);

        [DllImport("UnrealEditor-DotNetScripting.dll", CallingConvention = CallingConvention.Cdecl)]
        private static extern bool FindClass(
            [MarshalAs(UnmanagedType.LPStr)] string className, 
            out ReflectionClass outClass);

        [DllImport("UnrealEditor-DotNetScripting.dll", CallingConvention = CallingConvention.Cdecl)]
        private static extern int GetClassProperties(
            [MarshalAs(UnmanagedType.LPStr)] string className, 
            [Out] ReflectionProperty[] outProperties, 
            int maxProperties);

        [DllImport("UnrealEditor-DotNetScripting.dll", CallingConvention = CallingConvention.Cdecl)]
        private static extern int GetClassFunctions(
            [MarshalAs(UnmanagedType.LPStr)] string className, 
            [Out] ReflectionFunction[] outFunctions, 
            int maxFunctions);

        [DllImport("UnrealEditor-DotNetScripting.dll", CallingConvention = CallingConvention.Cdecl)]
        private static extern IntPtr CreateObject(
            [MarshalAs(UnmanagedType.LPStr)] string className, 
            IntPtr outer = default);

        [DllImport("UnrealEditor-DotNetScripting.dll", CallingConvention = CallingConvention.Cdecl)]
        private static extern bool DestroyObject(IntPtr obj);

        [DllImport("UnrealEditor-DotNetScripting.dll", CallingConvention = CallingConvention.Cdecl)]
        private static extern bool GetPropertyValue(
            IntPtr obj, 
            [MarshalAs(UnmanagedType.LPStr)] string propertyName, 
            IntPtr outValue, 
            int valueSize);

        [DllImport("UnrealEditor-DotNetScripting.dll", CallingConvention = CallingConvention.Cdecl)]
        private static extern bool SetPropertyValue(
            IntPtr obj, 
            [MarshalAs(UnmanagedType.LPStr)] string propertyName, 
            IntPtr value, 
            int valueSize);

        [DllImport("UnrealEditor-DotNetScripting.dll", CallingConvention = CallingConvention.Cdecl)]
        private static extern bool CallFunction(
            IntPtr obj, 
            [MarshalAs(UnmanagedType.LPStr)] string functionName, 
            IntPtr parameters, 
            IntPtr returnValue);

        [DllImport("UnrealEditor-DotNetScripting.dll", CallingConvention = CallingConvention.Cdecl)]
        private static extern IntPtr GetCurrentWorld();

        [DllImport("UnrealEditor-DotNetScripting.dll", CallingConvention = CallingConvention.Cdecl)]
        private static extern IntPtr SpawnActor(
            [MarshalAs(UnmanagedType.LPStr)] string className, 
            float x, float y, float z, 
            float pitch, float yaw, float roll);

        [DllImport("UnrealEditor-DotNetScripting.dll", CallingConvention = CallingConvention.Cdecl)]
        private static extern IntPtr FindActorByName(
            [MarshalAs(UnmanagedType.LPStr)] string actorName);

        [DllImport("UnrealEditor-DotNetScripting.dll", CallingConvention = CallingConvention.Cdecl)]
        private static extern int GetAllActorsOfClass(
            [MarshalAs(UnmanagedType.LPStr)] string className, 
            [Out] IntPtr[] outActors, 
            int maxActors);

        [DllImport("UnrealEditor-DotNetScripting.dll", CallingConvention = CallingConvention.Cdecl)]
        private static extern bool GetActorLocation(IntPtr actor, out float x, out float y, out float z);

        [DllImport("UnrealEditor-DotNetScripting.dll", CallingConvention = CallingConvention.Cdecl)]
        private static extern bool SetActorLocation(IntPtr actor, float x, float y, float z);

        [DllImport("UnrealEditor-DotNetScripting.dll", CallingConvention = CallingConvention.Cdecl)]
        private static extern bool GetActorRotation(IntPtr actor, out float pitch, out float yaw, out float roll);

        [DllImport("UnrealEditor-DotNetScripting.dll", CallingConvention = CallingConvention.Cdecl)]
        private static extern bool SetActorRotation(IntPtr actor, float pitch, float yaw, float roll);

        [DllImport("UnrealEditor-DotNetScripting.dll", CallingConvention = CallingConvention.Cdecl)]
        private static extern IntPtr AddComponent(
            IntPtr actor, 
            [MarshalAs(UnmanagedType.LPStr)] string componentClassName);

        [DllImport("UnrealEditor-DotNetScripting.dll", CallingConvention = CallingConvention.Cdecl)]
        private static extern IntPtr GetComponent(
            IntPtr actor, 
            [MarshalAs(UnmanagedType.LPStr)] string componentClassName);

        [DllImport("UnrealEditor-DotNetScripting.dll", CallingConvention = CallingConvention.Cdecl)]
        private static extern bool RemoveComponent(IntPtr actor, IntPtr component);

        [DllImport("UnrealEditor-DotNetScripting.dll", CallingConvention = CallingConvention.Cdecl)]
        private static extern bool GetObjectClassName(
            IntPtr obj, 
            StringBuilder outClassName, 
            int maxLength);

        [DllImport("UnrealEditor-DotNetScripting.dll", CallingConvention = CallingConvention.Cdecl)]
        private static extern bool IsObjectValid(IntPtr obj);

        [DllImport("UnrealEditor-DotNetScripting.dll", CallingConvention = CallingConvention.Cdecl)]
        private static extern void PrintObjectProperties(IntPtr obj);

        [DllImport("UnrealEditor-DotNetScripting.dll", CallingConvention = CallingConvention.Cdecl)]
        private static extern void GetReflectionStats(out int numClasses, out int numProperties, out int numFunctions);

        // =================================================================================
        // HIGH-LEVEL C# API
        // =================================================================================

        private static bool _isInitialized = false;

        /// <summary>
        /// Initialize the UE5 Reflection System
        /// </summary>
        public static bool Initialize()
        {
            if (_isInitialized) return true;

            try
            {
                bool result = InitializeReflectionSystem();
                _isInitialized = result;
                return result;
            }
            catch (Exception ex)
            {
                Console.WriteLine($"Failed to initialize UE5 Reflection: {ex.Message}");
                return false;
            }
        }

        /// <summary>
        /// Shutdown the UE5 Reflection System
        /// </summary>
        public static void Shutdown()
        {
            if (!_isInitialized) return;

            try
            {
                ShutdownReflectionSystem();
                _isInitialized = false;
            }
            catch (Exception ex)
            {
                Console.WriteLine($"Error shutting down UE5 Reflection: {ex.Message}");
            }
        }

        /// <summary>
        /// Get all available classes in the engine
        /// </summary>
        public static List<ReflectionClass> GetAllClasses(int maxClasses = 1000)
        {
            if (!_isInitialized) throw new InvalidOperationException("Reflection system not initialized");

            var classes = new ReflectionClass[maxClasses];
            int count = GetAllClasses(classes, maxClasses);

            var result = new List<ReflectionClass>();
            for (int i = 0; i < count; i++)
            {
                result.Add(classes[i]);
            }

            return result;
        }

        /// <summary>
        /// Find a class by name
        /// </summary>
        public static ReflectionClass? FindClass(string className)
        {
            if (!_isInitialized) throw new InvalidOperationException("Reflection system not initialized");

            if (FindClass(className, out ReflectionClass reflectionClass))
            {
                return reflectionClass;
            }

            return null;
        }

        /// <summary>
        /// Get properties of a class
        /// </summary>
        public static List<ReflectionProperty> GetClassProperties(string className, int maxProperties = 100)
        {
            if (!_isInitialized) throw new InvalidOperationException("Reflection system not initialized");

            var properties = new ReflectionProperty[maxProperties];
            int count = GetClassProperties(className, properties, maxProperties);

            var result = new List<ReflectionProperty>();
            for (int i = 0; i < count; i++)
            {
                result.Add(properties[i]);
            }

            return result;
        }

        /// <summary>
        /// Get functions of a class
        /// </summary>
        public static List<ReflectionFunction> GetClassFunctions(string className, int maxFunctions = 100)
        {
            if (!_isInitialized) throw new InvalidOperationException("Reflection system not initialized");

            var functions = new ReflectionFunction[maxFunctions];
            int count = GetClassFunctions(className, functions, maxFunctions);

            var result = new List<ReflectionFunction>();
            for (int i = 0; i < count; i++)
            {
                result.Add(functions[i]);
            }

            return result;
        }

        /// <summary>
        /// Create an instance of a UE5 class
        /// </summary>
        public static UE5Object? CreateObject(string className, UE5Object? outer = null)
        {
            if (!_isInitialized) throw new InvalidOperationException("Reflection system not initialized");

            IntPtr outerPtr = outer?.Handle ?? IntPtr.Zero;
            IntPtr objPtr = CreateObject(className, outerPtr);

            if (objPtr != IntPtr.Zero)
            {
                return new UE5Object(objPtr);
            }

            return null;
        }

        /// <summary>
        /// Get the current world
        /// </summary>
        public static UE5World? GetWorld()
        {
            if (!_isInitialized) throw new InvalidOperationException("Reflection system not initialized");

            IntPtr worldPtr = GetCurrentWorld();
            if (worldPtr != IntPtr.Zero)
            {
                return new UE5World(worldPtr);
            }

            return null;
        }

        /// <summary>
        /// Spawn an actor in the world
        /// </summary>
        public static UE5Actor? SpawnActor(string className, Vector3 location, Vector3 rotation)
        {
            if (!_isInitialized) throw new InvalidOperationException("Reflection system not initialized");

            IntPtr actorPtr = SpawnActor(className, location.X, location.Y, location.Z, 
                                       rotation.X, rotation.Y, rotation.Z);

            if (actorPtr != IntPtr.Zero)
            {
                return new UE5Actor(actorPtr);
            }

            return null;
        }

        /// <summary>
        /// Find actor by name
        /// </summary>
        public static UE5Actor? FindActorByName(string actorName)
        {
            if (!_isInitialized) throw new InvalidOperationException("Reflection system not initialized");

            IntPtr actorPtr = FindActorByName(actorName);
            if (actorPtr != IntPtr.Zero)
            {
                return new UE5Actor(actorPtr);
            }

            return null;
        }

        /// <summary>
        /// Get all actors of a specific class
        /// </summary>
        public static List<UE5Actor> GetAllActorsOfClass(string className, int maxActors = 100)
        {
            if (!_isInitialized) throw new InvalidOperationException("Reflection system not initialized");

            var actorPtrs = new IntPtr[maxActors];
            int count = GetAllActorsOfClass(className, actorPtrs, maxActors);

            var result = new List<UE5Actor>();
            for (int i = 0; i < count; i++)
            {
                if (actorPtrs[i] != IntPtr.Zero)
                {
                    result.Add(new UE5Actor(actorPtrs[i]));
                }
            }

            return result;
        }

        /// <summary>
        /// Get reflection system statistics
        /// </summary>
        public static (int Classes, int Properties, int Functions) GetStats()
        {
            if (!_isInitialized) throw new InvalidOperationException("Reflection system not initialized");

            GetReflectionStats(out int classes, out int properties, out int functions);
            return (classes, properties, functions);
        }
    }

    // =================================================================================
    // HIGH-LEVEL WRAPPER CLASSES
    // =================================================================================

    /// <summary>
    /// Simple 3D Vector structure
    /// </summary>
    [StructLayout(LayoutKind.Sequential)]
    public struct Vector3
    {
        public float X, Y, Z;

        public Vector3(float x, float y, float z)
        {
            X = x; Y = y; Z = z;
        }

        public static Vector3 Zero => new Vector3(0, 0, 0);
        public static Vector3 One => new Vector3(1, 1, 1);
        public static Vector3 Forward => new Vector3(1, 0, 0);
        public static Vector3 Right => new Vector3(0, 1, 0);
        public static Vector3 Up => new Vector3(0, 0, 1);

        public override string ToString() => $"({X:F2}, {Y:F2}, {Z:F2})";
    }

    /// <summary>
    /// Base wrapper for UE5 objects
    /// </summary>
    public class UE5Object : IDisposable
    {
        public IntPtr Handle { get; private set; }
        protected bool _disposed = false;

        public UE5Object(IntPtr handle)
        {
            Handle = handle;
        }

        /// <summary>
        /// Check if the object is valid
        /// </summary>
        public bool IsValid => Handle != IntPtr.Zero && UE5Reflection.IsObjectValid(Handle);

        /// <summary>
        /// Get the class name of this object
        /// </summary>
        public string GetClassName()
        {
            var sb = new StringBuilder(256);
            if (UE5Reflection.GetObjectClassName(Handle, sb, sb.Capacity))
            {
                return sb.ToString();
            }
            return "Unknown";
        }

        /// <summary>
        /// Get a property value
        /// </summary>
        public T GetProperty<T>(string propertyName) where T : struct
        {
            int size = Marshal.SizeOf<T>();
            IntPtr buffer = Marshal.AllocHGlobal(size);
            try
            {
                if (UE5Reflection.GetPropertyValue(Handle, propertyName, buffer, size))
                {
                    return Marshal.PtrToStructure<T>(buffer);
                }
                return default(T);
            }
            finally
            {
                Marshal.FreeHGlobal(buffer);
            }
        }

        /// <summary>
        /// Set a property value
        /// </summary>
        public bool SetProperty<T>(string propertyName, T value) where T : struct
        {
            int size = Marshal.SizeOf<T>();
            IntPtr buffer = Marshal.AllocHGlobal(size);
            try
            {
                Marshal.StructureToPtr(value, buffer, false);
                return UE5Reflection.SetPropertyValue(Handle, propertyName, buffer, size);
            }
            finally
            {
                Marshal.FreeHGlobal(buffer);
            }
        }

        /// <summary>
        /// Call a function on this object
        /// </summary>
        public bool CallFunction(string functionName, IntPtr parameters = default, IntPtr returnValue = default)
        {
            return UE5Reflection.CallFunction(Handle, functionName, parameters, returnValue);
        }

        /// <summary>
        /// Print all properties for debugging
        /// </summary>
        public void PrintProperties()
        {
            UE5Reflection.PrintObjectProperties(Handle);
        }

        public virtual void Dispose()
        {
            if (!_disposed && Handle != IntPtr.Zero)
            {
                UE5Reflection.DestroyObject(Handle);
                Handle = IntPtr.Zero;
                _disposed = true;
            }
            GC.SuppressFinalize(this);
        }

        ~UE5Object()
        {
            Dispose();
        }
    }

    /// <summary>
    /// Wrapper for UE5 World objects
    /// </summary>
    public class UE5World : UE5Object
    {
        public UE5World(IntPtr handle) : base(handle) { }

        /// <summary>
        /// Spawn an actor in this world
        /// </summary>
        public UE5Actor? SpawnActor(string className, Vector3 location, Vector3 rotation = default)
        {
            return UE5Reflection.SpawnActor(className, location, rotation);
        }

        /// <summary>
        /// Find actor by name in this world
        /// </summary>
        public UE5Actor? FindActor(string actorName)
        {
            return UE5Reflection.FindActorByName(actorName);
        }

        /// <summary>
        /// Get all actors of a specific class in this world
        /// </summary>
        public List<UE5Actor> GetActors(string className, int maxActors = 100)
        {
            return UE5Reflection.GetAllActorsOfClass(className, maxActors);
        }
    }

    /// <summary>
    /// Wrapper for UE5 Actor objects
    /// </summary>
    public class UE5Actor : UE5Object
    {
        public UE5Actor(IntPtr handle) : base(handle) { }

        /// <summary>
        /// Get actor location
        /// </summary>
        public Vector3 Location
        {
            get
            {
                if (UE5Reflection.GetActorLocation(Handle, out float x, out float y, out float z))
                {
                    return new Vector3(x, y, z);
                }
                return Vector3.Zero;
            }
            set
            {
                UE5Reflection.SetActorLocation(Handle, value.X, value.Y, value.Z);
            }
        }

        /// <summary>
        /// Get actor rotation
        /// </summary>
        public Vector3 Rotation
        {
            get
            {
                if (UE5Reflection.GetActorRotation(Handle, out float pitch, out float yaw, out float roll))
                {
                    return new Vector3(pitch, yaw, roll);
                }
                return Vector3.Zero;
            }
            set
            {
                UE5Reflection.SetActorRotation(Handle, value.X, value.Y, value.Z);
            }
        }

        /// <summary>
        /// Add a component to this actor
        /// </summary>
        public UE5Component? AddComponent(string componentClassName)
        {
            IntPtr componentPtr = UE5Reflection.AddComponent(Handle, componentClassName);
            if (componentPtr != IntPtr.Zero)
            {
                return new UE5Component(componentPtr);
            }
            return null;
        }

        /// <summary>
        /// Get a component from this actor
        /// </summary>
        public UE5Component? GetComponent(string componentClassName)
        {
            IntPtr componentPtr = UE5Reflection.GetComponent(Handle, componentClassName);
            if (componentPtr != IntPtr.Zero)
            {
                return new UE5Component(componentPtr);
            }
            return null;
        }

        /// <summary>
        /// Remove a component from this actor
        /// </summary>
        public bool RemoveComponent(UE5Component component)
        {
            return UE5Reflection.RemoveComponent(Handle, component.Handle);
        }
    }

    /// <summary>
    /// Wrapper for UE5 Component objects
    /// </summary>
    public class UE5Component : UE5Object
    {
        public UE5Component(IntPtr handle) : base(handle) { }
    }
}
