@echo off
setlocal

echo ========================================
echo Git Pull Tool
echo UE5-DotNet-Modding-System
echo ========================================

:: Check if we're in a git repository
git status >nul 2>&1
if errorlevel 1 (
    echo ERROR: Not in a Git repository!
    pause
    exit /b 1
)

:: Show current branch and status
echo.
echo Current branch:
git branch --show-current

echo.
echo Checking for remote changes...

:: Fetch latest changes from remote
git fetch origin

:: Check if we're behind remote
for /f %%i in ('git rev-list HEAD..origin/main --count') do set behind=%%i

if %behind% == 0 (
    echo.
    echo ✅ Already up to date with remote repository.
    echo No changes to pull.
    pause
    exit /b 0
)

echo.
echo Found %behind% new commit(s) on remote repository.
echo.

:: Check if we have local uncommitted changes
git diff-index --quiet HEAD --
set has_changes=%errorlevel%

git ls-files --others --exclude-standard | findstr . >nul 2>&1
if not errorlevel 1 set has_changes=1

if %has_changes% neq 0 (
    echo WARNING: You have local uncommitted changes.
    echo.
    echo Local changes:
    git status --short
    echo.
    echo Options:
    echo 1. Stash changes and pull (recommended)
    echo 2. Commit changes first, then pull
    echo 3. Cancel pull operation
    echo.
    set /p choice="Choose option (1/2/3): "
    
    if "!choice!"=="1" (
        echo Stashing local changes...
        git stash push -m "Auto-stash before pull - %date% %time%"
        if errorlevel 1 (
            echo ERROR: Failed to stash changes!
            pause
            exit /b 1
        )
        set stashed=true
    ) else if "!choice!"=="2" (
        echo Please commit your changes first using GitCommitPush.bat
        pause
        exit /b 0
    ) else (
        echo Pull operation cancelled.
        pause
        exit /b 0
    )
)

:: Pull changes from remote
echo.
echo Pulling changes from remote repository...
git pull origin main

if errorlevel 1 (
    echo ERROR: Pull failed!
    echo This might be due to:
    echo - Merge conflicts
    echo - Network connectivity issues
    echo - Authentication problems
    echo.
    if defined stashed (
        echo Restoring stashed changes...
        git stash pop
    )
    pause
    exit /b 1
)

:: Restore stashed changes if any
if defined stashed (
    echo.
    echo Restoring your stashed changes...
    git stash pop
    if errorlevel 1 (
        echo WARNING: Failed to restore stashed changes automatically.
        echo Your changes are still in the stash. Use 'git stash list' to see them.
        echo Use 'git stash pop' to restore them manually.
    ) else (
        echo ✅ Stashed changes restored successfully.
    )
)

echo.
echo ✅ SUCCESS: Repository updated from GitHub!
echo.
echo Recent commits:
git log --oneline -5

echo.
pause
