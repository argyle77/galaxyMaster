// File:   manips.h
// Author: argyle
// Created on November 1, 2014, 4:17 PM

#ifndef MANIPS_H
#define	MANIPS_H

  #include "display.h"        // Display output functions
  #include "galaxyConfig.h"

  // Define some named colors.
  // We want these to only actually be allocated in main.  Actually, we'd
  // rather they weren't allocated at all, but implemented as preprocessor
  // defines. This would cause them to take up no space in flash unless they
  // were actually used in the program. But the C90 standard used by the XC8
  // compiler doesn't permit the use of compound literals, so we have to settle
  // for allocated constants.  Regardless, here is an example of the compound
  // literal form:
  // #define PIXEL_BLACK ((color_t) {{0, 0, 0}})
  #ifdef _MAIN_C_
    const color_t PIXEL_RED        = {{255,   0,   0}};
    const color_t PIXEL_ORANGE     = {{255, 127,   0}};
    const color_t PIXEL_YELLOW     = {{255, 255,   0}};
    const color_t PIXEL_CHARTREUSE = {{127, 255,   0}};
    const color_t PIXEL_GREEN      = {{  0, 255,   0}};
    const color_t PIXEL_AQUA       = {{  0, 255, 127}};
    const color_t PIXEL_CYAN       = {{  0, 255, 255}};
    const color_t PIXEL_AZURE      = {{  0, 127, 255}};
    const color_t PIXEL_BLUE       = {{  0,   0, 255}};
    const color_t PIXEL_VIOLET     = {{127,   0, 255}};
    const color_t PIXEL_MAGENTA    = {{255,   0, 255}};
    const color_t PIXEL_ROSE       = {{255,   0, 127}};
    const color_t PIXEL_WHITE      = {{255, 255, 255}};
    const color_t PIXEL_LT_GREY    = {{191, 191, 191}};
    const color_t PIXEL_GREY       = {{127, 127, 127}};
    const color_t PIXEL_DARK_GREY  = {{ 63,  63,  63}};
    const color_t PIXEL_BLACK      = {{  0,   0,   0}};
    // And a convenient way to access them in numbered order:
    const color_t *colors[] = {&PIXEL_RED, &PIXEL_ORANGE, &PIXEL_YELLOW,
      &PIXEL_CHARTREUSE, &PIXEL_GREEN, &PIXEL_AQUA, &PIXEL_CYAN, &PIXEL_AZURE,
      &PIXEL_BLUE, &PIXEL_VIOLET, &PIXEL_MAGENTA, &PIXEL_ROSE};
    const color_t *colorsMono[] = {&PIXEL_BLACK, &PIXEL_DARK_GREY, &PIXEL_GREY, &PIXEL_LT_GREY, &PIXEL_WHITE};
  #else /* _MAIN_C_ */
    // Outside of main, we'll tell our libraries that they exist as placeholders
    // so that the linker can connect them to the master.c allocations.
    extern const color_t PIXEL_RED, PIXEL_ORANGE, PIXEL_YELLOW, PIXEL_CHARTREUSE,
                         PIXEL_GREEN, PIXEL_AQUA, PIXEL_CYAN, PIXEL_AZURE,
                         PIXEL_BLUE, PIXEL_VIOLET, PIXEL_MAGENTA, PIXEL_ROSE;
    extern const color_t PIXEL_WHITE, PIXEL_LT_GREY, PIXEL_GREY, PIXEL_DARK_GREY, PIXEL_BLACK;
    extern const color_t* colors[];
    extern const color_t* colorsMono[];
  #endif /* _MAIN_C_ */

  // Finally, an enumeration for named access to the colors array.
  typedef enum {
    COLOR_RED, COLOR_ORANGE, COLOR_YELLOW, COLOR_CHARTREUSE, COLOR_GREEN,
    COLOR_AQUA, COLOR_CYAN, COLOR_AZURE, COLOR_BLUE, COLOR_VIOLET,
    COLOR_MAGENTA, COLOR_ROSE
  } color_e;

  typedef enum {
    COLOR_BLACK, COLOR_DARK_GREY, COLOR_GREY, COLOR_LT_GREY, COLOR_WHITE
  } mono_e;

  // Array manipulation modes.
  typedef enum {
    MODE_MODULAR, MODE_CONSTRAINED
  } aMode_e;

  // Shift modes
  typedef enum {
    SHIFT_POSITIVE, SHIFT_NEGATIVE
  } sMode_e;

  // Color selection modes.
  typedef enum {
    CMODE_PRIMARY, CMODE_PRIMARY_W, CMODE_SECONDARY, CMODE_SECONDARY_W,
    CMODE_TERTIARY, CMODE_TERTIARY_W, CMODE_GREY, CMODE_GREY_B, CMODE_ANY_GREY,
    CMODE_ANY,
    CMODE_COUNT // Must be last! If you add more, this number is kept up-to-date.
  } cMode_e;

  // Prototypes
  void FadeChannel(galaxyData_t *galaxy, colorChannel_e channel, int amount, aMode_e mode);
  void Shift(galaxyData_t *galaxy, sMode_e dir, outputMapping_e map);
  void ColorAll(galaxyData_t *galaxy, color_t color);
  color_t GetRandomColor(cMode_e getColorMode);
  
#endif	/* MANIPS_H */
