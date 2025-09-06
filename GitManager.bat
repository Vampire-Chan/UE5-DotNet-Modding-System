@echo off
setlocal

echo ========================================
echo Git Quick Status and Management Tool
echo UE5-DotNet-Modding-System
echo ========================================

:: Check if we're in a git repository
git status >nul 2>&1
if errorlevel 1 (
    echo ERROR: Not in a Git repository!
    pause
    exit /b 1
)

:menu
cls
echo ========================================
echo Git Quick Status and Management Tool
echo UE5-DotNet-Modding-System
echo ========================================

:: Show current branch
echo Current branch: 
git branch --show-current

echo.
echo Repository: https://github.com/Vampire-Chan/UE5-DotNet-Modding-System
echo.

:: Show status
echo Git Status:
git status --short

:: Check if behind/ahead of remote
git fetch origin >nul 2>&1
for /f %%i in ('git rev-list HEAD..origin/main --count 2^>nul') do set behind=%%i
for /f %%i in ('git rev-list origin/main..HEAD --count 2^>nul') do set ahead=%%i

if defined behind if %behind% gtr 0 (
    echo.
    echo ⬇️  Behind remote by %behind% commit(s) - consider pulling
)

if defined ahead if %ahead% gtr 0 (
    echo.
    echo ⬆️  Ahead of remote by %ahead% commit(s) - consider pushing
)

echo.
echo ========================================
echo Choose an action:
echo.
echo 1. Quick Commit and Push (with auto-message)
echo 2. Custom Commit and Push
echo 3. Pull from GitHub
echo 4. View Recent History
echo 5. View File Changes (detailed)
echo 6. Create New Branch
echo 7. Switch Branch
echo 8. View All Branches
echo 9. Exit
echo.
set /p choice="Enter your choice (1-9): "

if "%choice%"=="1" goto quick_commit
if "%choice%"=="2" goto custom_commit
if "%choice%"=="3" goto pull_changes
if "%choice%"=="4" goto view_history
if "%choice%"=="5" goto view_changes
if "%choice%"=="6" goto create_branch
if "%choice%"=="7" goto switch_branch
if "%choice%"=="8" goto view_branches
if "%choice%"=="9" goto end

echo Invalid choice. Please try again.
pause
goto menu

:quick_commit
echo.
echo Quick committing all changes...
call GitCommitPush.bat
pause
goto menu

:custom_commit
echo.
set /p commit_msg="Enter detailed commit message: "
if "%commit_msg%"=="" (
    echo Commit message cannot be empty!
    pause
    goto menu
)

git add .
git commit -m "%commit_msg%"
if errorlevel 1 (
    echo Commit failed!
    pause
    goto menu
)

git push origin main
if errorlevel 1 (
    echo Push failed!
    pause
    goto menu
)

echo ✅ SUCCESS: Changes committed and pushed!
pause
goto menu

:pull_changes
echo.
echo Pulling changes from GitHub...
call GitPull.bat
pause
goto menu

:view_history
echo.
echo Recent commit history:
echo ========================
git log --oneline --graph -10
echo.
pause
goto menu

:view_changes
echo.
echo Detailed file changes:
echo ======================
git diff --name-status
echo.
echo Unstaged changes:
git diff --stat
echo.
echo Staged changes:
git diff --cached --stat
echo.
pause
goto menu

:create_branch
echo.
set /p branch_name="Enter new branch name: "
if "%branch_name%"=="" (
    echo Branch name cannot be empty!
    pause
    goto menu
)

git checkout -b %branch_name%
if errorlevel 1 (
    echo Failed to create branch!
    pause
    goto menu
)

echo ✅ Created and switched to branch: %branch_name%
pause
goto menu

:switch_branch
echo.
echo Available branches:
git branch
echo.
set /p branch_name="Enter branch name to switch to: "
if "%branch_name%"=="" (
    echo Branch name cannot be empty!
    pause
    goto menu
)

git checkout %branch_name%
if errorlevel 1 (
    echo Failed to switch branch!
    pause
    goto menu
)

echo ✅ Switched to branch: %branch_name%
pause
goto menu

:view_branches
echo.
echo All branches:
echo =============
echo Local branches:
git branch
echo.
echo Remote branches:
git branch -r
echo.
pause
goto menu

:end
echo.
echo Goodbye!
pause
