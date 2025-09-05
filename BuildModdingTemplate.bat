@echo off
echo Building .NET Modding System from ModdingTemplate...
echo.

cd /d "D:\UnrealProjs\Game"

echo [1/4] Building Core GameModding Library...
dotnet build "ModdingTemplate\GameModding\GameModding.csproj" --configuration Debug
if %ERRORLEVEL% neq 0 (
    echo ERROR: GameModding build failed!
    pause
    exit /b 1
)

echo.
echo [2/4] Building BasicMod Example...
dotnet build "ModdingTemplate\Examples\BasicMod\BasicMod.csproj" --configuration Debug
if %ERRORLEVEL% neq 0 (
    echo ERROR: BasicMod build failed!
    pause
    exit /b 1
)

echo.
echo [3/4] Building PlayerNiko Example...
dotnet build "ModdingTemplate\Examples\PlayerNikoExample\PlayerNikoExample.csproj" --configuration Debug
if %ERRORLEVEL% neq 0 (
    echo ERROR: PlayerNikoExample build failed!
    pause
    exit /b 1
)

echo.
echo [4/4] Building Type Conversion Test...
dotnet build "ModdingTemplate\Examples\TypeConversionTest\TypeConversionTest.csproj" --configuration Debug
if %ERRORLEVEL% neq 0 (
    echo ERROR: TypeConversionTest build failed!
    pause
    exit /b 1
)

echo.
echo ============================================
echo .NET Modding System Build Complete!
echo ============================================
echo.
echo Project Structure:
echo   Game Plugin:     Plugins\DotNetScripting\ (C++ bridge/wrapper)
echo   Modding Template: ModdingTemplate\ (C# API and examples)
echo.
echo Output Locations:
echo   Core Library: Binaries\Win64\DotNet\net8.0\GameModding.dll
echo   Example Mods: ModdingTemplate\Examples\*\bin\Debug\*.dll
echo.
echo Note: The C++ plugin (DotNetScripting.dll) will be built automatically
echo       when you compile the game project in Unreal Engine.
echo.
echo The ModdingTemplate folder contains everything modders need!
echo.
pause
