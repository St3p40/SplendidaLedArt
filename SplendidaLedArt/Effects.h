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
    uint16_t xadj = (256 / LED_ROWS) << 7;
    uint8_t hue;
  public:
    void run(bool load) override {
      if (load) {
        hue = random();
      }
      uint32_t t = millis() << 4;
      for (uint8_t x = 0; x < LED_COLS; x++) {
        uint16_t h1 = map(inoise16(x * xadj + t), 0, 65535, 0, LED_ROWS << 8);
        uint16_t h2 = map(inoise16(0, 35550, x * xadj + t), 0, 65535, 0, LED_ROWS << 8);
        uint8_t bh1 = uint8_t(h1 >> 8);
        uint8_t bh2 = uint8_t(h2 >> 8);
        for (uint8_t y = 0; y < LED_ROWS; y++) {
          leds[XY(x, y)] = CHSV(hue, map(y + x, 0, LED_ROWS + LED_COLS - 1, 255, 32), map(x - (LED_ROWS - 1 - y), 0, LED_COLS - 1, 196, 255)) + CHSV(0, 0, (y < bh1) ? map(y, 0, bh1, 64, 256) : 0) + CHSV(0, 0, (y < bh2) ? map(y, 0, bh2, 64, 256) : 0);
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

class EFF_CAUSTIC: public EFF_ROUTINE {
  private:
    uint8_t noise[2][LED_COLS + 1][LED_ROWS + 1];
    void wu_pixel(int16_t x, int16_t y) {
      // extract the fractional parts and derive their inverses
      uint8_t xx = x & 0xff, yy = y & 0xff, ix = 255 - xx, iy = 255 - yy;
      // calculate the intensities for each affected pixel
#define WU_WEIGHT(a, b)((uint8_t)(((a) * (b) + (a) + (b)) >> 8))
      uint8_t wu[4] = {
        WU_WEIGHT(ix, iy),
        WU_WEIGHT(xx, iy),
        WU_WEIGHT(ix, yy),
        WU_WEIGHT(xx, yy)
      };
#undef WU_WEIGHT
      // multiply the intensities by the colour, and saturating-add them to the pixels
      for (uint8_t i = 0; i < 4; i++) {
        uint8_t local_x = (x >> 8) + (i & 1);
        uint8_t local_y = (y >> 8) + ((i >> 1) & 1);
        if ((local_x >= 0 && local_x <= LED_COLS) && (local_y >= 0 && local_y <= LED_ROWS))
          noise[1][local_x][local_y] = constrain(qsub8(noise[1][local_x][local_y], wu[i] >> 1), 15, 255);
        }
    }
    uint16_t x, y, z;
    void fillNoise() {
      uint8_t dataSmoothing = 136;
      for (uint8_t i = 0; i < LED_COLS + 1; i++) {
        int32_t ioffset =  i << 5;
        for (uint8_t j = 0; j < LED_ROWS + 1; j++) {
          int32_t joffset = j << 5;

          uint8_t data = inoise8(x + ioffset, y + joffset, z);

          data = qsub8(data, 16);
          data = qadd8(data, scale8(data, 39));

          if (dataSmoothing) {
            uint8_t olddata = noise[0][i][j];
            uint8_t newdata = scale8(olddata, dataSmoothing) + scale8(data, 256 - dataSmoothing);
            data = newdata;
          }

          noise[0][i][j] = data;
        }
      }
      z += 16;

      // apply slow drift to X and Y, just for visual variation.
      x += 2;
      y --;
    }
  public:
    void run(bool load) override {
      fillNoise();
      memset8(&noise[1][0][0], 255, (LED_COLS + 1) * (LED_ROWS + 1));
      for (byte x = 0; x < LED_COLS; x++) {
        for (byte y = 0; y < LED_ROWS; y++) {
          uint8_t n0 = noise[0][x][y];
          uint8_t n1 = noise[0][x + 1][y];
          uint8_t n2 = noise[0][x][y + 1];
          int8_t xl = n0 - n1;
          int8_t yl = n0 - n2;
          int16_t xa = (x * 255) + ((xl * ((n0 + n1) )) >> 4);
          int16_t ya = (y * 255) + ((yl * ((n0 + n2) )) >> 4);
          wu_pixel(xa, ya);
        }
      }
      for (byte i = 0; i < LED_COLS; i++) {
        for (byte j = 0; j < LED_ROWS; j++) {
          nblend(leds[XY(i, j)], CHSV(150, noise[1][i][j], 255), 196);
        }
      }
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
      case 6: effPtr = new EFF_CAUSTIC; break;
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
