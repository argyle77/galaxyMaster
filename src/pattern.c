// Joshua Krueger
// 2014_11_02
// Galaxy master pattern code.

// Includes
#include "display.h"
#include "patternSupport.h"
#include "galaxyConfig.h"
#include "pattern.h"
#include <stdlib.h>       // rand()

// Coefficients - Pattern adjustments
// COEF_FADER_SPREAD - Adjust this to make the fader more or less gentle. Higher
// values increase the sharpness of the fade and introduce new seams.
#define COEF_FADER_SPREAD 5
// COEF_FADE_VALUE - Amount of increase/decrease of pixel brightness per step.
#define COEF_FADE_VALUE 3
// COEF_RAINBOW_FADE_VALUE - Sharpness / speed of the rainbow fade.
#define COEF_RAINBOW_FADE_VALUE 8
#define COEF_STROBE_FREQ_STEP 700   // How fast to sweep the strobe frequencies
#define COEF_MAX_STROBE_DELAY 15000 // Slowest strobe.
#define COEF_MIN_STROBE_DELAY 0     // Fastest strobe.


// FaderMovingSeam - Initially fill the array with a color fade in one of the
// channels (leaving the others with artifacts from previous patterns), then, on
// each step, increase or decrease the brightness value of each pixel in the
// array, allowing the value to roll over the limits (0-255).  This creates a
// smoothly varying background with a travelling seam.
void FaderMovingSeam(galaxyData_t *galaxy, unsigned char initial, outputMapping_e *map) {
  static colorChannel_e colorChannel = 0;
  static unsigned char fadeDirection = 0;
  int i;

  // On the first time through, fill the array with our fade color.
  if (initial) {
    *map = MAP_MIRROR;  // Arms are mirrored.
    colorChannel = rand() % 3;   // Choose a color channel.
    if (rand() % 2) {            // Choose fade direction.
      fadeDirection = -1;
    } else {
      fadeDirection = 1;
    }

    // Populate the arm with a colorwash on one of the channels, leaving the
    // other channels untouched.    
    for (i = 0; i < (galaxy->size / 2); i++) {
      galaxy->pixels[i]->chan[colorChannel] = i * COEF_FADER_SPREAD;
    } // End of fill loop.
  } // End of pattern initialization block.

  // Do the fade up or down depending on what rnum ended up being.
  // Only fade the selected channel (section from the initialization block).
  FadeChannel(galaxy, colorChannel, fadeDirection * COEF_FADE_VALUE, MODE_MODULAR);
}


// RGBSharpRotate - Initially fill the array with the pattern RGBRGB,...
// On each step, rotate the pattern down (or up) the arms.
void RGBSharpRotate(galaxyData_t *galaxy, unsigned char initial, outputMapping_e *map) {
  int i;
  static sMode_e shiftDirection = SHIFT_POSITIVE;

  // Populate the arm with R,G,B,R,G,B,R,G,B,R,G,B,R,G,B,R,G,B,R,G,B
  // This only needs to be done once - the rest of the pattern just shifts
  // the pixels down the arm repeatedly.
  if (initial) {
    *map = MAP_MIRROR;

    // Choose a direction.
    if (rand() % 2) {
      shiftDirection = SHIFT_POSITIVE;
    } else {
      shiftDirection = SHIFT_NEGATIVE;
    }

    // This may hurt your brain - its a somewhat obfuscated way of
    // accomplishing the above pattern.  It is often suggested that the
    // ternary conditional (?:) is a difficult expression to understand at
    // a glance.  The first line:
    // galaxy->pixels[i]->r = ((i + 0) % 3) == 0 ? 0xff : 0x00;
    // Is equivalent to:
    // if (((i + 0) % 3) == 0) {
    //   galaxy->pixels[i]->r = 0xff;
    // } else {
    //   galaxy->pixels[i]->r = 0x00;
    // }
    for (i = 0; i < PIXELS_PER_ARM; i++) {
      galaxy->pixels[i]->r = ((i + 0) % 3) == 0 ? 0xff : 0x00;
      galaxy->pixels[i]->g = ((i + 1) % 3) == 0 ? 0xff : 0x00;
      galaxy->pixels[i]->b = ((i + 2) % 3) == 0 ? 0xff : 0x00;
    }
  } /* End initial */
  
  // Shift (rotate) the pixels.
  Shift(galaxy, shiftDirection, *map);

  // Delay - otherwise this things goes so fast all you see is a whiteish strobe.
  Delay(30000);
}


