const uint16_t XYTable[] PROGMEM = {
  256, 257, 258, 259, 260, 261, 262, 263, 135, 136, 134, 133, 264, 265, 266, 267, 268, 269, 270, 271,
  272, 273, 274, 275, 276, 158, 157, 156, 155, 154, 137, 138, 132, 111, 277, 278, 279, 280, 281, 282,
  283, 284, 285, 286, 160, 161, 162, 163, 164, 165, 153, 287, 139, 131, 112, 110, 288, 289, 290, 291,
  292, 293, 294, 159, 179, 178, 177, 176, 175, 295, 166, 152, 296, 140, 130, 113, 109, 297, 298, 299,
  300, 301, 181, 180, 188, 189, 190, 191, 302, 174, 303, 167, 151, 141, 129, 114, 108, 304, 305, 306,
  307, 182, 186, 187, 308, 200, 199, 198, 192, 309, 173, 168, 150, 142, 310, 128, 115, 107,  86, 311,
  312, 185, 313, 202, 201, 216, 217, 314, 197, 193, 172, 169, 149, 143, 315, 127, 116, 106,  87, 316,
  317, 184, 203, 214, 215, 222, 221, 218, 318, 196, 194, 170, 148, 144, 319, 126, 117, 105,  88,  85,
  183, 204, 213, 320, 223, 241, 242, 220, 219, 195, 171, 147, 145, 321, 125, 118, 104, 322,  89,  84,
  205, 323, 212, 224, 240, 324, 245, 244, 243, 325, 326, 146, 123, 124, 327, 119, 103,  90,  83,  62,
  206, 211, 225, 328, 239, 246, 329,   1,  24,   0,  98, 330, 122, 121, 120, 102, 331,  91,  82,  63,
  207, 210, 226, 238, 247, 332,   2,  23,  25,  49,  74,  97,  99, 100, 101, 333,  92,  81,  64, 334,
  335, 209, 227, 237, 248,   3,  22,  26,  48,  50,  73,  75,  96,  95,  94,  93,  80,  65, 336,  61,
  337, 208, 228, 236, 249,   4,  21,  27,  47,  51,  72, 338,  76,  77,  78,  79,  66, 339,  60, 340,
  341, 342, 229, 235, 250,   5,  20,  28, 343,  46,  52,  71,  70,  69,  68,  67, 344,  59,  37, 345,
  346, 347, 230, 234, 251,   6,  19, 348,  29, 349,  45,  53,  54,  55,  56,  57,  58,  38, 350, 351,
  352, 353, 354, 231, 233, 252,   7,  18, 355,  30, 356,  44,  43,  42,  41,  40,  39, 357, 358, 359,
  360, 361, 362, 363, 232, 253,   8, 364,  17, 365,  31,  32,  33,  34,  35,  36, 366, 367, 368, 369,
  370, 371, 372, 373, 374, 375, 254,   9, 376,  16,  15,  14,  13, 377, 378, 379, 380, 381, 382, 383,
  384, 385, 386, 387, 388, 389, 390, 255,  10,  11,  12, 391, 392, 393, 394, 395, 396, 397, 398, 399
};

uint16_t XY(byte x, byte y) {
  uint16_t ledsindex = constrain(pgm_read_word(XYTable + y * LED_COLS + x), 0, 256);
  return (ledsindex);
}

const uint8_t WokwiBitmap[] PROGMEM = {
  0xff, 0xff, 0xf0, 0xff, 0xff, 0xf0, 0xff, 0xff, 0xf0, 0xff, 0xff, 0xf0, 0xff, 0x9e, 0x30, 0xcf,
  0x1e, 0xb0, 0xb7, 0x1e, 0x30, 0xc7, 0x9e, 0xf0, 0xef, 0xde, 0xf0, 0xf7, 0xcc, 0xf0, 0xf7, 0xed,
  0xf0, 0xf7, 0x8d, 0xf0, 0xfb, 0x23, 0xf0, 0xfa, 0x73, 0xf0, 0xf8, 0xe3, 0xf0, 0xf0, 0xeb, 0xf0,
  0xf0, 0xf3, 0xf0, 0xff, 0xff, 0xf0, 0xff, 0xff, 0xf0, 0xff, 0xff, 0xf0
};
void drawWokwiLogo() {
  uint8_t thisbyte = 0;
  for (uint8_t j = 0; j < LED_ROWS; j++) {
    for (uint8_t i = 0; i < LED_COLS; i++) {
      if (i & 7)
        thisbyte <<= 1;
      else
        thisbyte = pgm_read_byte(&WokwiBitmap[(LED_ROWS - 1 - j) * (27 / 8) + i / 8]);
      if (thisbyte & 0x80)
        leds[XY(i, j)] = CRGB::White; else leds[XY(i, j)] = CRGB(0, 0, 0);
    }
  }
}
void startupAnimation() {
  FastLED.clear();
  drawWokwiLogo();
  for (uint8_t i = 0; i < 255; i++) {
    delay(10);
    FastLED.setBrightness(map(i, 0, 255, 0, START_BRI));
    FastLED.show();
  }
  delay(100);
  for (uint8_t i = 255; i != 0; i--) {
    delay(10);
    FastLED.setBrightness(map(i, 0, 255, 0, START_BRI));
    FastLED.show();
  }
  FastLED.clear();
  FastLED.setBrightness(Light.brightness);
  FastLED.show();
}
void drawPixelXYF(float x, float y, CRGB color) {
  if (x < 0 || y < 0 || x > ((float)LED_COLS - 1) || y > ((float)LED_ROWS - 1)) return;
  uint8_t xx = (x - (int) x) * 255, yy = (y - (int) y) * 255, ix = 255 - xx, iy = 255 - yy;
  // calculate the intensities for each affected pixel
#define WU_WEIGHT(a, b)((uint8_t)(((a) * (b) + (a) + (b)) >> 8))
  uint8_t wu[4] = {
    WU_WEIGHT(ix, iy),
    WU_WEIGHT(xx, iy),
    WU_WEIGHT(ix, yy),
    WU_WEIGHT(xx, yy)
  };
  // multiply the intensities by the colour, and saturating-add them to the pixels
  for (uint8_t i = 0; i < 4; i++) {
    int16_t xn = x + (i & 1), yn = y + ((i >> 1) & 1);
    CRGB clr = leds[XY(xn, yn)];
    clr.r = qadd8(clr.r, (color.r * wu[i]) >> 8);
    clr.g = qadd8(clr.g, (color.g * wu[i]) >> 8);
    clr.b = qadd8(clr.b, (color.b * wu[i]) >> 8);
    leds[XY(xn, yn)] = clr;
  }
}

void drawLineF(float x1, float y1, float x2, float y2,
               const CRGB & color) {
  float deltaX = fabs(x2 - x1);
  float deltaY = fabs(y2 - y1);
  float error = deltaX - deltaY;

  float signX = x1 < x2 ? 0.5 : -0.5;
  float signY = y1 < y2 ? 0.5 : -0.5;

  while (x1 != x2 || y1 != y2) {
    if ((signX > 0. && x1 > x2 + signX) || (signX < 0. && x1 < x2 + signX)) break;
    if ((signY > 0. && y1 > y2 + signY) || (signY < 0. && y1 < y2 + signY)) break;
    drawPixelXYF(x1, y1, color);
    float error2 = error;
    if (error2 > -deltaY) {
      error -= deltaY;
      x1 += signX;
    }
    if (error2 < deltaX) {
      error += deltaX;
      y1 += signY;
    }
  }
}
