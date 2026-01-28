# LED Display Hardware Configuration

## Physical Connection Diagram

```
┌─────────────────────────────────────────────────────────────┐
│                      ARDUINO NANO R3                         │
│                   (ATmega328P @16MHz)                        │
│                                                              │
│  ┌──────────────────────────────────────────────────────┐  │
│  │ DIGITAL PINS (GPIO)                                  │  │
│  ├──────────────────────────────────────────────────────┤  │
│  │ D0  (PD0) - RXD (Serial Input)                       │  │
│  │ D1  (PD1) - TXD (Serial Output)                      │  │
│  │ D2  (PD2) - Button Input                            │  │
│  │ ...                                                  │  │
│  │ D10 (PB2) - CS  ──────────────────┐                 │  │
│  │ D11 (PB3) - CLK ────────────────┐ │                 │  │
│  │ D12 (PB4) - DIN ──────────────┐ │ │                 │  │
│  │ ...                           │ │ │                 │  │
│  └───────────────────────────────┼─┼─┼─────────────────┘  │
│                                  │ │ │                     │
│  ┌──────────────────────────────┼─┼─┼─────────────────┐  │
│  │ ANALOG PINS (I2C)            │ │ │                 │  │
│  ├──────────────────────────────┼─┼─┼─────────────────┤  │
│  │ A4  (PC4) - SDA              │ │ │                 │  │
│  │ A5  (PC5) - SCL              │ │ │ (MPU6050)       │  │
│  └──────────────────────────────┼─┼─┼─────────────────┘  │
│                                  │ │ │                     │
│  ┌──────────────────────────────┼─┼─┼─────────────────┐  │
│  │ POWER                         │ │ │                 │  │
│  ├──────────────────────────────┼─┼─┼─────────────────┤  │
│  │ 5V (VCC) ────────────────────┼─┼─┼──────────────┐  │  │
│  │ GND (Ground) ────────────────┼─┼─┼──────────────┼──┐ │
│  └──────────────────────────────┼─┼─┼──────────────┼──┼─┘ │
│                                  │ │ │              │  │   │
└──────────────────────────────────┼─┼─┼──────────────┼──┼───┘
                                   │ │ │              │  │
                    ┌──────────────┼─┼─┼──────────────┼──┼────────┐
                    │              │ │ │              │  │        │
                  ┌─┴──────────────┼─┼─┼──────────────┼──┴─────┐  │
                  │ VCC            │ │ │              GND      │  │
    ┌─────────────┤                │ │ │                      │  │
    │             │                │ │ │                      │  │
    │    ┌────────┴─────┐  ┌───────┴─┴─┴─────────┬─────────┐ │  │
    │    │ Connector A  │  │   SPI CABLE        │         │ │  │
    │    ├──────────────┤  ├────────────────────┤         │ │  │
    │    │ Pin 1: VCC   │  │ D12 (DIN) ─────────┤ Pin 1  │ │  │
    │    │ Pin 2: GND   │  │ D11 (CLK) ─────────┤ Pin 2  │ │  │
    │    │ Pin 3: DIN   │  │ D10 (CS)  ─────────┤ Pin 3  │ │  │
    │    │ Pin 4: CLK   │  │                    │ Pin 4  │ │  │
    │    │ Pin 5: CS    │  │ GND ──────────────┤ Pin 5  │ │  │
    │    └──────┬────────┘  │ VCC ──────────────┤ Pin 6  │ │  │
    │           │           │                    │         │ │  │
    │    ┌──────┴───────────┴────────────────────┼──────┐  │ │  │
    │    │                                        │      │  │ │  │
    │    │   ┌──────────────────┐                │      │  │ │  │
    │    └──>│ MAX7219 Module   │<───────────────┘      │  │ │  │
    │        │ (Matrix A)       │                       │  │ │  │
    │        │                  │                       │  │ │  │
    │        │ 8×8 LED Matrix   │                       │  │ │  │
    │        └────────┬─────────┘                       │  │ │  │
    │                 │                                │  │ │  │
    │    ┌────────────┴───────────┐                    │  │ │  │
    │    │                        │                    │  │ │  │
    │    │   ┌──────────────────┐ │                    │  │ │  │
    │    └──>│ MAX7219 Module   │ │                    │  │ │  │
    │        │ (Matrix B)       │ │                    │  │ │  │
    │        │                  │ │                    │  │ │  │
    │        │ 8×8 LED Matrix   │ │                    │  │ │  │
    │        └──────────────────┘ │                    │  │ │  │
    │           (Daisy-chained)   │                    │  │ │  │
    │                             │                    │  │ │  │
    └─────────────────────────────┘                    │  │ │  │
                                                       │  │ │  │
                              ┌────────────────────────┘  │ │  │
                              │                           │ │  │
                         ┌────┴──────────────────────┐   │ │  │
                         │   MPU6050 Sensor          │───┘ │  │
                         │   (I2C Address: 0x68)     │     │  │
                         │   ┌─────────────────────┐ │     │  │
                         │   │ Accelerometer/      │ │     │  │
                         │   │ Gyroscope 6-axis    │ │     │  │
                         │   └─────────────────────┘ │     │  │
                         │   VCC ────────────────────────┐ │  │
                         │   GND ────────────────────────┼─┘  │
                         │   SDA (A4) ────────────┐      │    │
                         │   SCL (A5) ────────────┤      │    │
                         └────────────────────────┼──────┼───┘
                                                  │      │
                                     GND ─────────┘      │
                                     VCC ────────────────┘
```

## SPI Communication Bus

