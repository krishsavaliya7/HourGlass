#ifndef SERIAL_PROTOCOL_H
#define SERIAL_PROTOCOL_H

#include <Arduino.h>

class SerialProtocol {
private:
    String inputBuffer;
    unsigned long lastCommandTime;
    
public:
    SerialProtocol();
    void init();
    void update();
    void processCommand(String command);
    
private:
    void sendResponse(String response);
    void sendJSON(String json);
    void sendError(String message);
    void parseCommand(String cmd);
};

#endif

