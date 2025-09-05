using System;
using System.Runtime.InteropServices;
using System.Text;

namespace GameModding
{
    /// <summary>
    /// Type conversion utilities for C# to Unreal Engine interop
    /// Handles proper marshaling between .NET types and UE C++ types
    /// </summary>
    public static class TypeConversions
    {
        // ═══════════════════════════════════════════════════════════════
        // STRING CONVERSIONS
        // ═══════════════════════════════════════════════════════════════

        /// <summary>
        /// Convert C# string to C-style null-terminated string pointer
        /// MUST be freed with FreeHGlobal after use!
        /// </summary>
        public static IntPtr StringToPtr(string str)
        {
            if (string.IsNullOrEmpty(str))
                return IntPtr.Zero;
            
            // Convert to UTF-8 bytes and allocate unmanaged memory
            byte[] utf8Bytes = Encoding.UTF8.GetBytes(str + '\0'); // Add null terminator
            IntPtr ptr = Marshal.AllocHGlobal(utf8Bytes.Length);
            Marshal.Copy(utf8Bytes, 0, ptr, utf8Bytes.Length);
            return ptr;
        }

        /// <summary>
        /// Convert C-style string pointer to C# string
        /// </summary>
        public static string? PtrToString(IntPtr ptr)
        {
            if (ptr == IntPtr.Zero)
                return null;
            
            return Marshal.PtrToStringUTF8(ptr);
        }

        /// <summary>
        /// Safe string conversion with automatic memory management
        /// Use this for temporary string operations
        /// </summary>
        public static void WithCString(string str, Action<IntPtr> action)
        {
            IntPtr ptr = StringToPtr(str);
            try
            {
                action(ptr);
            }
            finally
            {
                if (ptr != IntPtr.Zero)
                    Marshal.FreeHGlobal(ptr);
            }
        }

        // ═══════════════════════════════════════════════════════════════
        // VECTOR CONVERSIONS (C# Vector3 ↔ UE FVector)
        // ═══════════════════════════════════════════════════════════════

        /// <summary>
        /// UE FVector structure for interop (matches UE layout)
        /// </summary>
        [StructLayout(LayoutKind.Sequential)]
        public struct FVector
        {
            public double X;
            public double Y; 
            public double Z;

            public FVector(double x, double y, double z)
            {
                X = x;
                Y = y;
                Z = z;
            }

            // Convert from C# Vector3
            public static implicit operator FVector(Vector3 v)
            {
                return new FVector(v.X, v.Y, v.Z);
            }

            // Convert to C# Vector3
            public static implicit operator Vector3(FVector fv)
            {
                return new Vector3((float)fv.X, (float)fv.Y, (float)fv.Z);
            }
        }

        /// <summary>
        /// UE FVector3f structure for interop (32-bit float version)
        /// </summary>
        [StructLayout(LayoutKind.Sequential)]
        public struct FVector3f
        {
            public float X;
            public float Y;
            public float Z;

            public FVector3f(float x, float y, float z)
            {
                X = x;
                Y = y;
                Z = z;
            }

            // Convert from C# Vector3
            public static implicit operator FVector3f(Vector3 v)
            {
                return new FVector3f(v.X, v.Y, v.Z);
            }

            // Convert to C# Vector3
            public static implicit operator Vector3(FVector3f fv)
            {
                return new Vector3(fv.X, fv.Y, fv.Z);
            }
        }

        // ═══════════════════════════════════════════════════════════════
        // ROTATION CONVERSIONS (C# float ↔ UE FRotator)
        // ═══════════════════════════════════════════════════════════════

        /// <summary>
        /// UE FRotator structure for interop
        /// </summary>
        [StructLayout(LayoutKind.Sequential)]
        public struct FRotator
        {
            public double Pitch; // Rotation around Y axis
            public double Yaw;   // Rotation around Z axis  
            public double Roll;  // Rotation around X axis

            public FRotator(double pitch, double yaw, double roll)
            {
                Pitch = pitch;
                Yaw = yaw;
                Roll = roll;
            }

            // Convert from simple heading (yaw only)
            public static implicit operator FRotator(float heading)
            {
                return new FRotator(0, heading, 0);
            }

            // Convert to simple heading (yaw only)
            public static implicit operator float(FRotator rot)
            {
                return (float)rot.Yaw;
            }
        }

        // ═══════════════════════════════════════════════════════════════
        // COLOR CONVERSIONS (C# Color ↔ UE FLinearColor)
        // ═══════════════════════════════════════════════════════════════

        /// <summary>
        /// UE FLinearColor structure for interop
        /// </summary>
        [StructLayout(LayoutKind.Sequential)]
        public struct FLinearColor
        {
            public float R, G, B, A;

