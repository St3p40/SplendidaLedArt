class EFF_ROUTINE {
  public:
    virtual void run(bool load);
};

class EFF_FIRE: public EFF_ROUTINE {
    void run(bool load) override {
      static uint16_t t;
      t += 64;
      for (byte x = 0; x < LED_COLS; x++) {
        for (byte y = 0; y < LED_ROWS; y++) {
          int16_t Bri = inoise8(x * 64, (y * 64) - t) - (y * (255 / LED_ROWS));
          byte Col = Bri;// inoise8(x * scale, (y * scale) - t) - (y * (255 / LED_ROWS));
          if (Bri < 0) Bri = 0; if (Bri != 0) Bri = 256 - (Bri * 0.2);
          nblend(leds[XY(x, y)], ColorFromPalette(HeatColors_p, Col, Bri), 64);
        }
      }
    }
};

class EFF_PSP: public EFF_ROUTINE {
  private:
    int xadj = (256 / LED_ROWS) << 7;
  public:
    void run(bool load) override {
      uint32_t t = millis() << 4;
      for (uint8_t x = 0; x < LED_COLS; x++) {
        uint16_t h1 = map(inoise16(x * xadj + t), 0, 65535, 0, LED_ROWS << 8);
        uint16_t h2 = map(inoise16(0, 35550, x * xadj + t), 0, 65535, 0, LED_ROWS << 8);
        uint8_t bh1 = uint8_t(h1 >> 8);
        uint8_t bh2 = uint8_t(h2 >> 8);
        for (uint8_t y = 0; y < LED_ROWS; y++) {
          leds[XY(x, y)] = CHSV(150, map(y + x, 0, LED_ROWS + LED_COLS - 1, 255, 32), map(x - (LED_ROWS - 1 - y), 0, LED_COLS - 1, 196, 255)) + CHSV(0, 0, (y < bh1) ? map(y, 0, bh1, 64, 256) : 0) + CHSV(0, 0, (y < bh2) ? map(y, 0, bh2, 64, 256) : 0);
        }
        leds[XY(x, bh1)] += CHSV(0, 0, (h1 % 256));
        leds[XY(x, bh2)] += CHSV(0, 0, (h2 % 256));
      }
    }
};

class EFF_CHECKERBOARD: public EFF_ROUTINE {
    void run(bool load) override {
      uint16_t a = millis() >> 4;
      for (byte x = 0; x < LED_COLS; x++) {
        for (byte y = 0; y < LED_ROWS; y++) {
          for (byte z = 2; z < 6; z++) {
            leds[XY(x, y)] = (((x + (sin8((a >> 2) + (z << 5)) >> 3)) / (z << 1) + (y + (sin8((a >> 1) + (z << 5)) >> 3)) / (z << 1)) & 1) ? CHSV(((z << 8) - 1) / 5, 255, ~(255 / z)) : (z == 2) ? CRGB(0, 0, 0) : leds[XY(x, y)];
          }
        }
      }
    }
};

class EFF_SPIDER: public EFF_ROUTINE {
  private:
    const float sx = ((256.0 / ((float)LED_COLS / 2.0)) - ((float)LED_COLS / 16.0));
    const float cy = ((256.0 / ((float)LED_ROWS / 2.0)) - ((float)LED_ROWS / 16.0));
  public:
    void run(bool load) override {
      fadeToBlackBy(leds, 256, 30);
      double t = millis() / 6;
      for (uint8_t c = 0; c < 7; c++) {
        float xx = 2 + sin8(t + (100 * c) * 9) / sx;
        float yy = 2 + cos8(t + (150 * c) * 9) / cy;
        drawLineF(xx, yy, LED_COLS - xx - 1, LED_ROWS - yy - 1, CHSV(c * (256 / 7), 200, 255));
      }
    }
};

