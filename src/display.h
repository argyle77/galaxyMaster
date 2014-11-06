// File:   display.h
// Author: Joshua Krueger
// Created on November 1, 2014, 1:44 PM

#ifndef DISPLAY_H
#define	DISPLAY_H

  // Include
  #include "galaxyConfig.h"

// These are the available output maps for writing to the galaxy.
  // MAP_MIRROR results in the first 1/2 of the pixel array being written
  // symmetrically to each arm.  Zero pixel is the outermost tip of both arms.
  // ((max_pixel + ) / 2) - 1 is the innermost point.
  // MAP_FULL results in the whole map being written to the galaxy.  Zero pixel
  // is the outermost tip of one arm, max_pixel is the outermost tip of the
  // other arm.  max_pixel / 2 is the center of the galaxy.
  typedef enum { MAP_MIRROR, MAP_FULL } galaxyMap_e;

  // Public prototypes
  void WriteLights(galaxyData_t *galaxyData, galaxyMap_e map);

#endif	/* DISPLAY_H */
