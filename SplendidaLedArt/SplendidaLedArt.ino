//SplendidaLedArt
//St3p40(aka Stepko)
//18.01.23
#include "EMFButton.h"
#include "FastLED.h"
#include "EEPROM.h"
#include "Constants.h"

CRGB leds[NUM_REAL_LEDS];

EMFButton btn(BTN_PIN, 0);

#include "Tools.h"
#include "Effects.h"
#include "Button.h"

void setup() {
  FastLED.addLeds<NEOPIXEL, DATA_PIN>(leds, NUM_REAL_LEDS);
  if (CURRENT_LIMIT > 0) FastLED.setMaxPowerInVoltsAndMilliamps(5, CURRENT_LIMIT);
  if (EEPROM.read(0) == 252) {
    Effect.thisEff = EEPROM.read(1);
    Light.brightness = EEPROM.read(2);
  }
  Light.startupAnimation();
  Effect.RePoint();
  Effect.loadFlag = 1;
}
void loop() {
  Button.Tick();
  Effect.Tick();
}
