// File: master.c
// Author: Joshua Krueger
// Created: 2007_07_01, 2014_10_28

// Galaxy master code (base).
// The Galaxy Master is a PIC18F... that is responsible for the pattern
// generation.  It has (as yet) no inputs, and a single logic-level serial port
// output.  The serial port attaches to 9 slave PICs, which generate the 
// modulation signals for 126 LEDs based on the pattern code.  The LEDs are
// grouped in RGB triplets, making for 42 (126/3) total pixels. There are 21
// pixels per arm.

// This code is written primarily for the Microchip XC8 Compiler (v1.33)
// targeting the  PIC18F452 microcontroller.  The code includes a second target,
// "EMULATE" targeted to gcc compiler for x86 support with a graphical emulator
// output using SDL (Simple DirectMedia Layer).

// A note about xc8 - This is an ISO C90 (ANSI C) compiler.  Amongst those
// things notably not supported from the more modern C99 standard are "compound
// literals", which permit structure constants: (struct pixelData_t){0, 0, 0},
// and "designated initializers": point_t yermom = {.x = 1, .y = 2};  These
// things are acceptable on the EMULATE target, but not the PIC target.

#define _MAIN_C_

// Includes
#include "deviceConfig.h"   // Device config - Must be first.
#include "galaxyConfig.h"   // Useful defines, galaxy specifics, coefficients
#include "init.h"           // Hardware initialization functions
#include "display.h"        // Display output functions
#include "patternSupport.h"         // Pattern support - array manipulations.
#include "pattern.h"
#include <stdlib.h>         // srand(), rand(), exit(), EXIT_SUCCESS

// Function Prototypes
void GeneratePattern(galaxyData_t *galaxy);
void Delay (long int d);

// Emulation support
#ifdef EMULATE

// Emulator Includes
#include <SDL.h>
#include <SDL2_gfxPrimitives.h>
#include <math.h> // sin(), cos(), M_PI
#include <time.h> // timespec, nanosleep()
#include "version.h"

// Types
typedef struct { int x, y; } emuPoint_t;
typedef enum { DOEXIT, DONOTHING, DONEXTPATTERN, DOPAUSE } command_e;

// Defines
#define MARGIN_PERCENTAGE 0.08     // Margin around the galaxy diagram.
#define INITIAL_WINDOW_WIDTH 800   // Initial window width
#define INITIAL_WINDOW_HEIGHT 432  // Initial window height
#define PIX_SIZE 5                 // Emu pixel size is a square with sides PIX_SIZE * 2

// Globals
SDL_Window *sdlWindow = NULL;
SDL_Renderer *sdlRenderer = NULL;
emuPoint_t pixelMap[PIXEL_COUNT];
int delayMultiplier = 10;

// Prototypes
command_e HandleEvents(void);
void GeneratePixelMap(int w, int h);
void UpdateEmuOutput(galaxyData_t *gData, outputMapping_e mapType);
Uint32 ExpireTimer(Uint32 interval, void *param);
void DelayINSTR(int instructionCount);
void DelayMS(int ms);
void WindowTitle(int dMult);

#endif /* EMULATE */


