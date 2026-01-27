#include "Delay.h"

void NonBlockDelay::Delay (unsigned long ms)
{
  iStartTime = millis();
  iDuration = ms;
  return;
};

bool NonBlockDelay::Timeout (void)
{
  return ((millis() - iStartTime) >= iDuration);
}

unsigned long NonBlockDelay::Time(void)
 {
   return (millis() - iStartTime);
