#!/bin/bash

# Smart Hourglass System - Force Push Deployment Script
# This script will force push the cleaned code to your GitHub branch
# Branch: 10-02-26-krish
# Repo: https://github.com/krishsavaliya7/HourGlass

echo "🚀 Smart Hourglass - Git Deployment Script"
echo "==========================================="
echo ""

# Colors for output
RED='\033[0;31m'
GREEN='\033[0;32m'
YELLOW='\033[1;33m'
NC='\033[0m' # No Color

# Configuration
REPO_URL="https://github.com/krishsavaliya7/HourGlass.git"
BRANCH_NAME="10-02-26-krish"
COMMIT_MESSAGE="✅ Fixed critical issues - SPI pins, particle count, config cleanup"

echo -e "${YELLOW}📋 Deployment Configuration:${NC}"
echo "   Repository: $REPO_URL"
echo "   Branch: $BRANCH_NAME"
echo "   Action: Force Push (clean history)"
echo ""

# Check if we're in the right directory
if [ ! -f "package.json" ]; then
    echo -e "${RED}❌ Error: Not in project root directory${NC}"
    echo "   Please run this script from: rishi-file-FINAL/"
    exit 1
fi

echo -e "${GREEN}✅ Directory check passed${NC}"
echo ""

# Check if git is installed
if ! command -v git &> /dev/null; then
    echo -e "${RED}❌ Error: git is not installed${NC}"
    exit 1
fi

echo -e "${GREEN}✅ Git is installed${NC}"
echo ""

# Initialize git if needed
if [ ! -d ".git" ]; then
    echo -e "${YELLOW}📦 Initializing new git repository...${NC}"
    git init
    echo -e "${GREEN}✅ Git repository initialized${NC}"
else
    echo -e "${YELLOW}📦 Git repository already exists${NC}"
fi
echo ""

# Configure git user (you can modify these)
echo -e "${YELLOW}🔧 Configuring git user...${NC}"
git config user.name "Krish Savaliya"
git config user.email "krishsavaliya7@gmail.com"
echo -e "${GREEN}✅ Git user configured${NC}"
echo ""

# Remove old remote if exists and add new one
echo -e "${YELLOW}🔗 Setting up remote repository...${NC}"
git remote remove origin 2>/dev/null || true
git remote add origin "$REPO_URL"
echo -e "${GREEN}✅ Remote 'origin' configured${NC}"
echo ""

# Create .gitignore if it doesn't exist
if [ ! -f ".gitignore" ]; then
    echo -e "${YELLOW}📝 Creating .gitignore...${NC}"
    cat > .gitignore << 'EOF'
# Node modules
node_modules/
package-lock.json

# IDE files
.vscode/
.idea/
*.swp
*.swo
*~

# OS files
.DS_Store
Thumbs.db
desktop.ini

# Build files
*.hex
*.elf
*.map

# Local configuration
config_local.h
*.local.*

# Logs
*.log
npm-debug.log*

# Temporary files
*.tmp
*.temp
.cache/
EOF
    echo -e "${GREEN}✅ .gitignore created${NC}"
else
    echo -e "${GREEN}✅ .gitignore already exists${NC}"
fi
echo ""

# Stage all files
echo -e "${YELLOW}📦 Staging all files...${NC}"
git add -A
echo -e "${GREEN}✅ All files staged${NC}"
echo ""

# Show what will be committed
echo -e "${YELLOW}📋 Files to be committed:${NC}"
git status --short
echo ""

# Commit
echo -e "${YELLOW}💾 Creating commit...${NC}"
git commit -m "$COMMIT_MESSAGE" || {
    echo -e "${RED}❌ Commit failed${NC}"
    exit 1
}
echo -e "${GREEN}✅ Commit created${NC}"
echo ""

# Checkout/create branch
echo -e "${YELLOW}🌿 Switching to branch: $BRANCH_NAME${NC}"
git checkout -b "$BRANCH_NAME" 2>/dev/null || git checkout "$BRANCH_NAME"
echo -e "${GREEN}✅ On branch: $BRANCH_NAME${NC}"
echo ""

# Warning before force push
echo -e "${RED}⚠️  WARNING: This will FORCE PUSH to GitHub${NC}"
echo -e "${RED}   This will replace ALL history on branch: $BRANCH_NAME${NC}"
echo -e "${RED}   Old commits will be permanently lost!${NC}"
echo ""
read -p "Are you sure you want to continue? (yes/no): " confirm

if [ "$confirm" != "yes" ]; then
    echo -e "${YELLOW}❌ Deployment cancelled${NC}"
    exit 0
fi

echo ""
echo -e "${YELLOW}🚀 Force pushing to GitHub...${NC}"
echo -e "${YELLOW}   You may be prompted for GitHub credentials${NC}"
echo ""

# Force push
git push -f origin "$BRANCH_NAME" || {
    echo -e "${RED}❌ Force push failed${NC}"
    echo ""
    echo -e "${YELLOW}💡 Troubleshooting:${NC}"
    echo "   1. Check your internet connection"
    echo "   2. Verify GitHub credentials"
    echo "   3. Ensure you have write access to the repository"
    echo "   4. You may need to use a Personal Access Token instead of password"
    echo ""
    echo -e "${YELLOW}📖 GitHub Authentication Help:${NC}"
    echo "   https://docs.github.com/en/authentication"
    exit 1
}

echo ""
echo -e "${GREEN}═════════════════════════════════════════${NC}"
echo -e "${GREEN}🎉 SUCCESS! Code deployed to GitHub${NC}"
echo -e "${GREEN}═════════════════════════════════════════${NC}"
echo ""
echo -e "${GREEN}📍 Branch URL:${NC}"
echo "   https://github.com/krishsavaliya7/HourGlass/tree/$BRANCH_NAME"
echo ""
echo -e "${GREEN}✅ Deployment Summary:${NC}"
echo "   ✓ Repository initialized"
echo "   ✓ Files committed"
echo "   ✓ Force pushed to: $BRANCH_NAME"
echo "   ✓ Clean history created"
echo ""
echo -e "${YELLOW}📝 Next Steps:${NC}"
echo "   1. Visit your GitHub repository"
echo "   2. Create a Pull Request if needed"
echo "   3. Review the changes online"
echo "   4. Upload firmware to Arduino"
echo "   5. Test the web interface"
echo ""
echo -e "${GREEN}🚀 Happy coding!${NC}"
echo ""
