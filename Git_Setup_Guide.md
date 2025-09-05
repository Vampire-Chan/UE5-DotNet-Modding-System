# 🔄 Git Repository Setup and Management Guide

## 🚀 Initial Repository Setup

Run these commands to set up your Git repository:

```bash
# Initialize Git repository
git init

# Configure Git (replace with your details)
git config user.name "Your Name"
git config user.email "your.email@example.com"

# Add all files respecting .gitignore
git add .

# Create initial commit
git commit -m "🎉 Initial commit: UE5 Game with .NET Modding System

- Complete Unreal Engine 5.6 project structure
- DotNetScripting plugin with C++/C# bridge
- ModdingTemplate with GameModding library and examples
- Comprehensive Ped system with entity architecture
- Documentation and build guides
- Production-ready modding system"

# Add remote repository (replace with your GitHub repo URL)
git remote add origin https://github.com/yourusername/your-repo-name.git

# Push to GitHub
git branch -M main
git push -u origin main
```

## 📋 Daily Workflow Commands

### Basic Operations
```bash
# Check status
git status

# Stage changes
git add .                    # Add all changes
git add Source/              # Add specific directory
git add "*.cpp" "*.h"        # Add specific file types

# Commit changes
git commit -m "Feature: Add vehicle spawning system"
git commit -m "Fix: Resolve Ped animation controller issue"
git commit -m "Docs: Update bridge API extension guide"

# Push to GitHub
git push origin main
```

### Working with Branches
```bash
# Create and switch to feature branch
git checkout -b feature/vehicle-system
git checkout -b fix/ped-health-bug
git checkout -b docs/api-improvements

# Work on your changes, then commit
git add .
git commit -m "Implement vehicle inheritance from entity"

# Push feature branch
git push origin feature/vehicle-system

# Switch back to main
git checkout main

# Merge feature branch (after GitHub PR)
git pull origin main
git branch -d feature/vehicle-system  # Delete local branch
```

## 📊 Repository Structure Validation

Use these commands to verify your setup:

```bash
# Check what's being tracked
git ls-files | head -20

# Check what's being ignored
git status --ignored

# Verify file sizes (keep under GitHub's 100MB limit)
git ls-files | xargs ls -lh | sort -k5 -hr | head -10

# Check repository size
du -sh .git
```

## 🔧 Useful Git Commands for This Project

### Modding System Development
```bash
# Stage only modding-related changes
git add ModdingTemplate/ Plugins/DotNetScripting/

# Commit modding improvements
git commit -m "Enhance: Add vehicle export system to bridge"

# Track documentation changes
git add "*.md"
git commit -m "Docs: Update API extension examples"
```

### Game Development
```bash
# Stage game source changes
git add Source/ Content/ Data/

# Commit game features
git commit -m "Feature: Add advanced Ped AI system"

# Track config changes
git add Config/
git commit -m "Config: Update engine settings for performance"
```

### Build Output Management
```bash
# Check what DLLs are being tracked
git ls-files | grep -E "\.(dll|pdb)$"

# Add new mod DLLs
git add Binaries/Win64/DotNet/
git commit -m "Mods: Add new example mod builds"
```

## 🛡️ Best Practices for This Project

### Commit Message Conventions
```
Feature: Add new functionality
Fix: Bug fixes
Enhance: Improvements to existing features
Docs: Documentation updates
Config: Configuration changes
Refactor: Code restructuring
Test: Testing related changes
Build: Build system changes
```

### What to Commit Frequently
✅ **Source code changes** (C++ and C#)  
✅ **Documentation updates** (README, guides)  
✅ **Configuration changes** (UE settings)  
✅ **Asset updates** (maps, meshes if not too large)  
✅ **Data files** (XML configurations)  
✅ **Final mod DLLs** (working examples)  

### What NOT to Commit
❌ **Build intermediates** (automatically ignored)  
❌ **Personal settings** (user-specific configs)  
❌ **Temporary files** (logs, cache)  
❌ **Large binary assets** (>100MB - use Git LFS if needed)  

## 🔄 Backup and Sync Strategy

### Daily Routine
1. **Morning**: `git pull origin main` (get latest changes)
2. **During work**: Commit frequently with descriptive messages
3. **Evening**: `git push origin main` (backup your work)

### Before Major Changes
```bash
# Create backup branch
git checkout -b backup-before-major-refactor
git push origin backup-before-major-refactor

# Work on main branch
git checkout main
# ... make changes ...
```

### Weekly Maintenance
```bash
# Clean up merged branches
git branch --merged | grep -v "main" | xargs -n 1 git branch -d

# Update remote tracking
git remote prune origin

# Check repository health
git fsck
```

## 🚨 Emergency Recovery

### Undo Last Commit (if not pushed)
```bash
git reset --soft HEAD~1    # Keep changes staged
git reset --hard HEAD~1    # Discard changes completely
```

### Revert Pushed Commit
```bash
git revert HEAD            # Creates new commit that undoes last commit
git push origin main
```

### Recover Lost Work
```bash
git reflog                 # Shows command history
git checkout <commit-hash> # Recover specific state
```

## 📈 Repository Monitoring

### Check File Types Being Tracked
```bash
git ls-files | grep -E "\.(cpp|h|cs|md|uproject|uplugin)$" | wc -l
```

### Monitor Repository Size
```bash
git count-objects -vH      # Shows repository statistics
```

### Track Large Files
```bash
git ls-files | xargs ls -l | sort -k5 -nr | head -10
```

## 🔗 GitHub Integration

### Setting Up GitHub Repository
1. Create new repository on GitHub
2. **Don't** initialize with README (we have our own)
3. Copy the remote URL
4. Use in the setup commands above

### Recommended GitHub Settings
- ✅ **Enable Issues** (for bug tracking)
- ✅ **Enable Wiki** (for extended documentation)  
- ✅ **Protect main branch** (require PR reviews)
- ✅ **Enable Discussions** (for community help)

### GitHub Actions (Optional)
Consider setting up automated builds for:
- C++ compilation validation
- .NET mod building
- Documentation generation

---

## 🎯 Quick Setup Checklist

1. ✅ Run `git init` in project root
2. ✅ Configure Git user settings  
3. ✅ Verify `.gitignore` is working correctly
4. ✅ Create initial commit with all important files
5. ✅ Create GitHub repository
6. ✅ Add remote and push
7. ✅ Set up branch protection on GitHub
8. ✅ Start daily commit routine

**Your project is now under professional version control!** 🎉