// RainbowFader - Red, Yellow, Green, Cyan, Blue, Magenta, repeat.
// Fader works by coloring the first pixel (using the last pixel's value),
// but then shifting the entire array down the arm.
void RainbowFader(galaxyData_t *galaxy, unsigned char initial, outputMapping_e *map) {
  static int transition = 0;

  if (initial) {
    *map = MAP_MIRROR;
  }

  // transition is used to remember which colors we are transitioning between
  // in this pattern.  There are 6 transitions...
  switch(transition) {
    case 0:
      // Red -> Yellow (Green increases)
      galaxy->pixels[0]->r = galaxy->pixels[PIXELS_PER_ARM - 1]->r;
      galaxy->pixels[0]->g = galaxy->pixels[PIXELS_PER_ARM - 1]->g + COEF_RAINBOW_FADE_VALUE;
      galaxy->pixels[0]->b = galaxy->pixels[PIXELS_PER_ARM - 1]->b;

      // Check if green has hit the end of the transition (fully saturated).
      if (galaxy->pixels[0]->g > 255 - COEF_RAINBOW_FADE_VALUE) {
        
        // It has, so set it to max and move on to next section.
        galaxy->pixels[0]->g = 255;
        transition++;
      }
      break;
    
    case 1:
      // Yellow -> Green (Red decreases)
      galaxy->pixels[0]->r = galaxy->pixels[PIXELS_PER_ARM - 1]->r - COEF_RAINBOW_FADE_VALUE;
      galaxy->pixels[0]->g = galaxy->pixels[PIXELS_PER_ARM - 1]->g;
      galaxy->pixels[0]->b = galaxy->pixels[PIXELS_PER_ARM - 1]->b;
      
      if (galaxy->pixels[0]->r < COEF_RAINBOW_FADE_VALUE) {
        galaxy->pixels[0]->r = 0;
        transition++;
      }
      break;

    case 2:
      // Green -> Cyan (Blue increases)
      galaxy->pixels[0]->r = galaxy->pixels[PIXELS_PER_ARM - 1]->r;
      galaxy->pixels[0]->g = galaxy->pixels[PIXELS_PER_ARM - 1]->g;
      galaxy->pixels[0]->b = galaxy->pixels[PIXELS_PER_ARM - 1]->b + COEF_RAINBOW_FADE_VALUE;
      
      if (galaxy->pixels[0]->b > 255 - COEF_RAINBOW_FADE_VALUE) {
        galaxy->pixels[0]->b = 255;
        transition++;
      }
      break;

    case 3:
      // Cyan -> Blue (Green decreases)
      galaxy->pixels[0]->r = galaxy->pixels[PIXELS_PER_ARM - 1]->r;
      galaxy->pixels[0]->g = galaxy->pixels[PIXELS_PER_ARM - 1]->g - COEF_RAINBOW_FADE_VALUE;
      galaxy->pixels[0]->b = galaxy->pixels[PIXELS_PER_ARM - 1]->b;
      
      if (galaxy->pixels[0]->g < COEF_RAINBOW_FADE_VALUE) {
        galaxy->pixels[0]->r = 0;
        transition++;
      }
      break;

    case 4:
      // Blue -> Magenta (Red increases)
      galaxy->pixels[0]->r = galaxy->pixels[PIXELS_PER_ARM - 1]->r + COEF_RAINBOW_FADE_VALUE;
      galaxy->pixels[0]->g = galaxy->pixels[PIXELS_PER_ARM - 1]->g;
      galaxy->pixels[0]->b = galaxy->pixels[PIXELS_PER_ARM - 1]->b;
      
      if (galaxy->pixels[0]->r > 255 - COEF_RAINBOW_FADE_VALUE) {
        galaxy->pixels[0]->r = 255;
        transition++;
      }
      break;

    case 5:
      // Magenta -> Red (Blue decreases
      galaxy->pixels[0]->r = galaxy->pixels[PIXELS_PER_ARM - 1]->r;
      galaxy->pixels[0]->g = galaxy->pixels[PIXELS_PER_ARM - 1]->g;
      galaxy->pixels[0]->b = galaxy->pixels[PIXELS_PER_ARM - 1]->b - COEF_RAINBOW_FADE_VALUE;

      if (galaxy->pixels[0]->b < COEF_RAINBOW_FADE_VALUE) {
        galaxy->pixels[0]->b = 0;
        transition = 0;
      }
      break;

    default:
      // Shouldn't get here, but just in case...
      transition = 0;
      break;
  }
  
  // Shift - Gotta be negative unless I change a bunch of things above?
  Shift(galaxy, SHIFT_NEGATIVE, *map);
}


