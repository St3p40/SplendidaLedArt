struct {
  void OneClick() {
    Effect.Next();
  }
  void TwoClicks() {
    Effect.ToggleDemo(); Effect.Blink();
  }
  void ThreeClicks() {
    if (EEPROM.read(0) != 252) EEPROM.write(0, 252);
    if (EEPROM.read(1) != Effect.thisEff) EEPROM.write(1, Effect.thisEff);
    if (EEPROM.read(2) != Light.brightness) EEPROM.write(2, Light.brightness);
    Effect.Blink();
  }
  bool BriDir: 1;
  void Tick() {
    btn.tick();
    if (btn.isPressed()) digitalWrite(13, 1); else digitalWrite(13, 0);
    if (btn.hasClicks() == 1) {
      OneClick();
    }
    if (btn.hasClicks() == 2) {
      TwoClicks();
    }
    if (btn.hasClicks() == 3) {
      ThreeClicks();
    }
    if (btn.isHeld())
    {
      BriDir = !BriDir;
    }
    if (btn.isHold()) {
      Light.brightness = constrain(Light.brightness + (Light.brightness / 25 + 1) * (BriDir * 2 - 1), 1 , BRIGHTNESS);
      FastLED.setBrightness(Light.brightness);
    }
  }
} Button;
