# Serial Command Protocol Documentation

This document describes the **USB serial protocol** used by the Smart Hourglass & Clock System. All communication happens over the board's serial port (e.g. `COMx` on Windows, `/dev/ttyUSBx` on Linux, or via OTG on Android).

**Serial Settings:**
- Baud Rate: **9600**
- Data Bits: 8
- Stop Bits: 1
- Parity: None
- Flow Control: None

Commands are ASCII text lines terminated by a newline (`\n`). The device replies with either **JSON** or **key=value** lines describing the current state.

---

## 1. General Format

- **Host → Device**: `COMMAND [ARG1 [ARG2 ...]]\n`
- **Device → Host**: either
  - Single JSON line, e.g.  
    `{"mode":"clock","time":"14:30","orientation":"horizontal","hourglassProgress":42,"diceValue":3,"flipCount":5}`
  - Or multiple `KEY=VALUE` lines, e.g.  
    ```text
    MODE=clock
    TIME=14:30
    ORIENTATION=horizontal
    HG_PROGRESS=42
    DICE=3
    FLIP=5
    ```

The default and recommended format is **single-line JSON** for easier parsing in JavaScript.

---

## 2. Commands

### 2.1 `SET_MODE`

Change operating mode.

**Syntax:**
```text
SET_MODE <mode>
```

**Modes:**
- `clock`
- `hourglass`
- `dice`
- `flip`

**Example:**
```text
SET_MODE dice
```

**Typical Response (JSON):**
```json
{"status":"ok","mode":"dice"}
```

---

### 2.2 `SET_TIME`

Set clock time (Clock mode).

**Syntax:**
```text
SET_TIME <hours> <minutes>
```

**Arguments:**
- `hours`: `0–23`
- `minutes`: `0–59`

**Example:**
```text
SET_TIME 14 30
```

**Typical Response:**
```json
{"status":"ok","mode":"clock","time":"14:30"}
```

---

### 2.3 `SET_HG`

Set hourglass duration (Hourglass mode).

**Syntax:**
```text
SET_HG <hours> <minutes>
```

**Arguments:**
- `hours`: `0–23`
- `minutes`: `1–59` (0 minutes is not allowed)

**Example:**
```text
SET_HG 0 5
```

**Typical Response:**
```json
{"status":"ok","mode":"hourglass","hours":0,"minutes":5}
```

**Note:** The hourglass uses realistic particle physics animation matching the reference implementation. Particle drops are timed based on duration, and buzzer provides audio feedback.

---

### 2.4 `ROLL_DICE`

Roll the dice (Dice mode).

**Syntax:**
```text
ROLL_DICE
```

**Example:**
```text
ROLL_DICE
```

**Typical Response:**
```json
{"status":"ok","mode":"dice","diceValue":4}
```

---

### 2.5 `GET_STATUS`

Get current device status and mode information.

**Syntax:**
```text
GET_STATUS
```

**Example:**
```text
GET_STATUS
```

**Typical JSON Response:**
```json
{
  "mode": "clock|hourglass|dice|flip",
  "orientation": "horizontal|vertical",
  "time": "HH:MM",              // Clock mode
  "hourglassProgress": 0-100,   // Hourglass mode
  "diceValue": 1-6,             // Dice mode
  "flipCount": 0,               // Flip counter mode
  "batteryLevel": 0-100         // Optional / simulated
}
```

---

### 2.6 `RESET_HG`

Reset hourglass timer (Hourglass mode).

**Syntax:**
```text
RESET_HG
```

**Example:**
```text
RESET_HG
```

**Typical Response:**
```json
{"status":"ok"}
```

**Note:** Resets the hourglass animation and refills the top matrix with particles.

---

### 2.7 `GET_FLIP_COUNT`

Get flip counter value (Flip Counter mode).

**Syntax:**
```text
GET_FLIP_COUNT
```

**Typical Response:**
```json
{"count":5}
```

---

### 2.8 `RESET_FLIP`

Reset flip counter to zero (Flip Counter mode).

**Syntax:**
```text
RESET_FLIP
```

**Typical Response:**
```json
{"status":"ok"}
```

---

### 2.9 `GET_ORIENTATION`

Get current device orientation data.

**Syntax:**
```text
GET_ORIENTATION
```

**Typical Response:**
```json
{"angle":90,"x":0.5,"y":-0.3,"z":0.8}
```

---

### 2.10 `GET_DISPLAY`

Get current LED matrix display state.

**Syntax:**
```text
GET_DISPLAY
```

**Typical Response:**
```json
{"matrixA":[[8x8 boolean array]],"matrixB":[[8x8 boolean array]]}
```

---

### 2.11 `SET_BRIGHTNESS`

Set display brightness.

**Syntax:**
```text
SET_BRIGHTNESS <level>
```

**Arguments:**
- `level`: `0–15` (0 = dimmest, 15 = brightest)

**Example:**
```text
SET_BRIGHTNESS 10
```

**Typical Response:**
```json
{"status":"ok","brightness":10}
```

---

## 3. Error Responses

When a command is invalid or cannot be processed, the device replies with an error object:

```json
{
  "status": "error",
  "message": "Invalid command",
  "code": 400
}
```

**Common Error Codes:**
- `400` – Invalid syntax or parameter (e.g. out-of-range value)
- `404` – Unknown command
- `500` – Internal error

---

## 4. Host-Side Usage (Web Serial)

The web UI uses the **Web Serial API** to open the device’s serial port and send/receive these commands.

### 4.1 Opening the Port

```javascript
const port = await navigator.serial.requestPort();
await port.open({ baudRate: 9600 });
```

### 4.2 Writing a Command

```javascript
const textEncoder = new TextEncoderStream();
const writableStreamClosed = textEncoder.readable.pipeTo(port.writable);
const writer = textEncoder.writable.getWriter();

await writer.write("GET_STATUS\n");
```

### 4.3 Reading Responses

```javascript
const reader = textDecoder.readable.getReader();

while (true) {
  const { value, done } = await reader.read();
  if (done) break;
  if (value) {
    const line = value.trim();
    // Parse JSON or key=value here
  }
}
```

See `web-ui/js/serial.js` (or equivalent) for the concrete implementation used in this project.

