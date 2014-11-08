// Joshua Krueger
// 2014_11_01
// Galaxy master device initialization functions.

#include "init.h"
#include "deviceConfig.h"

// Initialize the PIC hardware.
void HardwareInit(void) {

#ifndef EMULATE

  // Assign the I/O port senses.  On startup, all ports default to input (1).
  DEBUG_LED_PORTCTRL = PORT_OUTPUT;  // Address transmit LED (for debug).

  // Open the serial port
  OpenUSART(USART_TX_INT_OFF  & USART_RX_INT_OFF & \
            USART_ASYNCH_MODE & USART_NINE_BIT & \
            BAUDMODE, BAUDVALUE_X);

#endif /* EMULATE */

}
