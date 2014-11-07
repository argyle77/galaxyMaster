To make this project in a build directory separate from the source:

1) Extract the archive somewhere convenient (like your home directory or Desktop):
  tar -zxvf galaxyEmulator-0.4.0.20141104.tgz
  
  Or, if using git (preferred method):
  git clone https://github.com/argyle77/galaxyMaster.git EmulatorTarget

2) Enter the extracted directory and make a build directory.
  cd EmulatorTarget
  mkdir build
  cd build

3) Run cmake to generate the build scripts.
  cmake ..

4) Run make to compile and link the project.
  make

Once build is successful, run galaxyEmulator in the build/bin directory:
  bin/galaxyEmulator

Hints:
  You may need to install the following packages:
    build-essential, cmake, libsdl2-gfx-dev, git

  To do this in an ubuntu derivative environment, run the following command:
    sudo aptitude install build-essential cmake libsdl2-gfx-dev git


When looking at this code, keep in mind, there are two build targets.  One of
them is the PIC microcontroller on the galaxy itself.  The other is the so-
called Emulator target.  This file gives instructions on building the emulator
target. Emulator code is demarcated in the source files using the c pre-
processor definition, "EMULATOR" (source enclosed in #ifdef EMULATOR / #endif), 
which the cmake build process automatically activates.  It is sort of like 
having two overlapping programs that share a lot of common code.  The emulator 
outputs the patterns to a window so that you can see a preview without needing 
the galaxy hardware on hand.  I've broken the code into several files.  Here 
is a manifest:


README.txt - This file. The project build instructions and the file manifest.
CMakeLists.txt - These files contain the cmake build instructions. Don't worry
    about them unless you want to change the version numbers.
version.h.in - Totally unimportant.  Used for versioning.
deviceConfig.h - Lots of defines for setting up the PIC hardware.  Not important
    for writing pattern code.
galaxyConfig.h - Definitions specific to the galaxy.  Some convenient constants
    and typdefs in here.
init.h, init.c - PIC hardware initialization functions.  Unimportant.
display.h, display.c - Functions to write the pattern out to the LEDs. You won't
    need to change any of this.
master.c - Contains main().  Calls the pattern generators.  You probably won't
    need to change any of this.
patternSupport.h, patternSupport.c - Contains helper functions that are useful
    in generating patterns.  Currently available are:
      FadeChannel - Increases or decreases the intensity of all the LEDs of a
          given color channel.
      Shift - Rotates the color of all the LEDs up or down the arms by one step
      ColorAll - Set the color of all the pixels on the galaxy to a chosen
          color.
      GetRandomColor - Returns a random color in accordance with a selected 
          mode.
    You can see how these functions are used by looking in pattern.c
pattern.h, pattern.c - A collection of pattern generation functions.  This is
    where you put your pattern code.  There are 6 examples already in this
    file.  Add new patterns to patternList in pattern.h to get them to run.


Pattern functions manipulate the galaxy->pixels array one step at a time.  The
loop in master.c calls your function over and over again and sends the results
to the slaves (or to the emulator window) each time.  The iterations field in
the patternList tells the loop how many times to call a particular pattern
before moving on to the next one.  Patterns have access to the "initial"
variable, which is set to TRUE the first time a new pattern function is called,
and FALSE every time thereafter.  Pattern functions can use static variables to
keep track of their state information between calls.  Code pieces that you write
that might be useful for multiple patterns may be candidates for inclusion as
functions in patternSupport.c.

The emulator timing is not exact and may be thought of as suggestive of what a
pattern might look like on the galaxy itself. When run, the emulator will print
out a list of available keypresses to the terminal window so that you'll know
how to use it.

The code, as it stands, occupies 22% of the PIC data memory (RAM) and 30% of the
PIC program space (flash).  The PIC can use floating point variables and
functions, but these are very memory and CPU intensive and may not result in the
timing you desire.  The PIC only has 1536 bytes of RAM, so keep in mind that
though the emulator target can compile and run anything (I personally have 8 GB
of RAM available to it), the galaxy itself is somewhat more limited.

I built this project using the Linux Mint operating system, which is an Ubuntu
derivative.  It should be very easy to get it running on any Ubuntu derivative,
and relatively easy to get it running on other popular linux distributions.
With the right libraries installed, you may even be able to build it in windows
or on MAC (thanks to cmake), but I have little experience programming for either
of these platforms.

To build the code for the PIC target rather than the emulator target, I suggest
obtaining a copy of MPLAB X IDE:
  http://www.microchip.com/pagehandler/en-us/family/mplabx/
as well as the XC8 compiler:
  http://www.microchip.com/pagehandler/en-us/devtools/mplabxc/home.html
both of which are available for Windows, Linux, and MAC for free from MicroChip.
