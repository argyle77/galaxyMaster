// File:   galaxyConfig.h
// Author: Joshua Krueger
// Created on November 1, 2014, 12:34 PM
// Contains galaxy definitions.

#ifndef GALAXY_H
#define	GALAXY_H

  // Useful defines.
  #define FOREVER for(;;)
  #define FALSE 0
  #define TRUE 1

  // Hardware defines. There are 42 LED triplets total, 21 on each arm.
  #define PIXEL_COUNT 42
  #define ARM_COUNT 2
  #define PIXELS_PER_ARM (PIXEL_COUNT / ARM_COUNT)
  #define BYTES_PER_PIXEL 3   // 1 byte red, 1 byte green, 1 byte blue.
  #define SLAVE_COUNT 9       // There are 9 LED modulator slaves to communicate to.

  // Data packet delay for emulator target (milliseconds):
  // PACKET_TIME = PACKET_BYTES * BITS_PER_BYTE * BIT_RATE * 1000ms/s
  // A full packet transmission is (42 pixels * 3 bytes per pixel) + 9 slave addresses
  #define PACKET_BYTES ((PIXEL_COUNT * BYTES_PER_PIXEL) + SLAVE_COUNT)
  #define BITS_PER_BYTE 11  // USART 9 bit addressing - 1 Start, 8 Data, 1 Address select, 1 Stop
  #define PACKET_TIME (PACKET_BYTES * BITS_PER_BYTE * BIT_RATE * 1000)  // ms

  // Convenience types
  // Color channels - Placing CHANNEL_COUNT at the end automatically makes this
  // enumeration value available to use as the proper count.
  typedef enum { RED = 0, GREEN, BLUE, CHANNEL_COUNT } colorChannel_e;

  // When operating on a pixel, we want to make sure we can use both named
  // channel addressing as well as array (enumeration) addressing for the color
  // channels.  This union accomplishes that objective.
  typedef union {
    unsigned char chan[CHANNEL_COUNT];  // First element of the union - array
                                        // addressing.
    struct { unsigned char r, g, b; };  // Second element - named addressing
                                        // using an anonymous struct.
  } color_t;

  // The galaxy's pixel array.  Note this is only an array of pointers.  The
  // actual pixels must be seperately allocated (see main() in master.c).
  typedef struct {
    color_t *pixels[PIXEL_COUNT];  // Array of pointers to the galaxy's pixels.
    int size;
  } galaxyData_t;


  // So that Delay() can be used outside of master.c
  #ifndef _MAIN_C_
  extern void Delay(long int d);  
  #endif
  
#endif	/* GALAXY_H */
