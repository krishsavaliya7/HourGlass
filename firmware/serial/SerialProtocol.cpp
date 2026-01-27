#include "SerialProtocol.h"
#include "../config.h"

// Forward declarations - these will be set by main.ino
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
extern String getStatusJSON();
extern String getOrientationJSON();
extern String getDisplayJSON();

SerialProtocol::SerialProtocol() {
    inputBuffer = "";
    lastCommandTime = 0;
}

void SerialProtocol::init() {
    // Serial already initialized in main setup
    inputBuffer.reserve(64);
}

void SerialProtocol::update() {
    // Read incoming serial data
    while (Serial.available() > 0) {
        char c = Serial.read();
        
        if (c == '\n' || c == '\r') {
            if (inputBuffer.length() > 0) {
                processCommand(inputBuffer);
                inputBuffer = "";
            }
        } else if (c >= 32 && c < 127) { // Printable ASCII
            inputBuffer += c;
            if (inputBuffer.length() >= 64) {
                // Buffer overflow protection
                inputBuffer = "";
            }
        }
    }
}

void SerialProtocol::processCommand(String command) {
    command.trim();
    command.toUpperCase();
    
    if (command.length() == 0) return;
    
    parseCommand(command);
    lastCommandTime = millis();
}

void SerialProtocol::parseCommand(String cmd) {
    // Parse command format: COMMAND [arg1] [arg2] ...
    int spaceIndex = cmd.indexOf(' ');
    String command = spaceIndex > 0 ? cmd.substring(0, spaceIndex) : cmd;
    String args = spaceIndex > 0 ? cmd.substring(spaceIndex + 1) : "";
    
    if (command == "GET_STATUS") {
        sendJSON(getStatusJSON());
    }
    else if (command == "SET_MODE") {
        args.trim();
        int mode = MODE_CLOCK;
        if (args == "CLOCK") mode = MODE_CLOCK;
        else if (args == "HOURGLASS") mode = MODE_HOURGLASS;
        else if (args == "DICE") mode = MODE_DICE;
        else if (args == "FLIPCOUNTER" || args == "FLIP") mode = MODE_FLIPCOUNTER;
        else {
            sendError("Invalid mode");
            return;
        }
        setMode(mode);
        sendResponse("OK");
    }
    else if (command == "SET_TIME") {
        int spaceIdx = args.indexOf(' ');
        if (spaceIdx > 0) {
            int hours = args.substring(0, spaceIdx).toInt();
            int minutes = args.substring(spaceIdx + 1).toInt();
            if (hours >= 0 && hours <= 23 && minutes >= 0 && minutes <= 59) {
                setClockTime(hours, minutes);
                sendResponse("OK");
            } else {
                sendError("Invalid time range");
            }
        } else {
            sendError("Missing time parameters");
        }
    }
    else if (command == "SET_HG") {
        int spaceIdx = args.indexOf(' ');
        if (spaceIdx > 0) {
            int hours = args.substring(0, spaceIdx).toInt();
            int minutes = args.substring(spaceIdx + 1).toInt();
            if (hours >= 0 && hours <= 23 && minutes >= 1 && minutes <= 59) {
                setHourglassDuration(hours, minutes);
                sendResponse("OK");
            } else {
                sendError("Invalid duration");
            }
        } else {
            sendError("Missing duration parameters");
        }
    }
    else if (command == "RESET_HG") {
        resetHourglass();
        sendResponse("OK");
    }
    else if (command == "ROLL_DICE") {
        rollDice();
        sendJSON("{\"diceValue\":" + String(getDiceValue()) + "}");
    }
    else if (command == "GET_FLIP_COUNT") {
        sendJSON("{\"count\":" + String(getFlipCount()) + "}");
    }
    else if (command == "RESET_FLIP") {
        resetFlipCounter();
        sendResponse("OK");
    }
    else if (command == "GET_ORIENTATION") {
        sendJSON(getOrientationJSON());
    }
    else if (command == "GET_DISPLAY") {
        sendJSON(getDisplayJSON());
    }
    else if (command == "SET_BRIGHTNESS") {
        int level = args.toInt();
        if (level >= 0 && level <= 15) {
            setBrightness(level);
            sendResponse("OK");
        } else {
            sendError("Brightness must be 0-15");
        }
    }
    else {
        sendError("Unknown command: " + command);
    }
}

void SerialProtocol::sendResponse(String response) {
    Serial.println(response);
}

void SerialProtocol::sendJSON(String json) {
    Serial.println(json);
}

void SerialProtocol::sendError(String message) {
    Serial.print("ERR ");
    Serial.println(message);
}

