@echo off
setlocal enabledelayedexpansion

echo ========================================
echo Git Auto Commit and Push Tool
echo UE5-DotNet-Modding-System
echo ========================================

:: Check if we're in a git repository
git status >nul 2>&1
if errorlevel 1 (
    echo ERROR: Not in a Git repository!
    pause
    exit /b 1
)

:: Show current status
echo.
echo Current Git Status:
git status --short

:: Check if there are any changes
git diff-index --quiet HEAD --
if %errorlevel% == 0 (
    git ls-files --others --exclude-standard | findstr . >nul
    if errorlevel 1 (
        echo.
        echo No changes to commit.
        pause
        exit /b 0
    )
)

:: Ask for commit message
echo.
set /p commit_msg="Enter commit message (or press Enter for auto-message): "

if "%commit_msg%"=="" (
    :: Generate automatic commit message with timestamp
    for /f "tokens=2 delims==" %%I in ('wmic os get localdatetime /value') do set datetime=%%I
    set year=!datetime:~0,4!
    set month=!datetime:~4,2!
    set day=!datetime:~6,2!
    set hour=!datetime:~8,2!
    set minute=!datetime:~10,2!
    
    set commit_msg=Auto-commit: Development progress !year!-!month!-!day! !hour!:!minute!
)

echo.
echo Commit message: !commit_msg!
echo.

:: Add all changes
echo Adding all changes...
git add .

:: Check if anything was staged
git diff-index --quiet --cached HEAD --
if %errorlevel% == 0 (
    echo No changes staged for commit.
    pause
    exit /b 0
)

:: Commit changes
echo Committing changes...
git commit -m "!commit_msg!"

if errorlevel 1 (
    echo ERROR: Commit failed!
    pause
    exit /b 1
)

:: Push to GitHub
echo.
echo Pushing to GitHub...
git push origin main

if errorlevel 1 (
    echo ERROR: Push failed!
    echo This might be due to:
    echo - Network connectivity issues
    echo - Need to pull remote changes first
    echo - Authentication problems
    echo.
    echo Try running GitPull.bat first, then try again.
    pause
    exit /b 1
)

echo.
echo ✅ SUCCESS: Changes committed and pushed to GitHub!
echo Repository: https://github.com/Vampire-Chan/UE5-DotNet-Modding-System
echo.
pause