// Main - Set things up, call the pattern generator.
int main (void) {

  int i;

  // Reserve the pixel data memory.
  galaxyData_t galaxy;                // Contains an array of pointers to pixels.
  color_t actualPixels[PIXEL_COUNT];  // Sets aside memory for the actual pixels.

  galaxy.size = PIXEL_COUNT;

  // Map the array of pointers to the actual pixel memory.
  for (i = 0; i < PIXEL_COUNT; i++) {
    galaxy.pixels[i] = &actualPixels[i];
  }

  // Seed the pseudorandom number generator.
  srand(25);

  // Initialize the PIC hardware
  HardwareInit();

  // Initialize the emulator hardware.
#ifdef EMULATE
  // Init the display window.
  // SDL, Simple DirectMedia Layer, is a library for access to the keyboard,
  // and graphics hardware.  See www.libsdl.org  
  if (SDL_Init(SDL_INIT_AUDIO|SDL_INIT_VIDEO|SDL_INIT_TIMER) < 0) {
    fprintf(stderr, "Unable to init SDL: %s\n", SDL_GetError());
    exit(EXIT_FAILURE);
  } else {
    // Register an exit callback to cleanup SDL.
    atexit(SDL_Quit);
  }

   // Initialize the SDL surfaces.
  SDL_CreateWindowAndRenderer(INITIAL_WINDOW_WIDTH, INITIAL_WINDOW_HEIGHT,
                              SDL_WINDOW_RESIZABLE, &sdlWindow, &sdlRenderer);
  if ((sdlWindow == NULL) || (sdlRenderer == NULL)) {
    fprintf(stderr, "Unable to create SDL window or renderer! %s\n", SDL_GetError());
    exit(EXIT_FAILURE);
  }

  // Set the window title
  WindowTitle(delayMultiplier);

  // Clear the window to black.
  SDL_SetRenderDrawColor(sdlRenderer, 0, 0, 0, 255);
  SDL_RenderClear(sdlRenderer);
  SDL_RenderPresent(sdlRenderer);

  // Generate the output pixel map
  GeneratePixelMap(INITIAL_WINDOW_WIDTH, INITIAL_WINDOW_HEIGHT);

  // Print version information.
  if (strlen(PRERELEASE_VERSION) == 0) {
    printf("Galaxy Emulator v%s.%s.%s\n", MAJOR_VERSION, MINOR_VERSION, PATCH_VERSION);
  } else {
    printf("Galaxy Emulator v%s.%s.%s-%s\n", MAJOR_VERSION, MINOR_VERSION, PATCH_VERSION, PRERELEASE_VERSION);
  }
  printf("Keys: ESC, <ctrl> c, q - Quit\n");
  printf("      +                - Increase emulation speed\n");
  printf("      -                - Decrease emulation speed\n");
  printf("      0                - Set emulation speed to 100%%\n");
  printf("      p                - Pause / unpause the simulation\n");
  printf("      SPACE            - Go to another pattern\n");
  printf("The emulator window must have focus for the key presses to work.\n");
#endif

  // Hand off control to the pattern generator.
  GeneratePattern(&galaxy);

  // We can only get this far on the emulator.
  exit(EXIT_SUCCESS);  // Required on the emu target for SDL cleanup.
  return(0);  // Never happens.
}


// Pattern generation.
void GeneratePattern(galaxyData_t *galaxy) {
  
  // Vars
  int pattern = 0;
  int initial = TRUE;
  outputMapping_e currentOutputMap = MAP_FULL;
  long int timer = 0;
#ifdef EMULATE
  unsigned char pause = FALSE;
#endif /* EMULATE */

  // Set the initial pixel state to all black.
  ColorAll(galaxy, PIXEL_BLACK);

  // Run the pattern loop.
  FOREVER {

#ifdef EMULATE
    // Handle keyboard and window events.
    switch(HandleEvents()) {
      case DOEXIT:
        return;  // Return to the main function for exit.
      case DONEXTPATTERN:
        // Pick a new pattern.
        pattern = rand() % PATTERN_COUNT;
        initial = TRUE;
        timer = 0;
        // printf("Pattern: %i\n", pattern);
        break;
      case DOPAUSE:
        pause = pause ? FALSE : TRUE;
        WindowTitle(pause ? 1000000 : delayMultiplier);  // Kludge
        break;
      case DONOTHING:
      default:
        break;
    }

    // Write to the emulator output.
    UpdateEmuOutput(galaxy, currentOutputMap);

    // If paused, restart (continue) the FOREVER loop without further processing.
    if (pause) continue; 

#else /* EMULATE */
    
    // Write to the serial port.
    WriteLights(galaxy, currentOutputMap);

#endif /* EMULATE */

    // Run the selected pattern from the patternFunctions array.
    patternList[pattern].patternFunction(galaxy, initial, &currentOutputMap);

    // Set for the first pass when a new pattern is chosen, but can be unset now.
    initial = FALSE;  

    // Have we run the pattern long enough?
    timer++;
    if (timer >= patternList[pattern].iterations) {
      // Timer expired.  Choose a new pattern and set the initial flag.
      pattern = rand() % PATTERN_COUNT;
      initial = TRUE;
      timer = 0;
    }
  } // End of FOREVER Loop
} // End GeneratePattern()


