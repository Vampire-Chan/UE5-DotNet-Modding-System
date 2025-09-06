using System;
using System.Collections.Generic;
using GameModding;
using GameModding.Reflection;

namespace Examples
{
    /// <summary>
    /// Example demonstrating real-time UE5 Reflection API integration
    /// Shows how to interact with UE5 objects, properties, and functions from C#
    /// </summary>
    public class ReflectionExample : IGameMod
    {
        public string Name => "UE5 Reflection API Example";
        public string Version => "1.0.0";
        public string Author => "Your Name";

        private UE5World? currentWorld;
        private List<UE5Actor> spawnedActors = new List<UE5Actor>();

        public void Initialize()
        {
            Console.WriteLine("=== UE5 Reflection API Example ===");

            // Initialize the reflection system
            if (!UE5Reflection.Initialize())
            {
                Console.WriteLine("Failed to initialize UE5 Reflection system!");
                return;
            }

            Console.WriteLine("✅ UE5 Reflection system initialized successfully!");

            // Get reflection statistics
            var stats = UE5Reflection.GetStats();
            Console.WriteLine($"📊 Reflection Stats: {stats.Classes} classes, {stats.Properties} properties, {stats.Functions} functions");

            // Get the current world
            currentWorld = UE5Reflection.GetWorld();
            if (currentWorld == null)
            {
                Console.WriteLine("❌ Failed to get current world!");
                return;
            }

            Console.WriteLine($"🌍 Current world: {currentWorld.GetClassName()}");

            // Demonstrate various reflection capabilities
            DemonstrateClassReflection();
            DemonstrateObjectCreation();
            DemonstrateActorManipulation();
            DemonstratePedInteraction();
            DemonstrateComponentSystem();
        }

        /// <summary>
        /// Demonstrate class reflection capabilities
        /// </summary>
        private void DemonstrateClassReflection()
        {
            Console.WriteLine("\n=== Class Reflection Demo ===");

            // Find specific classes
            var actorClass = UE5Reflection.FindClass("Actor");
            if (actorClass.HasValue)
            {
                var ac = actorClass.Value;
                Console.WriteLine($"🎭 Found Actor class: {ac.Name}, Size: {ac.ClassSize}, Parent: {ac.ParentName}");
                Console.WriteLine($"   Properties: {ac.NumProperties}, Functions: {ac.NumFunctions}");
                Console.WriteLine($"   IsActor: {ac.IsActor}, IsComponent: {ac.IsComponent}, IsBlueprintable: {ac.IsBlueprintable}");
            }

            // Find your custom APed class
            var pedClass = UE5Reflection.FindClass("APed");
            if (pedClass.HasValue)
            {
                var pc = pedClass.Value;
                Console.WriteLine($"🚶 Found APed class: {pc.Name}, Size: {pc.ClassSize}, Parent: {pc.ParentName}");

                // Get APed properties
                var properties = UE5Reflection.GetClassProperties("APed");
                Console.WriteLine($"📋 APed Properties ({properties.Count}):");
                foreach (var prop in properties)
                {
                    Console.WriteLine($"   - {prop.Name} ({prop.TypeName}): Size={prop.Size}, Offset={prop.Offset}");
                }

                // Get APed functions
                var functions = UE5Reflection.GetClassFunctions("APed");
                Console.WriteLine($"⚙️ APed Functions ({functions.Count}):");
                foreach (var func in functions)
                {
                    Console.WriteLine($"   - {func.Name}(): Returns {func.ReturnTypeName}, Params: {func.NumParameters}");
                    Console.WriteLine($"     Static: {func.IsStatic}, BlueprintCallable: {func.IsBlueprintCallable}");
                }
            }

            // List some common UE5 classes
            Console.WriteLine("\n📚 Available Classes (first 10):");
            var allClasses = UE5Reflection.GetAllClasses(10);
            foreach (var cls in allClasses)
            {
                Console.WriteLine($"   - {cls.Name} (Parent: {cls.ParentName})");
            }
        }

        /// <summary>
        /// Demonstrate object creation and manipulation
        /// </summary>
        private void DemonstrateObjectCreation()
        {
            Console.WriteLine("\n=== Object Creation Demo ===");

            // Try to create a basic UE5 object
            var testObject = UE5Reflection.CreateObject("Object");
            if (testObject != null)
            {
                Console.WriteLine($"✅ Created object: {testObject.GetClassName()}");
                Console.WriteLine($"   Valid: {testObject.IsValid}");

                // Print its properties for debugging
                Console.WriteLine("📋 Object properties:");
                testObject.PrintProperties();

                testObject.Dispose();
            }
            else
            {
                Console.WriteLine("❌ Failed to create test object");
            }
        }

