#include "SerialProtocol.h"
#include "config.h"

// ===== Externals from main.ino =====
extern void setMode(int mode);
extern void setClockTime(int hours, int minutes);
extern void setHourglassDuration(int hours, int minutes);
extern void resetHourglass();
extern void rollDice();
extern void resetFlipCounter();
extern int getCurrentMode();
extern int getDiceValue();
extern int getFlipCount();
extern void setBrightness(int level);
extern const char* getStatusJSON();
extern const char* getOrientationJSON();
extern const char* getDisplayJSON();
// ==================================

SerialProtocol::SerialProtocol() {
    inputPos = 0;
    lastCommandTime = 0;
    memset(inputBuffer, 0, sizeof(inputBuffer));
}

void SerialProtocol::init() {
    inputPos = 0;
}

void SerialProtocol::update() {
    while (Serial.available() > 0) {
        char c = Serial.read();

        if (c == '\n' || c == '\r') {
            if (inputPos > 0) {
                inputBuffer[inputPos] = '\0';
                processCommand(inputBuffer);
                inputPos = 0;
            }
        } else if (c >= 32 && c < 127) {
            if (inputPos < sizeof(inputBuffer) - 1) {
                inputBuffer[inputPos++] = c;
            } else {
                Serial.println(F("ERR Buffer overflow"));
                inputPos = 0;
            }
        }
    }
}

void SerialProtocol::processCommand(const char* command) {
    char cmd[32];  // Reduced from 48 - matches input buffer
    strlcpy(cmd, command, sizeof(cmd));

    int len = strlen(cmd);
    while (len > 0 && cmd[len - 1] <= 32) cmd[--len] = '\0';
    while (len > 0 && cmd[0] <= 32) memmove(cmd, cmd + 1, len--);
    for (int i = 0; i < len; i++) cmd[i] = toupper(cmd[i]);

    if (len == 0) return;

    parseCommand(cmd);
    lastCommandTime = millis();
}

void SerialProtocol::parseCommand(const char* cmd) {
    const char* spacePtr = strchr(cmd, ' ');
    int cmdLen = spacePtr ? (spacePtr - cmd) : strlen(cmd);
    const char* args = spacePtr ? (spacePtr + 1) : "";

    #define CMD_MATCH(s) (cmdLen == strlen(s) && strncmp(cmd, s, cmdLen) == 0)

    // ===== STATUS & INFO COMMANDS =====
    if (CMD_MATCH("GET_STATUS")) {
        sendJSON(getStatusJSON());
    }
    else if (CMD_MATCH("GET_ORIENTATION")) {
        sendJSON(getOrientationJSON());
    }
    else if (CMD_MATCH("GET_DISPLAY")) {
        sendJSON(getDisplayJSON());
    }
    
    // ===== MODE COMMANDS =====
    else if (CMD_MATCH("SET_MODE")) {
        int mode;
        if (!strcmp(args, "CLOCK")) mode = MODE_CLOCK;
        else if (!strcmp(args, "HOURGLASS")) mode = MODE_HOURGLASS;
        else if (!strcmp(args, "DICE")) mode = MODE_DICE;
        else if (!strcmp(args, "FLIPCOUNTER") || !strcmp(args, "FLIP")) mode = MODE_FLIPCOUNTER;
        else { sendError(F("Invalid mode")); return; }

        setMode(mode);
        sendResponse(F("OK"));
    }
    
    // ===== CLOCK MODE COMMANDS =====
    else if (CMD_MATCH("SET_TIME")) {
        int hours, minutes;
        if (sscanf(args, "%d %d", &hours, &minutes) == 2) {
            if (hours >= 0 && hours <= 23 && minutes >= 0 && minutes <= 59) {
                setClockTime(hours, minutes);
                sendResponse(F("OK"));
            } else {
                sendError(F("Time out of range (HH: 0-23, MM: 0-59)"));
            }
        } else {
            sendError(F("Usage: SET_TIME HH MM"));
        }
    }
    
    // ===== HOURGLASS MODE COMMANDS =====
    else if (CMD_MATCH("SET_HG")) {
        int hours, minutes;
        if (sscanf(args, "%d %d", &hours, &minutes) == 2) {
            if (hours >= 0 && hours <= 23 && minutes >= 0 && minutes <= 59) {
                if (hours == 0 && minutes == 0) {
                    sendError(F("Duration must be greater than 0"));
                } else {
                    setHourglassDuration(hours, minutes);
                    sendResponse(F("OK"));
                }
            } else {
                sendError(F("Duration out of range (HH: 0-23, MM: 0-59)"));
            }
        } else {
            sendError(F("Usage: SET_HG HH MM"));
        }
    }
    else if (CMD_MATCH("RESET_HG")) {
        resetHourglass();
        sendResponse(F("OK"));
    }
    
    // ===== DICE MODE COMMANDS =====
    else if (CMD_MATCH("ROLL_DICE")) {
        rollDice();
        Serial.print(F("{\"diceValue\":"));
        Serial.print(getDiceValue());
        Serial.println(F("}"));
    }
    
    // ===== FLIP COUNTER MODE COMMANDS =====
    else if (CMD_MATCH("GET_FLIP_COUNT")) {
        Serial.print(F("{\"count\":"));
        Serial.print(getFlipCount());
        Serial.println(F("}"));
    }
    else if (CMD_MATCH("RESET_FLIP")) {
        resetFlipCounter();
        sendResponse(F("OK"));
    }
    
    // ===== DISPLAY COMMANDS =====
    else if (CMD_MATCH("SET_BRIGHTNESS")) {
        int level;
        if (args[0] != '\0' && sscanf(args, "%d", &level) == 1) {
            if (level >= 0 && level <= 15) {
                setBrightness(level);
                sendResponse(F("OK"));
            } else {
                sendError(F("Brightness must be 0-15"));
            }
        } else {
            sendError(F("Brightness must be 0-15"));
        }
    }
    
    // ===== UNKNOWN COMMAND =====
    else {
        sendError(F("Unknown command"));
    }

    #undef CMD_MATCH
}

// ===== OUTPUT HELPERS =====

void SerialProtocol::sendResponse(const char* response) {
    Serial.println(response);
}

void SerialProtocol::sendResponse(const __FlashStringHelper* response) {
    Serial.println(response);
}

void SerialProtocol::sendJSON(const char* json) {
    Serial.println(json);
}

void SerialProtocol::sendError(const char* message) {
    Serial.print(F("ERR "));
    Serial.println(message);
}

void SerialProtocol::sendError(const __FlashStringHelper* message) {
    Serial.print(F("ERR "));
    Serial.println(message);
}