            public FLinearColor(float r, float g, float b, float a = 1.0f)
            {
                R = r;
                G = g; 
                B = b;
                A = a;
            }

            // Predefined colors
            public static FLinearColor White => new FLinearColor(1, 1, 1, 1);
            public static FLinearColor Black => new FLinearColor(0, 0, 0, 1);
            public static FLinearColor Red => new FLinearColor(1, 0, 0, 1);
            public static FLinearColor Green => new FLinearColor(0, 1, 0, 1);
            public static FLinearColor Blue => new FLinearColor(0, 0, 1, 1);
        }

        // ═══════════════════════════════════════════════════════════════
        // HANDLE CONVERSIONS (C# objects ↔ UE pointers)
        // ═══════════════════════════════════════════════════════════════

        /// <summary>
        /// Convert UE object pointer to safe C# handle
        /// Returns IntPtr.Zero if null
        /// </summary>
        public static IntPtr ObjectToHandle(object? obj)
        {
            if (obj == null) return IntPtr.Zero;
            
            // In a real implementation, you might use a handle table
            // For now, we'll use the object's hash code as a simple handle
            return new IntPtr(obj.GetHashCode());
        }

        /// <summary>
        /// Validate that a handle points to a valid UE object
        /// </summary>
        public static bool IsValidHandle(IntPtr handle)
        {
            return handle != IntPtr.Zero;
        }

        // ═══════════════════════════════════════════════════════════════
        // ENUM CONVERSIONS
        // ═══════════════════════════════════════════════════════════════

        /// <summary>
        /// Convert C# enum to UE enum (int)
        /// </summary>
        public static int EnumToInt<T>(T enumValue) where T : Enum
        {
            return Convert.ToInt32(enumValue);
        }

        /// <summary>
        /// Convert UE enum (int) to C# enum
        /// </summary>
        public static T IntToEnum<T>(int value) where T : Enum
        {
            return (T)Enum.ToObject(typeof(T), value);
        }

        // ═══════════════════════════════════════════════════════════════
        // ARRAY CONVERSIONS
        // ═══════════════════════════════════════════════════════════════

        /// <summary>
        /// Convert C# array to unmanaged array pointer
        /// MUST be freed with FreeHGlobal after use!
        /// </summary>
        public static IntPtr ArrayToPtr<T>(T[] array) where T : struct
        {
            if (array == null || array.Length == 0)
                return IntPtr.Zero;

            int size = Marshal.SizeOf<T>() * array.Length;
            IntPtr ptr = Marshal.AllocHGlobal(size);
            
            for (int i = 0; i < array.Length; i++)
            {
                IntPtr elementPtr = IntPtr.Add(ptr, i * Marshal.SizeOf<T>());
                Marshal.StructureToPtr(array[i], elementPtr, false);
            }
            
            return ptr;
        }

        /// <summary>
        /// Convert unmanaged array pointer to C# array
        /// </summary>
        public static T[] PtrToArray<T>(IntPtr ptr, int count) where T : struct
        {
            if (ptr == IntPtr.Zero || count <= 0)
                return new T[0];

            T[] array = new T[count];
            
            for (int i = 0; i < count; i++)
            {
                IntPtr elementPtr = IntPtr.Add(ptr, i * Marshal.SizeOf<T>());
                array[i] = Marshal.PtrToStructure<T>(elementPtr);
            }
            
            return array;
        }

        // ═══════════════════════════════════════════════════════════════
        // UTILITY FUNCTIONS
        // ═══════════════════════════════════════════════════════════════

        /// <summary>
        /// Safe pointer operation with automatic cleanup
        /// </summary>
        public static void WithPointer<T>(T value, Action<IntPtr> action) where T : struct
        {
            IntPtr ptr = Marshal.AllocHGlobal(Marshal.SizeOf<T>());
            try
            {
                Marshal.StructureToPtr(value, ptr, false);
                action(ptr);
            }
            finally
            {
                Marshal.FreeHGlobal(ptr);
            }
        }

        /// <summary>
        /// Convert degrees to radians (UE often uses radians)
        /// </summary>
        public static float DegreesToRadians(float degrees)
        {
            return degrees * (float)(Math.PI / 180.0);
        }

        /// <summary>
        /// Convert radians to degrees
        /// </summary>
        public static float RadiansToDegrees(float radians)
        {
            return radians * (float)(180.0 / Math.PI);
        }

        /// <summary>
        /// Convert UE units (cm) to meters
        /// </summary>
        public static float UnrealUnitsToMeters(float unrealUnits)
        {
            return unrealUnits / 100.0f;
        }

        /// <summary>
        /// Convert meters to UE units (cm)
        /// </summary>
        public static float MetersToUnrealUnits(float meters)
        {
            return meters * 100.0f;
        }
    }
}
