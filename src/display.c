// File: display.c
// Author: Joshua Krueger
// Created: 2014_11_01

// Galaxy master display driver.

// Includes
#include "deviceConfig.h"
#include "display.h"

// Prototypes
void TransmitAddress(unsigned char value);
void TransmitData(unsigned char value);

// Write out a whole set of instructions to the slaves to set the lights.
// Note, we mirror the output to each arm, making all patterns symmetrical.
// This wasn't required... It was lazy.
// Including all data bytes and address byte transmissions, we are writing out
// 135 bytes in the code below.  A byte consists of 1 start bit, 8 data bits,
// 1 address bit, 1 stop bit, for a total of 11 bits per byte.  This makes a
// total of 1485 bits written out by this blocking code.  This is our rate
// limiting step.  At 69.4 kbps (transmission baud rate), this means a full
// update to all the lights takes 21.3 ms.  This translates to a maximum
// galactic frame rate of 46.8 FPS (1 / 21.3ms).  This code could be re-written
// to take advantage of interrupts, as well as adding special control codes for
// common operations (like full galactic blanking or all light color update,
// partial frame updates, sync signals, etc), which would allow for a
// significantly faster frame rate and more time to do number crunching, but
// we aren't there yet.  A change like that would significantly change the way
// all patterns are written.  For now, the emulation will set a timer to wait
// for the transmission time so that the output more closely matches the PIC
// target output.
void WriteLights(galaxyData_t *gData, outputMapping_e mapType) {
  int i;
  // Variable chipAddressMap is a map of the slave chip addresses.  These values
  // were originally chosen because the initial pattern generation platform did
  // not properly support 9 bit serial addressing or switching on the fly between
  // MARK and SPACE parity.  I had to use ODD parity with 8 data bits to emulate
  // the 9 bit serial scheme that the PIC slaves use to distinguish data bytes
  // from address bytes in serial communications.  The addresses below, therefore,
  // all have the same parity to indicate that they are addresses.  The data
  // bytes (at the time) had to be constrained to avoid this parity (effectively
  // eliminating 1/2 the available intensity values. Thankfully, this is no longer
  // the case, however, the addresses remain as an artifact of the earlier
  // situation.
  const unsigned char chipAddressMap[] = {0x03, 0x05, 0x06, 0x09, 0x0a, 0x12, 0x11, 0x0f, 0x0c};

  // First 15 leds. (5 triplets) - 16 bytes.
  TransmitAddress(chipAddressMap[0]);
  for (i = 20; i > 15; i--) {
    TransmitData(gData->pixels[i]->g);
    TransmitData(gData->pixels[i]->b);
    TransmitData(gData->pixels[i]->r);
  }

  // Next 15 leds. (5 triplets) - 16 bytes
  TransmitAddress(chipAddressMap[1]);
  for (i = 15; i > 10; i--) {
    TransmitData(gData->pixels[i]->g);
    TransmitData(gData->pixels[i]->b);
    TransmitData(gData->pixels[i]->r);
  }

  // third 15 leds. (5 triplets) - 16 bytes
  TransmitAddress(chipAddressMap[2]);
  for (i = 10; i > 5; i--) {
    TransmitData(gData->pixels[i]->g);
    TransmitData(gData->pixels[i]->b);
    TransmitData(gData->pixels[i]->r);
  }

  // fourth 15 leds.  (5 triplets) - 16 bytes
  TransmitAddress(chipAddressMap[3]);
  for (i = 5; i > 0; i--) {
    TransmitData(gData->pixels[i]->g);
    TransmitData(gData->pixels[i]->b);
    TransmitData(gData->pixels[i]->r);
  }
 
  // Chip #4 -> 0x0a is the end cap / shared chip (slave #9 in the original galaxy)
  // It controls the last LED TRIPLET on each arm.
  TransmitAddress(chipAddressMap[4]); //  - 7 bytes
    TransmitData(gData->pixels[0]->g);
    TransmitData(gData->pixels[0]->b);
    TransmitData(gData->pixels[0]->r);

  if (mapType == MAP_MIRROR) {
    TransmitData(gData->pixels[0]->g);
    TransmitData(gData->pixels[0]->b);
    TransmitData(gData->pixels[0]->r);
  } else {
    TransmitData(gData->pixels[41]->g);
    TransmitData(gData->pixels[41]->b);
    TransmitData(gData->pixels[41]->r);
  }

  // First 15 leds. (5 triplets)  - 16 bytes
  if (mapType == MAP_MIRROR) {
  TransmitAddress(chipAddressMap[8]);
  for (i = 20; i > 15; i--) {
    TransmitData(gData->pixels[i]->g);
    TransmitData(gData->pixels[i]->b);
    TransmitData(gData->pixels[i]->r);
  }

  // Next 15 leds. (5 triplets)  - 16 bytes
  TransmitAddress(chipAddressMap[7]);
  for (i = 15; i > 10; i--) {
    TransmitData(gData->pixels[i]->g);
    TransmitData(gData->pixels[i]->b);
    TransmitData(gData->pixels[i]->r);
  }

  // third 15 leds. (5 triplets)  - 16 bytes
  TransmitAddress(chipAddressMap[6]);
  for (i = 10; i > 5; i--) {
    TransmitData(gData->pixels[i]->g);
    TransmitData(gData->pixels[i]->b);
    TransmitData(gData->pixels[i]->r);
  }

  // fourth 15 leds.  (5 triplets)  - 16 bytes
  TransmitAddress(chipAddressMap[5]);
  for (i = 5; i > 0; i--) {
    TransmitData(gData->pixels[i]->g);
    TransmitData(gData->pixels[i]->b);
    TransmitData(gData->pixels[i]->r);
  }
  } else {
  // 21 - 25. (5 triplets) - 16 bytes
  TransmitAddress(chipAddressMap[8]);
  for (i = 21; i < 26; i++) {
    TransmitData(gData->pixels[i]->g);
    TransmitData(gData->pixels[i]->b);
    TransmitData(gData->pixels[i]->r);
  }

  // 26 - 30. (5 triplets)  - 16 bytes
  TransmitAddress(chipAddressMap[7]);
  for (i = 26; i < 31; i++) {
    TransmitData(gData->pixels[i]->g);
    TransmitData(gData->pixels[i]->b);
    TransmitData(gData->pixels[i]->r);
  }

  // 31 - 35. (5 triplets)  - 16 bytes
  TransmitAddress(chipAddressMap[6]);
  for (i = 31; i < 36; i++) {
    TransmitData(gData->pixels[i]->g);
    TransmitData(gData->pixels[i]->b);
    TransmitData(gData->pixels[i]->r);
  }

  // 36 - 40.  (5 triplets)  - 16 bytes
  TransmitAddress(chipAddressMap[5]);
  for (i = 36; i < 41; i++) {
    TransmitData(gData->pixels[i]->g);
    TransmitData(gData->pixels[i]->b);
    TransmitData(gData->pixels[i]->r);
  }
  }
}


// Transmit a data byte on the serial port.
void TransmitData(unsigned char value) {
#ifndef EMULATE
  while (BusyUSART());  // Wait to make sure the last byte is done transmitting.
  TXSTAbits.TX9D = BYTETYPE_DATA;  // Set the next as a data byte.
  TXREG = value;  // Write the next byte to be shifted out.
#endif
}


// Transmit an address to the serial port.
void TransmitAddress(unsigned char value) {
#ifndef EMULATE
  DEBUG_LED = LED_ON;  // Debugging
  while (BusyUSART());  // Wait for the last byte to transmit.
  DEBUG_LED = LED_OFF;  // Debugging
  TXSTAbits.TX9D = BYTETYPE_ADDRESS;  // Set the next as an address.
  TXREG = value;  // Write the address to be shifted out.
#endif
}
