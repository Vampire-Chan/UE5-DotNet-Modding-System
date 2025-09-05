
using UnrealBuildTool;

public class DotNetScripting : ModuleRules
{
    public DotNetScripting(ReadOnlyTargetRules Target) : base(Target)
    {
        PCHUsage = PCHUsageMode.UseExplicitOrSharedPCHs;

        PublicDependencyModuleNames.AddRange(
            new string[]
            {
                "Core",
            }
        );

        PrivateDependencyModuleNames.AddRange(
            new string[]
            {
                "CoreUObject",
                "Engine",
                "Slate",
                "SlateCore",
            }
        );

        // .NET Runtime Integration
        PublicAdditionalLibraries.Add(@"C:\Program Files\dotnet\packs\Microsoft.NETCore.App.Host.win-x64\9.0.8\runtimes\win-x64\native\nethost.lib");
        PublicSystemIncludePaths.Add(@"C:\Program Files\dotnet\packs\Microsoft.NETCore.App.Host.win-x64\9.0.8\runtimes\win-x64\native");
        
        // Enable C++20 for UE 5.6 compatibility
        CppStandard = CppStandardVersion.Cpp20;
    }
}