// SequenceTest - Shift a single pixel of color across the array.
void SequenceTest(galaxyData_t *galaxy, unsigned char initial, outputMapping_e *map) {

  static sMode_e shiftDir = SHIFT_NEGATIVE;

  if (initial) {
    *map = MAP_MIRROR;

    if (rand() % 2) {
      shiftDir = SHIFT_NEGATIVE;
    } else {
      shiftDir = SHIFT_POSITIVE;
    }
    
    // Clear to black.
    ColorAll(galaxy, PIXEL_BLACK);

    // Choose a color for the first pixel.
    *galaxy->pixels[0] = GetRandomColor(CMODE_TERTIARY_W);
  }

  // Shifts the pixel set in the initialization block down the arm.
  Shift(galaxy, shiftDir, *map);
}


// VarStrobe - Color strobe with changing frequency.  Chooses a color to strobe,
// then goes between it and black at every step while sweeping the delay time
// up and down to change the frequency.
void VariableStrobe(galaxyData_t *galaxy, unsigned char initial, outputMapping_e *map) {
  static unsigned char phase = 0;
  static int strobeDelay = 0, delayStep = 0;
  static color_t tmp;
  int i;
  
  if (initial) {
    *map = MAP_MIRROR;
    phase = 0;  // This is used to figure out if we are on or off this step.
    strobeDelay = 0;
    delayStep = COEF_STROBE_FREQ_STEP;

    tmp = GetRandomColor(CMODE_TERTIARY_W);
  } // End initialization block.

  // Are we on or off?
  phase = (phase + 1) % 2;
  if (phase) {
    // ON
    for (i = 0; i < PIXELS_PER_ARM; i++) {
      *galaxy->pixels[i] = tmp;
    }
  } else {
    // OFF
    for (i = 0; i < PIXELS_PER_ARM; i++) {
      *galaxy->pixels[i] = PIXEL_BLACK;
    }
  }

  strobeDelay = strobeDelay + delayStep;
  
  // If the delay has hit a limit, reverse the step.
  if ((strobeDelay >= COEF_MAX_STROBE_DELAY) ||
      (strobeDelay <= COEF_MIN_STROBE_DELAY)) {
    delayStep = delayStep * -1;
  }

  Delay(strobeDelay);
}


// Scroll a sequence of random colors across the array in a random direction.
void RandomMarquee(galaxyData_t *galaxy, unsigned char initial, outputMapping_e *map) {
  static cMode_e colorMode;
  static sMode_e direction;

  if (initial) {
    *map = MAP_FULL;
    
    // Choose a random color mode.
    colorMode = rand() % CMODE_COUNT;
    direction = rand() % 2;
  }

  // Load a color into pixel 0.
  if (direction == SHIFT_POSITIVE) {
    *galaxy->pixels[galaxy->size - 1] = GetRandomColor(colorMode);
  } else {
    *galaxy->pixels[0] = GetRandomColor(colorMode);
  }

  // Shift.
  Shift(galaxy, direction, MAP_FULL);

  // Delay.
  Delay(20000);
}
