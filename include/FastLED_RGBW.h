/*
 * CRGBW pixel type used by FlummyLights.
 *
 * FastLED is still used for color math and CRGB compatibility, while the
 * physical SK6812 RGBW output is handled by rgbw_output.cpp.
 */

#ifndef FastLED_RGBW_h
#define FastLED_RGBW_h

struct CRGBW  {
  union {
    struct {
      union {
        uint8_t g;
        uint8_t green;
      };
      union {
        uint8_t r;
        uint8_t red;
      };
      union {
        uint8_t b;
        uint8_t blue;
      };
      union {
        uint8_t w;
        uint8_t white;
      };
    };
    uint8_t raw[4];
  };

  CRGBW(){}

  CRGBW(uint8_t rd, uint8_t grn, uint8_t blu, uint8_t wht){
    r = rd;
    g = grn;
    b = blu;
    w = wht;
  }

  inline void operator = (const CRGB c) __attribute__((always_inline)){ 
    this->r = c.r;
    this->g = c.g;
    this->b = c.b;
    this->white = 0;
  }
};

#endif