        /// <summary>
        /// Demonstrate actor spawning and manipulation
        /// </summary>
        private void DemonstrateActorManipulation()
        {
            Console.WriteLine("\n=== Actor Manipulation Demo ===");

            if (currentWorld == null) return;

            // Try to spawn a basic actor
            var spawnLocation = new Vector3(0, 0, 100); // Spawn 100 units above origin
            var spawnRotation = new Vector3(0, 0, 0);

            var newActor = currentWorld.SpawnActor("Actor", spawnLocation, spawnRotation);
            if (newActor != null)
            {
                Console.WriteLine($"🎬 Spawned actor: {newActor.GetClassName()}");
                Console.WriteLine($"   Location: {newActor.Location}");
                Console.WriteLine($"   Rotation: {newActor.Rotation}");

                spawnedActors.Add(newActor);

                // Move the actor around
                newActor.Location = new Vector3(100, 100, 100);
                Console.WriteLine($"📍 Moved actor to: {newActor.Location}");

                // Rotate the actor
                newActor.Rotation = new Vector3(0, 45, 0);
                Console.WriteLine($"🔄 Rotated actor to: {newActor.Rotation}");
            }
            else
            {
                Console.WriteLine("❌ Failed to spawn actor");
            }

            // Find existing actors
            var existingActors = currentWorld.GetActors("Actor", 5);
            Console.WriteLine($"🔍 Found {existingActors.Count} existing actors in world");
        }

        /// <summary>
        /// Demonstrate interaction with your APed system
        /// </summary>
        private void DemonstratePedInteraction()
        {
            Console.WriteLine("\n=== Ped Interaction Demo ===");

            if (currentWorld == null) return;

            // Try to spawn your custom APed
            var pedSpawnLocation = new Vector3(200, 0, 100);
            var newPed = currentWorld.SpawnActor("APed", pedSpawnLocation);

            if (newPed != null)
            {
                Console.WriteLine($"🚶 Spawned Ped: {newPed.GetClassName()}");
                spawnedActors.Add(newPed);

                // Try to access Ped-specific properties
                // (These would be based on your actual APed implementation)
                try
                {
                    // Example: Get health value (adjust property name to match your implementation)
                    var health = newPed.GetProperty<float>("Health");
                    Console.WriteLine($"❤️ Ped Health: {health}");

                    // Example: Set new health value
                    newPed.SetProperty("Health", 75.0f);
                    var newHealth = newPed.GetProperty<float>("Health");
                    Console.WriteLine($"❤️ Ped Health after change: {newHealth}");

                    // Example: Call a Ped function (adjust to match your implementation)
                    bool functionResult = newPed.CallFunction("StartWalking");
                    Console.WriteLine($"🚶‍♂️ Called StartWalking(): {functionResult}");
                }
                catch (Exception ex)
                {
                    Console.WriteLine($"⚠️ Error accessing Ped properties: {ex.Message}");
                    Console.WriteLine("   (This is expected if the exact property names don't match)");
                }

                // Print all Ped properties for debugging
                Console.WriteLine("📋 All Ped properties:");
                newPed.PrintProperties();
            }
            else
            {
                Console.WriteLine("❌ Failed to spawn APed (class might not be available in this context)");
            }

            // Look for existing Peds in the world
            var existingPeds = currentWorld.GetActors("APed");
            Console.WriteLine($"🔍 Found {existingPeds.Count} existing Peds in world");
        }

        /// <summary>
        /// Demonstrate component system interaction
        /// </summary>
        private void DemonstrateComponentSystem()
        {
            Console.WriteLine("\n=== Component System Demo ===");

            if (spawnedActors.Count == 0) return;

            var testActor = spawnedActors[0];
            Console.WriteLine($"🎭 Testing components on: {testActor.GetClassName()}");

            // Try to add a mesh component
            var meshComponent = testActor.AddComponent("StaticMeshComponent");
            if (meshComponent != null)
            {
                Console.WriteLine($"✅ Added component: {meshComponent.GetClassName()}");

                // Try to access component properties
                try
                {
                    // Component-specific operations would go here
                    meshComponent.PrintProperties();
                }
                catch (Exception ex)
                {
                    Console.WriteLine($"⚠️ Error accessing component: {ex.Message}");
                }
            }
            else
            {
                Console.WriteLine("❌ Failed to add mesh component");
            }

            // Try to get an existing component
            var existingComponent = testActor.GetComponent("RootComponent");
            if (existingComponent != null)
            {
                Console.WriteLine($"🔍 Found existing component: {existingComponent.GetClassName()}");
            }
        }

        public void Update(float deltaTime)
        {
            // Real-time reflection updates could go here
            // For example, continuously monitoring object properties
            
            static int updateCounter = 0;
            updateCounter++;

            // Every 60 updates (roughly 1 second at 60 FPS), print actor positions
            if (updateCounter % 60 == 0 && spawnedActors.Count > 0)
            {
                Console.WriteLine($"\n🔄 Real-time Update #{updateCounter / 60}:");
                foreach (var actor in spawnedActors)
                {
                    if (actor.IsValid)
                    {
                        Console.WriteLine($"   {actor.GetClassName()}: {actor.Location}");
                    }
                }
            }
        }

        public void Shutdown()
        {
            Console.WriteLine("\n=== Cleaning Up Reflection Demo ===");

            // Clean up spawned actors
            foreach (var actor in spawnedActors)
            {
                actor?.Dispose();
            }
            spawnedActors.Clear();

            // Clean up world reference
            currentWorld?.Dispose();
            currentWorld = null;

            // Shutdown reflection system
            UE5Reflection.Shutdown();

            Console.WriteLine("✅ Reflection demo cleanup complete");
        }
    }
}
