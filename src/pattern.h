// File:   pattern.h
// Author: argyle
// Created on November 2, 2014, 8:19 PM

#ifndef PATTERN_H
#define	PATTERN_H


// Pattern functions should all have the same signature.
void FaderMovingSeam(galaxyData_t *galaxy, unsigned char initial, galaxyMap_e *map);
void RGBSharpRotate(galaxyData_t *galaxy, unsigned char initial, galaxyMap_e *map);
void RainbowFader(galaxyData_t *galaxy, unsigned char initial, galaxyMap_e *map);
void SequenceTest(galaxyData_t *galaxy, unsigned char initial, galaxyMap_e *map);
void VariableStrobe(galaxyData_t *galaxy, unsigned char initial, galaxyMap_e *map);
void RandomMarquee(galaxyData_t *galaxy, unsigned char initial, galaxyMap_e *map);


// This struct contains a pointer to a pattern function and the number of times
// to run that pattern.
typedef struct {
  void (*patternFunction)(galaxyData_t *, unsigned char, galaxyMap_e *);
  long int iterations;
} pattern_t;

// This is the array of the pattern functions to run.  It should only be
// allocated once in main.
#ifdef _MAIN_C_

  const pattern_t patternList[] = {
    { FaderMovingSeam, 1200 },
    { RGBSharpRotate, 50 },
    { RainbowFader, 1000 },
    { SequenceTest, 600 },
    { VariableStrobe, 400 },
    { RandomMarquee, 100 }
  };

  // The number of patterns to choose from...
  #define PATTERN_COUNT ((int)(sizeof(patternList ) / sizeof(pattern_t)))

#endif

#endif	/* PATTERN_H */