```
┌─────────────────────────────────────────────────────┐
│                    SPI BUS                          │
├─────────────────────────────────────────────────────┤
│ Clock  (CLK)  ──> Synchronization signal            │
│                   Rate: 16 MHz (Arduino clock)      │
│                   Divided by 2^N in firmware        │
│                                                     │
│ Data Input (DIN) ──> Serial data shift-in           │
│                     Latched on rising CLK edge      │
│                                                     │
│ Chip Select (CS) ──> Frame synchronization          │
│                     Active LOW (0 = selected)       │
│                     Rising edge latches data        │
│                                                     │
│ Data Stream Format:                                 │
| Byte 1: Address (0x00=No-Op, 0x01-0x08=Digit,   |
|         0x09=Decode, 0x0A=Intensity,              |
|         0x0B=Scan, 0x0C=Shutdown, 0x0F=Test)      |
│ Byte 2: Data (8 bits for LED row)                  │
│                                                     │
│ Update Rate: ~60 Hz (per mode)                     │
│ Typical Update: 16 bytes per refresh               │
│   (8 rows × 2 matrices)                            │
└─────────────────────────────────────────────────────┘
```

## I2C Communication (MPU6050)

```
┌─────────────────────────────────────────────┐
│         I2C Bus (400 kHz)                   │
├─────────────────────────────────────────────┤
│ SCL (Clock)  ── A5 (PC5)                    │
│ SDA (Data)   ── A4 (PC4)                    │
│                                             │
│ Pull-up Resistors: ~4.7kΩ (typical)        │
│                                             │
│ MPU6050 Address: 0x68 (0x69 if AD0 high)  │
│                                             │
│ Data Read Rate: 100 Hz (orientation check) │
│                                             │
│ Registers Used:                            │
│ 0x3B-0x40: Accelerometer (X, Y, Z)        │
│ 0x43-0x48: Gyroscope (X, Y, Z)            │
│ 0x41-0x42: Temperature                    │
└─────────────────────────────────────────────┘
```

## Serial UART Connection

```
┌─────────────────────────────────────────────┐
│     USB Serial (UART @9600 baud)            │
├─────────────────────────────────────────────┤
│ TX (D1, PD1) ──> To Computer USB            │
│ RX (D0, PD0) ──< From Computer USB          │
│                                             │
│ Interface IC: CH340G (USB to Serial)        │
│ Baud Rate: 9600 bits/second                 │
│ Data Bits: 8                                │
│ Stop Bits: 1                                │
│ Parity: None                                │
│                                             │
│ Message Format:                             │
│ <COMMAND>:<PARAM1>:<PARAM2>:<PARAM3>\r\n  │
│                                             │
│ Example Messages:                           │
│ MODE:0\r\n         (Set Clock mode)        │
│ TIME:12:30\r\n     (Set time to 12:30)    │
│ BRIGHT:10\r\n      (Set brightness to 10) │
│ STATUS\r\n         (Request status)        │
└─────────────────────────────────────────────┘
```

## Power Distribution

```
┌──────────────────────────────┐
│  Power Supply: 5V (USB)      │
├──────────────────────────────┤
│  Total Current: ~800mA max   │
│                              │
│  Distribution:               │
│  ├─ Arduino Nano R3: 50mA    │
│  ├─ MPU6050: 15mA            │
│  ├─ LED Matrix A: 300mA      │
│  └─ LED Matrix B: 300mA      │
│                              │
│  Decoupling Capacitors:      │
│  ├─ C1: 100µF (bulk)         │
│  ├─ C2: 10µF (Arduino)       │
│  ├─ C3: 100nF (I2C)          │
│  ├─ C4: 100nF (SPI)          │
│  └─ C5: 10µF (LED)           │
└──────────────────────────────┘
```

⚠️ **USB Power Limitation**: Standard USB 2.0 provides only **500mA** maximum current, but LED Matrix A and B combined require up to **600mA** at full brightness. When powered via USB:
- Reduce LED brightness (use `SET_BRIGHTNESS` command with values 0-8 instead of 15)
- OR connect an external 5V power supply capable of supplying at least 800mA
- Without proper power, LEDs may flicker, display incorrectly, or the Arduino may reset

For full brightness operation, use an external 5V ≥800mA power supply.

## Pin Summary Table

| Pin | Function | Type | Hardware Component |
|-----|----------|------|-------------------|
| D0  | RXD (Serial RX) | Input | USB Serial |
| D1  | TXD (Serial TX) | Output | USB Serial |
| D2  | Button Input | Input | Push Button |
| D10 | CS (SPI) | Output | Max7219 |
| D11 | CLK (SPI) | Output | Max7219 |
| D12 | DIN (SPI) | Output | Max7219 |
| A4  | SDA (I2C) | Bidirectional | MPU6050 |
| A5  | SCL (I2C) | Bidirectional | MPU6050 |
| 5V  | Power | Supply | All modules |
| GND | Ground | Reference | All modules |

## Firmware Configuration (config.h)

```cpp
// Pin Definitions (from firmware/main/config.h - this is the source of truth)
#define PIN_DATAIN 5     // Data Input (DIN) - Pin 5/D1
#define PIN_CLK 4        // Clock (CLK) - Pin 4/D2
#define PIN_LOAD 10      // Load/Chip Select (CS) - Pin 10/D3
#define PIN_BUTTON 2     // Button input - Pin 2/D4
#define NUM_MATRICES 2   // Two 8×8 LED matrices

// I2C Configuration
#define MPU6050_ADDRESS 0x68

// Serial Configuration
#define BAUD_RATE 9600

// LED Configuration
#define LED_OFF 0x00
#define LED_ON 0xFF
#define BRIGHTNESS_MIN 0
#define BRIGHTNESS_MAX 15
```

---

**Last Updated**: 2026-01-27
**Hardware Version**: Arduino Nano R3 with Max7219 LED Drivers
**Web UI Version**: 1.0 with Glassmorphism Design