class EFF_CRAZY_BEES: public EFF_ROUTINE {
  private:
    struct {
      uint8_t posX, posY, aimX, aimY, hue;
      int8_t deltaX, deltaY, error, signX, signY;
      void aimed() {
        randomSeed(millis());
        aimX = random(0, (LED_COLS * 10) - 1);
        aimY = random(0, (LED_ROWS * 10) - 1);
        hue = random8();
        deltaX = abs(aimX - posX);
        deltaY = abs(aimY - posY);
        signX = (posX < aimX) ? 1 : -1;
        signY = (posY < aimY) ? 1 : -1;
        error = deltaX - deltaY;
      };
      void run() {
        for (uint8_t a = 0; a < 4; a++) {
          if ((posX != aimX) || (posY != aimY))
          {
            int16_t error2 = error * 2;
            if (error2 > -deltaY) {
              error -= deltaY;
              posX += signX;
            }
            if (error2 < deltaX) {
              error += deltaX;
              posY += signY;
            }
          }
          else {
            aimed();
          }
        }
        drawPixelXYF(aimX / 10.0 + 1, aimY / 10.0, CHSV(hue, 255, 255));
        drawPixelXYF(aimX / 10.0, aimY / 10.0 + 1, CHSV(hue, 255, 255));
        drawPixelXYF(aimX / 10.0 - 1, aimY / 10.0, CHSV(hue, 255, 255));
        drawPixelXYF(aimX / 10.0, aimY / 10.0 - 1, CHSV(hue, 255, 255));
        drawPixelXYF(posX / 10.0, posY / 10.0, CHSV(hue, 60, 255));
      }
    } bee[2];
  public:
    void run(bool load) override {
      if (load) {
        for (byte i = 0; i < 2; i++) {
          bee[i].posX = random(0, (LED_COLS * 10) - 1);
          bee[i].posY = random(0, (LED_ROWS * 10) - 1);
          bee[i].aimed(); bee[i].aimed();
        }
      }
      fadeToBlackBy(leds, NUM_REAL_LEDS, 8);
      for (byte i = 0; i < 2; i++) {
        bee[i].run();
      }
    }
};

class EFF_INFINITY: public EFF_ROUTINE {
    void run(bool load) override {
      float x = (float)beatsin16(24, 0, (LED_COLS - 1) * 255) / 255.0;
      float y = (float)beatsin16(48, (LED_ROWS / 4) * 255, (LED_COLS - 1 - LED_ROWS / 4) * 255) / 255.0;
      drawPixelXYF(x, y, CHSV(0, 0, 255));
      drawPixelXYF(x + 1, y, CHSV(0, 0, 255));
      drawPixelXYF(x - 1, y, CHSV(0, 0, 255));
      drawPixelXYF(x, y + 1, CHSV(0, 0, 255));
      drawPixelXYF(x, y - 1, CHSV(0, 0, 255));
      fadeToBlackBy(leds, NUM_REAL_LEDS, 2);
    }
};

struct {
  bool demo: 1;
  bool loadFlag: 1;
  byte thisEff: 4;
  EFF_ROUTINE* effPtr;
  uint32_t demotimer = 0;

  void RePoint() {
    if (effPtr != nullptr) {
      delete effPtr;
    }
    switch (thisEff) {
      case 1: effPtr = new EFF_PSP; break;
      case 2: effPtr = new EFF_CHECKERBOARD; break;
      case 3: effPtr = new EFF_SPIDER; break;
      case 4: effPtr = new EFF_CRAZY_BEES; break;
      case 5: effPtr = new EFF_INFINITY; break;
      default: effPtr = new EFF_FIRE; break;
    }
  }

  void Next() {
    thisEff = ++thisEff % NUM_EFFECTS; RePoint(); loadFlag = 1;
  }
  void PrevEffect() {
    thisEff = (NUM_EFFECTS + thisEff - 1) % NUM_EFFECTS; RePoint(); loadFlag = 1;
  }
  void RandEffect() {
    thisEff = random() % NUM_EFFECTS; RePoint(); loadFlag = 1;
  }

  void ToggleDemo() {
    demo = !demo;
    demotimer = millis();
  }
  void DemoOn() {
    demo = 1;
    demotimer = millis();
  }
  void DemoOff() {
    demo = 0;
  }

  void Blink() {
    FastLED.clear();
    FastLED.show();
  }

  void Tick() {
    effPtr -> run(loadFlag);
    loadFlag = 0;
    FastLED.show();
    if (demo && (millis() >= demotimer)) {
      RandEffect();
      demotimer = millis() + DEMO_TIME * 1000;
    }
  }

} Effect;
