@echo off
REM Smart Hourglass System - Force Push Deployment Script (Windows)
REM This script will force push the cleaned code to your GitHub branch
REM Branch: 10-02-26-krish
REM Repo: https://github.com/krishsavaliya7/HourGlass

echo ========================================
echo Smart Hourglass - Git Deployment Script
echo ========================================
echo.

REM Configuration
set REPO_URL=https://github.com/krishsavaliya7/HourGlass.git
set BRANCH_NAME=10-02-26-krish
set COMMIT_MESSAGE=Fixed critical issues - SPI pins, particle count, config cleanup

echo Deployment Configuration:
echo    Repository: %REPO_URL%
echo    Branch: %BRANCH_NAME%
echo    Action: Force Push (clean history)
echo.

REM Check if git is installed
where git >nul 2>nul
if %errorlevel% neq 0 (
    echo ERROR: git is not installed or not in PATH
    echo Please install Git from: https://git-scm.com/download/win
    pause
    exit /b 1
)

echo [OK] Git is installed
echo.

REM Check if we're in the right directory
if not exist "package.json" (
    echo ERROR: Not in project root directory
    echo Please run this script from: rishi-file-FINAL\
    pause
    exit /b 1
)

echo [OK] Directory check passed
echo.

REM Initialize git if needed
if not exist ".git" (
    echo Initializing new git repository...
    git init
    echo [OK] Git repository initialized
) else (
    echo [OK] Git repository already exists
)
echo.

REM Configure git user
echo Configuring git user...
git config user.name "Krish Savaliya"
git config user.email "krishsavaliya7@gmail.com"
echo [OK] Git user configured
echo.

REM Remove old remote if exists and add new one
echo Setting up remote repository...
git remote remove origin 2>nul
git remote add origin %REPO_URL%
echo [OK] Remote 'origin' configured
echo.

REM Create .gitignore if it doesn't exist
if not exist ".gitignore" (
    echo Creating .gitignore...
    (
        echo # Node modules
        echo node_modules/
        echo package-lock.json
        echo.
        echo # IDE files
        echo .vscode/
        echo .idea/
        echo *.swp
        echo *.swo
        echo *~
        echo.
        echo # OS files
        echo .DS_Store
        echo Thumbs.db
        echo desktop.ini
        echo.
        echo # Build files
        echo *.hex
        echo *.elf
        echo *.map
        echo.
        echo # Local configuration
        echo config_local.h
        echo *.local.*
        echo.
        echo # Logs
        echo *.log
        echo npm-debug.log*
        echo.
        echo # Temporary files
        echo *.tmp
        echo *.temp
        echo .cache/
    ) > .gitignore
    echo [OK] .gitignore created
) else (
    echo [OK] .gitignore already exists
)
echo.

REM Stage all files
echo Staging all files...
git add -A
echo [OK] All files staged
echo.

REM Show status
echo Files to be committed:
git status --short
echo.

REM Commit
echo Creating commit...
git commit -m "%COMMIT_MESSAGE%"
if %errorlevel% neq 0 (
    echo ERROR: Commit failed
    pause
    exit /b 1
)
echo [OK] Commit created
echo.

REM Checkout/create branch
echo Switching to branch: %BRANCH_NAME%
git checkout -b %BRANCH_NAME% 2>nul
if %errorlevel% neq 0 (
    git checkout %BRANCH_NAME%
)
echo [OK] On branch: %BRANCH_NAME%
echo.

REM Warning before force push
echo ========================================
echo WARNING: This will FORCE PUSH to GitHub
echo ========================================
echo This will replace ALL history on branch: %BRANCH_NAME%
echo Old commits will be permanently lost!
echo.
set /p confirm="Are you sure you want to continue? (yes/no): "

if /i not "%confirm%"=="yes" (
    echo Deployment cancelled
    pause
    exit /b 0
)

echo.
echo Force pushing to GitHub...
echo You may be prompted for GitHub credentials
echo.

REM Force push
git push -f origin %BRANCH_NAME%
if %errorlevel% neq 0 (
    echo ERROR: Force push failed
    echo.
    echo Troubleshooting:
    echo    1. Check your internet connection
    echo    2. Verify GitHub credentials
    echo    3. Ensure you have write access to the repository
    echo    4. You may need to use a Personal Access Token instead of password
    echo.
    echo GitHub Authentication Help:
    echo    https://docs.github.com/en/authentication
    pause
    exit /b 1
)

echo.
echo =========================================
echo SUCCESS! Code deployed to GitHub
echo =========================================
echo.
echo Branch URL:
echo    https://github.com/krishsavaliya7/HourGlass/tree/%BRANCH_NAME%
echo.
echo Deployment Summary:
echo    [OK] Repository initialized
echo    [OK] Files committed
echo    [OK] Force pushed to: %BRANCH_NAME%
echo    [OK] Clean history created
echo.
echo Next Steps:
echo    1. Visit your GitHub repository
echo    2. Create a Pull Request if needed
echo    3. Review the changes online
echo    4. Upload firmware to Arduino
echo    5. Test the web interface
echo.
echo Happy coding!
echo.
pause