// A simple delay loop.  It should be noted that this loop may not work at
// higher levels of compiler optimization.  Below is a version for each target.
void Delay(long int d) {

#ifndef EMULATE
  // We'll delay by just sitting in a for loop for a while.
  long int i;
  for (i = 0; i < d; i++);

#else /* EMULATE */
  // Looking at the disassembly listing, and simulation output, it appears the
  // above loop runs in ~21 instruction cycles on the PIC target.  From this
  // we can approximate a delay on the emulation target.
  #define DELAY_INSTRUCTIONS 21
  DelayINSTR((int) (d * DELAY_INSTRUCTIONS));

#endif /* EMULATE */
}


// Follows, emulator support...
#ifdef EMULATE


// Handle messages from the SDL framework for keyboard and window events.
command_e HandleEvents(void) {
  
  // Vars
  SDL_Event event;
  command_e returnValue = DONOTHING;

  // Events that occured in SDL (button pushes, window resize, etc) are stored
  // in a queue as they happen.  SDL_PollEvent grabs the first event off the
  // front of the line and stored it in the provided location (&event).  When
  // there are no events left in the queue, SDL_PollEvent returns FALSE and the
  // while block is skipped.
  while (SDL_PollEvent(&event)) {

    // Process according to the type of event that occured.
    switch(event.type) {

      // A keyboard button was pushed...
      case SDL_KEYDOWN:
        // printf("Key: %i \n", (int)event.key.keysym.sym );

        // <ctrl> c, and ESC keys exit the program.
        if (((event.key.keysym.mod & KMOD_CTRL) && (event.key.keysym.sym == SDLK_c)) ||  // <ctrl> c
            (event.key.keysym.sym == SDLK_ESCAPE) ||   // ESC
            (event.key.keysym.sym == SDLK_q)) {
          returnValue = DOEXIT;
        }

        // + key increases emulation speed by decreasing delay multiplier.
        if ((event.key.keysym.sym == SDLK_EQUALS) ||
            (event.key.keysym.sym == SDLK_PLUS) ||
            (event.key.keysym.sym == SDLK_KP_PLUS)) {
          delayMultiplier --;
          if (delayMultiplier < 0) {
            delayMultiplier = 0;
          }
          WindowTitle(delayMultiplier);
          // printf("Delay Multiplier: %f\n", delayMultiplier / 10.0);
        }

        // - key decreases emulation speed by increasing delay multiplier.
        if ((event.key.keysym.sym == SDLK_MINUS) ||
            (event.key.keysym.sym == SDLK_KP_MINUS)) {
          delayMultiplier ++;
          if (delayMultiplier > 100) {
            delayMultiplier = 100;
          }
          WindowTitle(delayMultiplier);
          // printf("Delay Multiplier: %4.1f\n", delayMultiplier / 10.0);
        }

        // 0 key reset emulation speed to 1.
        if ((event.key.keysym.sym == SDLK_0) ||
            (event.key.keysym.sym == SDLK_KP_0)) {
          delayMultiplier = 10;
          WindowTitle(delayMultiplier);
          // printf("Delay Multiplier: %4.1f\n", delayMultiplier / 10.0);
        }

        // Space goes to next pattern.
        if (event.key.keysym.sym == SDLK_SPACE) {
          returnValue = DONEXTPATTERN;
        }

        // p pauses the simulation.
        if (event.key.keysym.sym == SDLK_p) {
          returnValue = DOPAUSE;
        }
        
        break;
        
      // Someone closed the window, pressed <ctrl c> in the terminal, or killed the process.
      case SDL_QUIT:
        returnValue = DOEXIT;
        break;

      // Some other thing happened to the window.
      case SDL_WINDOWEVENT:
        
        // We only care about resize events.
        if (event.window.event == SDL_WINDOWEVENT_RESIZED) {
          // printf("Resize to: %i, %i\n", event.window.data1, event.window.data2);
          GeneratePixelMap(event.window.data1, event.window.data2);
        }
        break;

      default:
        break;
    } // End event type switch
  } // End event polling loop.
  return (returnValue);
} // End HandleEvents()


