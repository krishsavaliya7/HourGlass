#ifndef SERIAL_PROTOCOL_H
#define SERIAL_PROTOCOL_H

#include <Arduino.h>

class SerialProtocol {
private:
    char inputBuffer[32];  // Reduced from 48 - longest command is ~25 chars
    uint8_t inputPos;
    unsigned long lastCommandTime;

public:
    SerialProtocol();
    void init();
    void update();
    void processCommand(const char* command);

    // --- OUTPUT (SRAM + FLASH safe) ---
    void sendResponse(const char* response);
    void sendResponse(const __FlashStringHelper* response);

    void sendJSON(const char* json);

    void sendError(const char* message);
    void sendError(const __FlashStringHelper* message);

private:
    void parseCommand(const char* cmd);
};

#endif