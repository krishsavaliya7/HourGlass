# Glassmorphism Design Quick Reference

## 🎨 Color Palette

### Primary Colors
```
Cyan:       #00d4ff  (Primary accent, buttons, highlights)
Purple:     #8b5cf6  (Secondary accent, gradients)
Navy:       #0a0e27  (Dark background)
Slate:      #b8c5d6  (Light text, secondary)
White:      #e2e8f0  (Primary text)
```

### Semantic Colors
```
Success:    #10b981  (Online status)
Danger:     #ef4444  (Offline status)
Warning:    #f59e0b  (Alerts)
```

### Glass Effects
```
Surface:      rgba(255, 255, 255, 0.08)   /* Semi-transparent background */
Surface Light: rgba(255, 255, 255, 0.12)   /* Slightly more opaque */
Border:        rgba(255, 255, 255, 0.15)   /* Glass borders */
Shadow:        0 8px 32px rgba(31, 38, 135, 0.37)  /* Depth shadow */
```

---

## 🎯 CSS Classes

### Glass Effects
```css
.glass-header      /* Header with blur */
.glass-section     /* Main section containers */
.glass-btn         /* Interactive buttons */
.glass-card        /* Card components */
.glass-info        /* Info/text boxes */
.glass-footer      /* Footer styling */
```

### Component Classes
```css
.status-badge      /* Status indicators */
.mode-badge        /* Mode display badge */
.hardware-info     /* Hardware info label */
.section-header    /* Section title styling */
.matrix-label      /* Matrix A/B labels */
```

---

## 🏗️ Component Structure

### Header
```html
<header class="glass-header">
  <div class="header-content">
    <h1>Title</h1>
    <p class="subtitle">Subtitle (Hardware info)</p>
  </div>
  <div class="status-bar">
    <!-- Status items -->
  </div>
</header>
```

### Section
```html
<section class="glass-section">
  <div class="section-header">
    <h2>Title</h2>
    <span class="hardware-info">Hardware badge</span>
  </div>
  <!-- Content -->
</section>
```

### Glass Card
```html
<div class="glass-card">
  <h3>Card Title</h3>
  <!-- Card content -->
</div>
```

### Button
```html
<button class="glass-btn">
  <span class="mode-icon">Icon</span>
  <span class="mode-name">Label</span>
</button>
```

---

## 📐 Spacing & Typography

### Font Sizes
```
h1:        2.0rem   (Header)
h2:        1.5rem   (Section titles)
h3:        1.2rem   (Subsection titles)
Body:      1.0rem   (Regular text)
Small:     0.9rem   (Secondary text)
Mini:      0.75rem  (Labels)
```

### Spacing
```
Header padding:    25px 30px
Section padding:   25px
Button padding:    20px
Card padding:      15-20px
Gap (items):       15-30px
Margin-bottom:     20-30px
```

### Border Radius
```
Header:     20px
Section:    20px
Buttons:    16px
Cards:      16px
Inputs:     10px
Badge:      12px
Circle:     50%
```

---

## ✨ Effects & Animations

### Backdrop Filter
```css
backdrop-filter: blur(10px);
-webkit-backdrop-filter: blur(10px);
```

### Transitions
```css
transition: all 0.3s ease;        /* General */
transition-colors: 150-300ms;     /* Color only */
transition: background 0.1s ease; /* Fast feedback */
```

### Hover States
```
Button hover:
  - Background opacity increases
  - Border color changes to primary
  - Transform: translateY(-2px)
  - Shadow increases

Input focus:
  - Border color: primary
  - Background opacity increases
  - Box-shadow with primary glow
```

### Gradient Overlays
```
Active button:
  linear-gradient(135deg, rgba(0, 212, 255, 0.25) 0%, 
                          rgba(139, 92, 246, 0.15) 100%)

Status glow:
  radial-gradient(ellipse at 20% 50%, rgba(0, 212, 255, 0.1) 0%, 
                                      transparent 50%)
```

---

## 🔌 Hardware Integration Points

### Header Info
```html
<h1>⏳ Smart Hourglass Control Panel</h1>
<p class="subtitle">Arduino Nano R3 | Dual 8×8 LED Matrix</p>
```

### Display Section
```html
<span class="hardware-info">2× Max7219 8×8 LED Matrix</span>
```

### Settings Section
```html
<div class="glass-info">
  <p><strong>🔌 Connection:</strong> Arduino Nano R3 via USB Serial</p>
  <p><strong>📊 Display:</strong> 2× Max7219 8×8 LED Matrix</p>
  <p><strong>📡 Protocol:</strong> Serial @9600 baud</p>
</div>
```

---

## 🎬 Animation Timing

```
Smooth transitions:  200-300ms
Fast feedback:       100-150ms
Slow effects:        500-1000ms
Default ease:        cubic-bezier(0.4, 0, 0.2, 1)
```

---

## 📱 Responsive Breakpoints

```css
Mobile:    max-width: 375px
Tablet:    max-width: 768px
Laptop:    max-width: 1024px
Desktop:   1440px+
```

---

## 🧪 Testing Checklist

### Visual
- [ ] Blur effects visible on all glass elements
- [ ] Colors match cyan/purple/navy palette
- [ ] Border visibility good on dark background
- [ ] Shadows provide proper depth
- [ ] Text contrast sufficient (WCAG AA)

### Interaction
- [ ] Buttons have hover effects
- [ ] Focus states visible on keyboard nav
- [ ] Transitions are smooth (no jank)
- [ ] No layout shift on hover/focus

### Hardware Display
- [ ] Both LED matrices visible
- [ ] Matrix labels clear
- [ ] Hardware info badges readable
- [ ] Status badge shows correct state

### Responsive
- [ ] No horizontal scroll on mobile
- [ ] Touch targets minimum 44×44px
- [ ] Text readable at all sizes
- [ ] Layouts adapt properly

---

## 🚀 Performance Tips

1. **Use CSS variables** - Change theme easily
2. **Hardware acceleration** - Backdrop filters use GPU
3. **Optimize images** - Keep file sizes small
4. **Lazy load** - Load JS only when needed
5. **Minimize repaints** - Use `will-change: filter` sparingly

---

## 🔧 Customization Guide

### Change Primary Color
```css
:root {
  --primary-color: #your-color;  /* Change cyan */
}
```

### Adjust Blur Strength
```css
:root {
  --glass-blur: blur(15px);  /* Increase/decrease blur */
}
```

### Modify Border Opacity
```css
:root {
  --border-color: rgba(255, 255, 255, 0.20);  /* More/less visible */
}
```

### Change Background Gradient
```css
body {
  background: linear-gradient(135deg, 
    #your-color1 0%, 
    #your-color2 50%, 
    #your-color3 100%);
}
```

---

## 📚 Resources

- **Glassmorphism**: https://glassmorphism.com/
- **Color Palette**: https://coolors.co/
- **CSS Backdrop Filter**: https://developer.mozilla.org/en-US/docs/Web/CSS/backdrop-filter
- **Web Serial API**: https://developer.mozilla.org/en-US/docs/Web/API/Web_Serial_API

---

**Quick Start**: Copy any component class and modify as needed!