// Generate the pixel layout of the galaxy based on window dimension, place
// the layout into global pixelMap.  This is called on startup and whenever
// the window is resized.
void GeneratePixelMap(int w, int h) {

  // Vars
  int i, r;
  int c1x, c1y, c2x, c2y;
  float aspect, theta;

  // r is the radius of a galactic arm.  Its half the screen width minus the margin.
  r = (((w/2) - (MARGIN_PERCENTAGE / 2 * w)) / 2);

  // Aspect is used to vary the radius during calculations so that the window
  // can be stretched in w or h independently.
  aspect = h / ((w/2) + (MARGIN_PERCENTAGE / 2 * w));

  // c1x is the center (x) of the left arm.
  c1x = w/2 - (((w/2) - (MARGIN_PERCENTAGE / 2 * w)) / 2);

  // c2x is the center (x) of the right arm.
  c2x = w/2 + (((w/2) - (MARGIN_PERCENTAGE / 2 * w)) / 2);
  
  // c1y, c2y is the center (y) of the arms.
  c1y = c2y = h/2;

  //printf("%i, %i\n%i, %i\n", c1x, c1y, c2x, c2y);
  //printf("r = %i\n", r);

  // Calculates the location of each of the galaxy pixels in the map by placing
  // them in sine-like arcs.
  for (i = 0 ; i < PIXELS_PER_ARM ; i++) {
    theta = (i + 1) * (180.0 / PIXELS_PER_ARM);
    pixelMap[i].x = c1x + (r * cosf(theta * M_PI / 180));
    pixelMap[i].y = c1y - (aspect*(r * sinf(theta * M_PI / 180)));
    pixelMap[i + PIXELS_PER_ARM].x = c2x - (r * cosf(theta * M_PI / 180));
    pixelMap[i + PIXELS_PER_ARM].y = c2y + (aspect*(r * sinf(theta * M_PI / 180)));
    //printf("Theta: %6.2f, x1 = %4i, y1 = %4i, x2 = %4i, y2 = %4i\n",
    //       theta, pixelMap[i].x, pixelMap[i].y,
    //       pixelMap[i+PIXELS_PER_ARM].x, pixelMap[i+PIXELS_PER_ARM].y);
  }
}


