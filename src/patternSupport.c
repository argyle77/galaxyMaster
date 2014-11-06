// Joshua Krueger
// 2014_11_01
// Simple galaxy array manipulations for pattern support.

#include "deviceConfig.h"
#include "patternSupport.h"
#include <stdlib.h> // rand();

void FadeChannel(galaxyData_t *galaxy, colorChannel_e channel, int amount, aMode_e mode) {

  // Vars
  int i, value;

  switch(mode) {
    case MODE_MODULAR:
      // Use modular arithmetic... Rollover the upper / lower bounds.
      // Good for sudden brightness / sudden darkness effects.
      for (i = 0; i < galaxy->size ; i++) {
        galaxy->pixels[i]->chan[channel] += amount;
      }
      break;

    case MODE_CONSTRAINED:
      // Constrain the channel value between the upper and lower bounds.
      for (i = 0; i < galaxy->size ; i++) {
        value = galaxy->pixels[i]->chan[channel];
        value += amount;
        if (amount < 0) {
          if (value < 0) {
            value = 0;
          }
        } else {
          if (value > 255) {
            value = 255;
          }
        }
        galaxy->pixels[i]->chan[channel] = value;
      }
      break;

    default:
      break;
  }
}


// Color all pixels a chosen color.
void ColorAll(galaxyData_t *galaxy, color_t color) {
  int i;

  for (i = 0; i < galaxy->size; i++) {
    *galaxy->pixels[i] = color;
  }
}


// Rotate the pixels down the arms based on the given map.
void Shift(galaxyData_t *galaxy, sMode_e dir, galaxyMap_e map) {
  color_t tmp1, tmp2;
  int i;

  // Save aside the last pixel(s).
  if (dir == SHIFT_NEGATIVE) {
    // Negative shift (left or outward)
    tmp1 = *galaxy->pixels[0];                  // Both FULL and MIRROR.
    tmp2 = *galaxy->pixels[galaxy->size - 1];   // Just for MIRROR.
    // A note about the above constructs:  galaxy->pixels[0] is a pointer to a
    // color_t structure (actually a union).  Structs and unions can be assigned
    // in c just like ints and floats, without having to use memcpy().
    // Dereferencing the pointer, (*galaxy->pixels[0]) gives us the color value
    // of the pixel replete with all three color channels, permitting us to copy
    // those values into the tmp vars in a single assignment.
  } else {
    // Positive shift (right or inward)
    if (map == MAP_MIRROR) {
      // inward
      tmp1 = *galaxy->pixels[galaxy->size / 2 - 1];
      tmp2 = *galaxy->pixels[galaxy->size / 2];
    } else { // map == MAP_FULL
      // right
      tmp1 = *galaxy->pixels[galaxy->size - 1];
    }
  }

  // Do the shifts
  if (map == MAP_FULL) {
    if (dir == SHIFT_POSITIVE) {
      // Shift everything right.
      for (i = galaxy->size - 1; i > 0; i--) {
        *galaxy->pixels[i] = *galaxy->pixels[i - 1];
      }
    } else {
      // Shift everything left.
      for (i = 0; i < galaxy->size - 1; i++) {
        *galaxy->pixels[i] = *galaxy->pixels[i + 1];
      }
    }
  } else {
    if (dir == SHIFT_POSITIVE) {
      // Shift left side inward.
      for (i = galaxy->size / 2 - 1; i > 0; i--) {
        *galaxy->pixels[i] = *galaxy->pixels[i - 1];
      }
      // Shift right side inward.
      for (i = galaxy->size / 2; i < galaxy->size - 1; i++) {
        *galaxy->pixels[i] = *galaxy->pixels[i + 1];
      }
    } else {
      // Shift right side outward.
      for (i = 0; i < galaxy->size / 2 - 1; i++) {
        *galaxy->pixels[i] = *galaxy->pixels[i + 1];
      }
      // Shift left side outward.
      for (i = galaxy->size - 1; i > galaxy->size / 2 + 1; i--) {
        *galaxy->pixels[i] = *galaxy->pixels[i - 1];
      }
    }
  }

  // Restore the last pixel(s).
  if (dir == SHIFT_NEGATIVE) {
    if (map == MAP_MIRROR) {
      // Outward
      *galaxy->pixels[galaxy->size / 2 - 1] = tmp1;
      *galaxy->pixels[galaxy->size / 2] = tmp2;
    } else {
      // Left
      *galaxy->pixels[galaxy->size - 1] = tmp1;
    }
  } else {
    if (map == MAP_MIRROR) {
      // Inward
      *galaxy->pixels[0] = tmp1;
      *galaxy->pixels[galaxy->size - 1] = tmp2;
    } else {
      // Right
      *galaxy->pixels[0] = tmp1;
    }
  }
}


// Returns a random color according to the color mode rule as follows:
// CMODE_PRIMARY - Returns Red, Green, or Blue.
// CMODE_PRIMARY_W - Same as primary, except may include White.
// CMODE_SECONDARY - Returns Red, Yellow, Green, Cyan, Blue, or Magenta
// CMODE_SECONDARY_W - Same as CMODE_SECONDARY, but also may include White.
// CMODE_TERTIARY - Returns Red, Orange, Yellow, Chartreuse, Green, Aqua, Cyan, Azure, Blue, Violet, Magenta, or Rose.
// CMODE_TERTIARY_W - Same as CMODE_TERTIARY, but also may include White.
// CMODE_GREY - Dark Grey, Grey, Light Grey, White
// CMODE_GREY_B - Same as CMODE_GREY, but may include Black.
// CMODE_ANY_GREY - Any of 256 shades of grey.
// CMODE_ANY - Returns any random combination of the RGB channels.
color_t GetRandomColor(cMode_e colorMode) {
  color_t returnValue;
  int a;

  switch (colorMode) {
    case CMODE_PRIMARY:
      // 3 colors - (0,1,2) * 4 = (0,4,8) -> RED, GREEN, BLUE from the colors array.
      returnValue = *colors[(rand() % 3) * 4];
      break;

    case CMODE_PRIMARY_W:
      a = rand() % 4;
      if (a < 3) {
        returnValue = *colors[a * 4];
      } else {
        return PIXEL_WHITE;
      }
      break;
      
    case CMODE_SECONDARY:
      returnValue = *colors[(rand() % 6) * 2];
      break;

    case CMODE_SECONDARY_W:
      a = rand() % 7;
      if (a < 6) {
        returnValue = *colors[a * 2];
      } else {
        returnValue = PIXEL_WHITE;
      }
      break;

    case CMODE_TERTIARY:
      returnValue = *colors[(rand() % 12)];
      break;

    case CMODE_TERTIARY_W:
      a = rand() % 13;
      if (a < 12) {
        returnValue = *colors[a];
      } else {
        returnValue = PIXEL_WHITE;
      }
      break;

    case CMODE_GREY:
      returnValue = *colorsMono[(rand() % 4) + 1];
      break;

    case CMODE_GREY_B:
      returnValue = *colorsMono[rand() % 5];
      break;

    case CMODE_ANY_GREY:
      a = rand() % 256;
      returnValue.r = returnValue.g = returnValue.b = a;
      break;

    case CMODE_ANY:
    default:
      returnValue.r = rand() % 256;
      returnValue.g = rand() % 256;
      returnValue.b = rand() % 256;
      break;
  }

  return returnValue;
}
