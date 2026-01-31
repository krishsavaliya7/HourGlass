#include "Delay.h"
void NonBlockDelay::Delay (unsigned long t)
{
  iStartTime = millis();
  iDuration = t;
  return;
};
bool NonBlockDelay::Timeout (void)
{
  return ((millis() - iStartTime) >= iDuration);
}
unsigned long NonBlockDelay::Time(void)
 {
   return iStartTime;
 }