// This is the emulator's version of WriteLights.  The output window is updated
// here.  TODO: Reparameterize -> PIXELS_PER_ARM should come from (gData->size / 2).
void UpdateEmuOutput(galaxyData_t *gData, outputMapping_e map) {

  // Vars
  int i;

  // A word on mapping...  Layouts below are described as left to right.
  // pixelMap is generated as 20 .. 0, 21 .. 41
  // gData is 0 .. 41 in FULL, and 0 .. 20, 20 .. 0 in MIRROR.
  if (map == MAP_MIRROR) {
    // Mirrored output.
    for (i = 0; i < PIXELS_PER_ARM ; i++) {
      // Pixmap 0 .. 20.  gData 20 .. 0
      boxRGBA(sdlRenderer, pixelMap[i].x - PIX_SIZE, pixelMap[i].y - PIX_SIZE,
                           pixelMap[i].x + PIX_SIZE, pixelMap[i].y + PIX_SIZE,
                           gData->pixels[PIXELS_PER_ARM - i - 1]->r,
                           gData->pixels[PIXELS_PER_ARM - i - 1]->g,
                           gData->pixels[PIXELS_PER_ARM - i - 1]->b, 255);

      // Pixmap 21 .. 41.  gData 20 .. 0
      boxRGBA(sdlRenderer, pixelMap[i + PIXELS_PER_ARM].x - PIX_SIZE,
                           pixelMap[i + PIXELS_PER_ARM].y - PIX_SIZE,
                           pixelMap[i + PIXELS_PER_ARM].x + PIX_SIZE,
                           pixelMap[i + PIXELS_PER_ARM].y + PIX_SIZE,
                           gData->pixels[PIXELS_PER_ARM - i - 1]->r,
                           gData->pixels[PIXELS_PER_ARM - i - 1]->g,
                           gData->pixels[PIXELS_PER_ARM - i - 1]->b, 255);
    }
  } else {
    // Full output.
    for (i = 0; i < PIXELS_PER_ARM ; i++) {
      // Pixmap 0 .. 20.  gData 20 .. 0
      boxRGBA(sdlRenderer, pixelMap[i].x - PIX_SIZE, pixelMap[i].y - PIX_SIZE,
                           pixelMap[i].x + PIX_SIZE, pixelMap[i].y + PIX_SIZE,
                           gData->pixels[PIXELS_PER_ARM - i - 1]->r,
                           gData->pixels[PIXELS_PER_ARM - i - 1]->g,
                           gData->pixels[PIXELS_PER_ARM - i - 1]->b, 255);

      // Pixmap 21 .. 41.  gData 21 .. 41
      boxRGBA(sdlRenderer, pixelMap[i + PIXELS_PER_ARM].x - PIX_SIZE,
                           pixelMap[i + PIXELS_PER_ARM].y - PIX_SIZE,
                           pixelMap[i + PIXELS_PER_ARM].x + PIX_SIZE,
                           pixelMap[i + PIXELS_PER_ARM].y + PIX_SIZE,
                           gData->pixels[PIXELS_PER_ARM + i]->r,
                           gData->pixels[PIXELS_PER_ARM + i]->g,
                           gData->pixels[PIXELS_PER_ARM + i]->b, 255);
    }
  }

  // Render the scene to the window.
  SDL_RenderPresent(sdlRenderer);

  // Set the timer to emulate the serial transmission time.
  DelayMS((int) PACKET_TIME);
}


// Emulation Timer - Introduce a delay on the emulator target.  Time is in
// milliseconds.  Note, the SDL timers are only gauranteed a resulotion of 10ms.
void DelayMS(int time_ms) {

  // Vars
  SDL_TimerID timerID;
  volatile unsigned char timerRunning;

  // Note, timerRunning must be declared volatile.  This is a rule for any
  // variable altered in an interrupt.  It tells the compiler's optimizer not to
  // remove checks on it from blocks of code that don't alter it (like our
  // while(timerRunning) block below).

  // Factor in the emulation speed.
  time_ms = time_ms * (delayMultiplier / 10.0);
  
  // Set the timer.
  timerRunning = TRUE;
  timerID = SDL_AddTimer(time_ms, ExpireTimer, (void *) &timerRunning);

  // Wait until it expires.
  // while(timerRunning) ; //  This works, but uses 100% CPU.
  while(timerRunning) {
    // Sleep for 1ms (1000000 ns).  This prevents the processor from thrashing
    // while we wait for the timer to expire.
    nanosleep((struct timespec[]) {{0,1000000}}, NULL);
  }

  // Remove the expired timer.
  SDL_RemoveTimer(timerID);
}


// A delay wrapper that takes # of target instructions to delay rather than
// a time in ms.
void DelayINSTR(int instructionCount) {
  DelayMS(instructionCount * INSTRUCTION_TIME * 1000);
}


// Timer expiration callback.  
Uint32 ExpireTimer(Uint32 interval, void *timerFlag) {

  // Clear the timer flag
  *(unsigned char *)timerFlag = FALSE;

  // This tells the timer not to restart.  A repeating timer can be set up by
  // providing the time in ms to wait for next in the return statement here.
  return(0);
}


// Updates the title of the window with the speed.
void WindowTitle(int dMult) {
  // Vars
  char windowTitle[] = "Galaxy Emulator - xxxx.x%  ";

  // Build the title string.
  snprintf(windowTitle, sizeof(windowTitle),
           "Galaxy Emulator - %5.1f%%",
           (10.0 / dMult) * 100);

  // Set it.
  SDL_SetWindowTitle(sdlWindow, windowTitle);
}

#endif /* EMULATE */
