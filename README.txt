Avocamentum Galactica (http://douglasruuska.com/avocamentum-galactica/), the
galaxy, a kinetic sculpture by Douglas Ruuska with electronics by Joshua
Krueger, features at its core a PIC18F452 microcontroller running a pattern
generator written in C. The pattern generator creates a serial broadcast that
is received by the LED drivers to produce the color effects seen across the
fiber optic stars of the galaxy's spiral arms. This project is that pattern
generator.

The star fibers are driven by 42 individually addressable RGB LEDs at 24 bits
per pixel color resolution. There are 21 RGB LEDs per arm, aligned roughly
sequentially.  

The software project consists of two build targets. The primary target is the
galaxy's PIC and uses the serial port and galaxy hardware as the display output.
This code was written using the C90 standard (with extensions) for the MicroChip
XC8 compiler (v1.33). The secondary target was written to allow the code to be
run on a general purpose computer with an emulated output shown in a graphical
window. The build instructions in this file apply to the emulator target.


To make this project in a build directory separate from the source:

1) Extract the archive somewhere convenient (like your home directory or
   Desktop):
   
  tar -zxvf galaxyEmulator-0.4.0.20141104.tgz
  
  Or, using git (preferred method):
  
  git clone https://github.com/argyle77/galaxyMaster.git EmulatorTarget

2) Enter the extracted directory and make a build directory.

  cd EmulatorTarget
  mkdir build
  cd build

3) Run cmake to generate the build scripts.

  cmake ..

4) Run make to compile and link the project.

  make

5) Once build is successful, run galaxyEmulator in the build/bin directory:

  bin/galaxyEmulator

Hints:

  You may need to install the following packages:
    build-essential, cmake, libsdl2-gfx-dev, git

  To do this in an Ubuntu derivative environment, run the following command:
    sudo aptitude install build-essential cmake libsdl2-gfx-dev git


When looking at this code, keep in mind that there are two build targets. 
Emulator code is demarcated in the source files using the c pre-processor
define, "EMULATOR" (source enclosed in #ifdef EMULATOR / #endif), which the
cmake build process automatically activates. It is sort of like having two
overlapping programs that share a lot of common code. The emulator outputs the
patterns to a window so that you can see a preview without needing the galaxy 
hardware on hand. I've broken the code into several files. Here is a manifest:


README.txt - This file. The project build instructions and the file manifest.

LICENSE.txt - Copyright notice and license for the project source and
    documentation.
    
CMakeLists.txt - These files contain the cmake build instructions. Don't worry
    about them unless you want to change the version numbers.
    
version.h.in - Totally unimportant. Used for versioning.

deviceConfig.h - Lots of defines for setting up the PIC hardware. Not important
    for writing pattern code.
    
galaxyConfig.h - Definitions specific to the galaxy. Some convenient constants
    and typdefs in here.
    
init.h, init.c - PIC hardware initialization functions. Unimportant.

display.h, display.c - Functions to write the pattern out to the LEDs. You won't
    need to change any of this.
    
master.c - Contains main(). Calls the pattern generators. You probably won't
    need to change any of this.
    
patternSupport.h, patternSupport.c - Contains helper functions that are useful
    in generating patterns. Currently available are:
    FadeChannel - Increases or decreases the intensity of all the LEDs of a
        given color channel.
    Shift - Rotates the color of all the LEDs up or down the arms by one step
    ColorAll - Set the color of all the pixels on the galaxy to a chosen
        color.
    GetRandomColor - Returns a random color in accordance with a selected 
        mode.
    Examples of how these functions are used can be found in pattern.c
    
pattern.h, pattern.c - A collection of pattern generation functions. This is
    where you put your pattern code. There are 6 examples already in this
    file. Add new patterns to patternList in pattern.h to get them to run.


Pattern functions manipulate the galaxy->pixels array one step at a time. The
loop in master.c calls your function over and over again and sends the results
to the serial port (or to the emulator window) each time. The iterations field 
in the patternList tells the program how many times to call a particular pattern
function before moving on to the next one. Patterns have access to the "initial"
variable, which is set to TRUE the first time a new pattern function is called,
and FALSE every time thereafter. Pattern functions can use static variables to
keep track of state information between calls. Code pieces that you write that
might be useful for multiple patterns may be candidates for inclusion as 
functions in patternSupport.c.

The emulator timing is not exact and may be thought of as suggestive of what a
pattern might look like on the galaxy itself. When run, the emulator will print
out a list of available key-presses to the terminal window so that you'll know
how to use it.

The code, as it stands, occupies 22% of the PIC data memory (RAM) and 30% of the
PIC program space (flash). The PIC can use floating point variables and
functions, but these are very memory and CPU intensive and may not result in the
timing you desire. The PIC only has 1536 bytes of RAM, so keep in mind that
though the emulator target can compile and run anything (I personally have giga-
bytes of RAM available to it), the galaxy itself is somewhat more limited.

I built this project using the Linux Mint operating system, which is an Ubuntu
derivative. It should be very easy to get it running on any Ubuntu derivative,
and relatively easy to get it running on other popular linux distributions.
With the right software and libraries installed, you may even be able to build
it in Windows or MAC (thanks to cmake), but I have little experience programming 
for either of these platforms.

To build the code for the PIC target rather than the emulator target, I suggest
obtaining a copy of MPLAB X IDE:
  http://www.microchip.com/pagehandler/en-us/family/mplabx/
as well as the XC8 compiler:
  http://www.microchip.com/pagehandler/en-us/devtools/mplabxc/home.html
both of which are available for multiple operating systems for no charge from
MicroChip. Create a simple Microchip Embedded, standalone project with the
PIC18F452 and the XC8 compiler selected and add the source files to your 
project. There should be no special settings or flags required for compilation
in this environment. If you use a different PIC for this project, you'll find
most of the stuff you need to change in deviceConfig.h

This project's use of git and presence on github are the author's efforts to
learn to use these tools.
