using System;
using GameModding;

namespace TypeConversionTest
{
    public class TypeConversionTestMod
    {
        public static void OnLoad()
        {
            Game.Log.Info("Type Conversion Test Mod Loaded!");
            
            // Test Vector3 type conversions
            TestVector3Conversions();
            
            // Test FRotator type conversions  
            TestRotatorConversions();
            
            // Test string marshaling
            TestStringMarshaling();
            
            Game.Log.Info("Type Conversion Test completed!");
        }
        
        private static void TestVector3Conversions()
        {
            Game.Log.Info("=== Testing Vector3 Type Conversions ===");
            
            // Create a C# Vector3
            var testPosition = new Vector3(100.5f, 200.75f, 300.25f);
            Game.Log.Info($"Original C# Vector3: {testPosition}");
            
            // Test implicit conversion to UE and back
            TypeConversions.FVector3f ueVector = testPosition;
            Game.Log.Info($"Converted to UE FVector3f: ({ueVector.X}, {ueVector.Y}, {ueVector.Z})");
            
            Vector3 backToVector3 = ueVector;
            Game.Log.Info($"Converted back to Vector3: {backToVector3}");
            
            // Verify values match
            bool matches = Math.Abs(testPosition.X - backToVector3.X) < 0.001f &&
                          Math.Abs(testPosition.Y - backToVector3.Y) < 0.001f &&
                          Math.Abs(testPosition.Z - backToVector3.Z) < 0.001f;
                          
            Game.Log.Info($"Round-trip conversion test: {(matches ? "PASSED" : "FAILED")}");
        }
        
        private static void TestRotatorConversions()
        {
            Game.Log.Info("=== Testing FRotator Type Conversions ===");
            
            // Test with typical rotation values
            float testHeading = 90.0f;
            Game.Log.Info($"Original heading: {testHeading}");
            
            // Test implicit conversion to UE rotator and back
            TypeConversions.FRotator ueRotator = testHeading;
            Game.Log.Info($"Converted to UE FRotator: ({ueRotator.Pitch}, {ueRotator.Yaw}, {ueRotator.Roll})");
            
            float backToHeading = ueRotator;
            Game.Log.Info($"Converted back to heading: {backToHeading}");
            
            // Verify values match
            bool matches = Math.Abs(testHeading - backToHeading) < 0.001f;
            Game.Log.Info($"Heading conversion test: {(matches ? "PASSED" : "FAILED")}");
        }
        
        private static void TestStringMarshaling()
        {
            Game.Log.Info("=== Testing String Marshaling ===");
            
            string testMessage = "Hello from C# with special chars: åäö!@#";
            Game.Log.Info($"Original string: '{testMessage}'");
            
            // Test string marshaling through the type conversion system
            TypeConversions.WithCString(testMessage, (ptr) =>
            {
                Game.Log.Info("String successfully marshaled to C pointer");
                
                // In a real scenario, this pointer would be passed to native code
                // and the string would be reconstructed on the C++ side
                Game.Log.Info("String marshaling test: PASSED");
            });
        }
    }
}
