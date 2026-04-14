//
// Created by wiktor on 3/24/26.
//
#include "sensorsim.h"
#include "Arduino.h"
#include "math.h"

float sinsim(float avr, float ampl, float period) {
  float arg = 2 * M_PI * millis() * 0.001 / period;
  return avr + ampl * sin(arg);
}