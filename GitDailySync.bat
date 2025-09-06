@echo off
setlocal

echo ========================================
echo Daily Git Sync Tool
echo UE5-DotNet-Modding-System
echo ========================================

:: Check if we're in a git repository
git status >nul 2>&1
if errorlevel 1 (
    echo ERROR: Not in a Git repository!
    pause
    exit /b 1
)

echo Starting daily sync process...
echo.

:: Step 1: Pull latest changes first
echo 🔄 Step 1/3: Pulling latest changes from GitHub...
git fetch origin

:: Check if we're behind remote
for /f %%i in ('git rev-list HEAD..origin/main --count') do set behind=%%i

if %behind% gtr 0 (
    echo Found %behind% new commit(s) on remote. Pulling...
    
    :: Check for local changes
    git diff-index --quiet HEAD --
    set has_changes=%errorlevel%
    
    if %has_changes% neq 0 (
        echo Stashing local changes...
        git stash push -m "Daily sync auto-stash - %date% %time%"
        set stashed=true
    )
    
    git pull origin main
    if errorlevel 1 (
        echo ERROR: Pull failed!
        if defined stashed git stash pop
        pause
        exit /b 1
    )
    
    if defined stashed (
        echo Restoring local changes...
        git stash pop
    )
) else (
    echo ✅ Already up to date with remote.
)

:: Step 2: Add and commit any local changes
echo.
echo 📝 Step 2/3: Checking for local changes to commit...

git add .

:: Check if anything was staged
git diff-index --quiet --cached HEAD --
if %errorlevel% neq 0 (
    :: Generate daily commit message
    for /f "tokens=2 delims==" %%I in ('wmic os get localdatetime /value') do set datetime=%%I
    set year=!datetime:~0,4!
    set month=!datetime:~4,2!
    set day=!datetime:~6,2!
    set hour=!datetime:~8,2!
    set minute=!datetime:~10,2!
    
    set commit_msg=Daily sync: Development progress !year!-!month!-!day! !hour!:!minute!
    
    echo Committing changes: !commit_msg!
    git commit -m "!commit_msg!"
    
    if errorlevel 1 (
        echo ERROR: Commit failed!
        pause
        exit /b 1
    )
    
    set has_commits=true
) else (
    echo ✅ No local changes to commit.
)

:: Step 3: Push to GitHub
if defined has_commits (
    echo.
    echo 🚀 Step 3/3: Pushing changes to GitHub...
    git push origin main
    
    if errorlevel 1 (
        echo ERROR: Push failed!
        pause
        exit /b 1
    )
    
    echo ✅ Changes pushed successfully!
) else (
    echo.
    echo 📤 Step 3/3: No changes to push.
)

:: Summary
echo.
echo ========================================
echo Daily Sync Complete! ✅
echo ========================================
echo Repository: https://github.com/Vampire-Chan/UE5-DotNet-Modding-System
echo.

:: Show final status
echo Final status:
git status --short

echo.
echo Recent commits:
git log --oneline -3

echo.
echo 💡 Tip: Run this script daily to keep your work synchronized!
echo.
pause
