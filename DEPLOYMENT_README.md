# 🚀 Git Deployment Guide

## Quick Start

### For Linux/Mac Users:
```bash
cd rishi-file-FINAL
./deploy-to-github.sh
```

### For Windows Users:
```cmd
cd rishi-file-FINAL
deploy-to-github.bat
```

---

## What This Does

This deployment script will:

1. ✅ Initialize a fresh Git repository
2. ✅ Configure Git with your credentials
3. ✅ Add remote: https://github.com/krishsavaliya7/HourGlass.git
4. ✅ Stage ALL project files
5. ✅ Create a commit with fixes applied
6. ✅ Switch to branch: `10-02-26-krish`
7. ✅ **FORCE PUSH** to GitHub (replaces all history)

---

## ⚠️ Important Notes

### This is a FORCE PUSH
- **All previous commits** on the `10-02-26-krish` branch will be **replaced**
- The branch will have a **clean, fresh history**
- Old commits will be **permanently lost**
- This is exactly what you requested for a clean result ✅

### GitHub Authentication

You'll need to authenticate when pushing. You have two options:

#### Option 1: Personal Access Token (Recommended)
1. Go to: https://github.com/settings/tokens
2. Click "Generate new token (classic)"
3. Give it a name: "HourGlass Deploy"
4. Select scopes: `repo` (full control)
5. Click "Generate token"
6. **Copy the token** (you won't see it again!)
7. When prompted for password, **paste the token**

#### Option 2: SSH Key
1. Set up SSH key: https://docs.github.com/en/authentication/connecting-to-github-with-ssh
2. Change the script to use SSH URL:
   ```bash
   # Edit deploy-to-github.sh or deploy-to-github.bat
   # Change:
   REPO_URL="https://github.com/krishsavaliya7/HourGlass.git"
   # To:
   REPO_URL="git@github.com:krishsavaliya7/HourGlass.git"
   ```

---

## What Was Fixed

Before deploying, the following critical issues were automatically fixed:

### ✅ Fix #1: SPI Pin Configuration
- **File:** `web-ui/js/led-display.js`
- **Change:** Updated documentation from CLK=11 to CLK=13 (matches Arduino)

### ✅ Fix #2: Particle Count Standardization
- **Files:** `Firmware/rishi-file/rishi-file.ino`, `web-ui/index.html`
- **Change:** Unified to 48 particles (from 60) for stability

### ✅ Fix #3: Removed Duplicate Config
- **File:** `Firmware/rishi-file/rishi-file.ino`
- **Change:** Removed duplicate `DELAY_FRAME` definition

### ✅ Fix #4: Added Error Handling
- **File:** `web-ui/js/led-display.js`
- **Change:** Added try-catch for LED matrix initialization

See `CHANGELOG.md` for full details.

---

## Manual Git Commands (Alternative)

If you prefer to run Git commands manually:

```bash
# Navigate to project
cd rishi-file-FINAL

# Initialize repository
git init

# Configure user
git config user.name "Krish Savaliya"
git config user.email "krishsavaliya7@gmail.com"

# Add remote
git remote add origin https://github.com/krishsavaliya7/HourGlass.git

# Stage all files
git add -A

# Commit
git commit -m "✅ Fixed critical issues - SPI pins, particle count, config cleanup"

# Create/switch to branch
git checkout -b 10-02-26-krish

# Force push
git push -f origin 10-02-26-krish
```

---

## Verify Deployment

After deployment:

1. **Visit GitHub:**
   ```
   https://github.com/krishsavaliya7/HourGlass/tree/10-02-26-krish
   ```

2. **Check the commit:**
   - Should see: "✅ Fixed critical issues - SPI pins, particle count, config cleanup"
   - Should have clean history (only this commit)

3. **Review files:**
   - `CHANGELOG.md` - New file with all fixes documented
   - `web-ui/js/led-display.js` - Updated pin configuration
   - `Firmware/rishi-file/rishi-file.ino` - Fixed particle count
   - `web-ui/index.html` - Updated to 48 particles

---

## Troubleshooting

### "git: command not found"
- **Linux/Mac:** Install Git: `sudo apt-get install git` or `brew install git`
- **Windows:** Download from https://git-scm.com/download/win

### "Permission denied (publickey)"
- Use Personal Access Token instead (see Authentication section above)

### "Force push failed"
- Check internet connection
- Verify you have write access to the repository
- Try using SSH instead of HTTPS

### "Updates were rejected"
- This shouldn't happen with force push (`-f` flag)
- If it does, ensure you're on the correct branch
- Run: `git branch` to verify

---

## File Structure After Deployment

```
HourGlass (Repository)
└── 10-02-26-krish (Branch) ← Your clean code here
    ├── Firmware/
    │   └── rishi-file/
    │       ├── rishi-file.ino (✅ Fixed)
    │       ├── config.h
    │       ├── LedControl.h/cpp
    │       └── ...
    ├── web-ui/
    │   ├── index.html (✅ Fixed)
    │   ├── css/styles.css
    │   └── js/
    │       ├── led-display.js (✅ Fixed)
    │       ├── api.js
    │       ├── serial.js
    │       └── ...
    ├── CHANGELOG.md (✅ New)
    ├── README.md
    └── package.json
```

---

## After Deployment

1. **Test locally first:**
   ```bash
   cd web-ui
   npm run serve-node
   # or
   python -m http.server 8080
   ```

2. **Upload Arduino firmware:**
   - Open `Firmware/rishi-file/rishi-file.ino` in Arduino IDE
   - Select board: Arduino Nano
   - Upload to your device

3. **Test web interface:**
   - Open Chrome/Edge
   - Visit: http://localhost:8080
   - Click "Connect Device"
   - Test all modes

---

## Need Help?

- **GitHub Documentation:** https://docs.github.com
- **Git Basics:** https://git-scm.com/doc
- **Web Serial API:** https://developer.mozilla.org/en-US/docs/Web/API/Web_Serial_API

---

**Ready to deploy?** Run the script and follow the prompts! 🚀
