#ifndef DELAY_H
#define DELAY_H

#if defined(ARDUINO) && ARDUINO >= 100
#include <Arduino.h>
#else
#include <WProgram.h>
#endif

class NonBlockDelay {
    unsigned long iStartTime;
    unsigned long iDuration;
  public:
    void Delay (unsigned long);
    bool Timeout (void);
    unsigned long Time(void);
};

#endif  // DELAY_H
