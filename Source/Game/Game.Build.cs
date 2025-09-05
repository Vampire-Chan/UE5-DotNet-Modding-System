// Copyright Epic Games, Inc. All Rights Reserved.
using System.IO;
using UnrealBuildTool;

public class Game : ModuleRules
{
	public Game(ReadOnlyTargetRules Target) : base(Target)
	{
		PCHUsage = PCHUsageMode.UseExplicitOrSharedPCHs;

		PublicDependencyModuleNames.AddRange(new string[] { "Core", "CoreUObject", "Engine", "InputCore", "HeadMountedDisplay", "EnhancedInput", "AIModule", "UMG", "PhysicsCore", "AssetRegistry", "PakFile", "AnimGraphRuntime" });

		PrivateDependencyModuleNames.AddRange(new string[] { "EnhancedInput", "Slate", "SlateCore", "TinyXML2" });

				PublicIncludePaths.AddRange(new string[] {
            ModuleDirectory ,
			System.IO.Path.Combine(ModuleDirectory, "Animation"),
			System.IO.Path.Combine(ModuleDirectory, "Core"),
			System.IO.Path.Combine(ModuleDirectory, "Core", "Enums"),
			System.IO.Path.Combine(ModuleDirectory, "ThirdPartyLibs", "TinyXML2")
			
			// Add the module's root Source directory
        });

		// Uncomment if you are using Slate UI
		// PrivateDependencyModuleNames.AddRange(new string[] { "Slate", "SlateCore" });
		
		// Uncomment if you are using online features
		// PrivateDependencyModuleNames.Add("OnlineSubsystem");

		// To include OnlineSubsystemSteam, add it to the plugins section in your uproject file with the Enabled attribute set to true
	}
}