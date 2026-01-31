/**
 * LED Display Module
 * Controls visualization of 2× Max7219 8×8 LED Matrix connected via SPI to Arduino Nano R3
 * 
 * Hardware Configuration:
 * - Matrix A: Top 8×8 display (Max7219 Module 1)
 * - Matrix B: Bottom 8×8 display (Max7219 Module 2)
 * - SPI Pins: D10 (CS), D11 (CLK), D12 (DIN)
 */

class LEDMatrix {
  constructor(elementId, rows = 8, cols = 8, matrixName = 'A') {
    this.element = document.getElementById(elementId);
    if (!this.element) {
      throw new Error(`LED Matrix Error: DOM element with ID '${elementId}' not found for Matrix ${matrixName}`);
    }
    this.rows = rows;
    this.cols = cols;
    this.matrixName = matrixName;
    this.leds = [];
    this.hardwareRef = {
      driver: 'Max7219',
      address: matrixName,
      resolution: `${rows}×${cols}`,
      spiPins: { CS: 10, CLK: 11, DIN: 12 }
    };
    this.initialize();
  }

  initialize() {
    if (!this.element) return;
    this.element.innerHTML = '';
    this.leds = [];
    for (let i = 0; i < this.rows * this.cols; i++) {
      const led = document.createElement('button');
      led.className = 'led';
      led.setAttribute('aria-label', `LED ${i + 1}`);
      led.addEventListener('click', () => this.toggleLED(i));
      this.element.appendChild(led);
      this.leds.push(led);
    }
  }

  toggleLED(index) {
    if (index >= 0 && index < this.leds.length) {
      this.leds[index].classList.toggle('active');
    }
  }

  setLED(index, state) {
    if (index >= 0 && index < this.leds.length) {
      this.leds[index].classList.toggle('active', state);
    }
  }

  setPattern(pattern) {
    // Validate pattern is an array and safely handle out-of-bounds indices
    if (!Array.isArray(pattern)) {
      console.warn('setPattern: pattern must be an array');
      return;
    }
    
    this.leds.forEach((led, index) => {
      // Treat missing values as 0 (off) if pattern is shorter than LED count
      const state = index < pattern.length ? pattern[index] : 0;
      led.classList.toggle('active', state === 1);
    });
  }

  clearMatrix() {
    this.leds.forEach(led => led.classList.remove('active'));
  }

  getPattern() {
    return this.leds.map(led => led.classList.contains('active') ? 1 : 0);
  }
}

let matrixA, matrixB;

document.addEventListener('DOMContentLoaded', () => {
  // Initialize both LED matrices from hardware
  matrixA = new LEDMatrix('matrix-a', 8, 8, 'A');
  matrixB = new LEDMatrix('matrix-b', 8, 8, 'B');
  
  // Log hardware configuration
  console.log('LED Display System Initialized');
  console.log('Hardware: 2× Max7219 8×8 LED Matrix');
  console.log('Interface: SPI (Arduino Nano R3)');
  console.log('Matrix A:', matrixA.hardwareRef);
  console.log('Matrix B:', matrixB.hardwareRef);
});