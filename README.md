# SJ2-C

This is the development infrastructure for the SJ2 board, written in C that anyone can understand easily. It was designed to **learn the low level workings of a microcontroller platform with an RTOS**:

- Fully implemented in C
- Minimalistic design, little to no abstractions
- Follows good coding principles (such as YAGNI and DRY)
- State of the art build system
    - Support Mac, Linux, Windows out of the box
    - No makefiles
    - No compiler to install
    - No VMs!
    - No WSL required on windows


## Setup and Install

Setup and install should be super simple unless you have windows, which is not suited for ideal software development, but these steps should still get you setup regardless of your OS.

1. Install Python on windows (Mac and Linux should already have that)
    * Skip this for Mac or Linux
    * Follow [this guide](installs/README.md) for windows install guide
2. Open up a terminal window, and type `pip install scons`
    * If any issues on Ubuntu(linux), try `sudo apt install scons`
3. Install the Board driver from the `drivers` directory

That is it, you should now be ready to build software for your board.


## Compile & Flash

1. Use any IDE and open up the `lpc40xx_freertos` folder
2. Build the project:
    * From the root directory of this `sjtwo-c` folder, type: `scons`
    * Once you get used to it, also try `scons -j4` to use more threads to build
3. Invoke the python script to flash your new program
    * `python nxp-programmer/flash.py -p/-d/--port/--device <Device Port> --input _build/sjtwo-c.bin`
    * Note that input file defaults to `_build/sjtwo-c.bin`
    * For Mac, it might be: `python nxp-programmer/flash.py -d /dev/tty.SLAB_USBtoUART`
    * See [nxp-programmer README](nxp-programmer/README.md) for more information
4. Use your favorite serial terminal to watch the output from your board


## Build System

We use [SCons](https://scons.org/) as a build platform. The developers of SJ2-C applied experience of diverse set of build systems acquired over many years, and resorted to this one. The candidates were:

- SCons (used at Tesla, and many other companies)
- Bazel (used at Google, Zoox and many other companies)
- makefile (really?)

From experience, we can state that `bazel` is really cool, but hard to extend. SCons dependencies are tricky, but it is based on python, and easy to understand and extend. SCons is also portable and able to run on multiple platforms (Mac, Linux, Windows).


## History

We wanted to build a strong foundational sample project for SJ-2 development board that is used at San Jose State University (SJSU).

Originally, Preet created "SJ1" development board, which meant that there was one development board across multiple classes at SJSU. I was an enthusiast and created a hybrid project composed of C and C++ sources. I love C++ a little more than C because I can express artistic designs in the language, but a language is really a tool, and you need to select the right tool for the job. Presently I work on Embedded 'Firmware' code for automotive industry which is in C, and 'Software' code in C++, because C is the right tool for firmware, and C++ is the right tool for software. "SJ2" [original](https://github.com/kammce/SJSU-Dev2) software was also designed by an enthusiast (Khalil), who is a very talented person, but expressing a starter project in C++ increased the complexity so much that many developers had a difficult time adopting it.

This is where the "SJ2-C" was born, which was completely re-designed to write simple code that everyone could understand. Many students are new to programming and it was not expected that C++ would come naturally ... we simplified, simplified, and really simplified things. The code may appear less fancy (isn't that w good thing?), but it is simple to understand and trace with minimal abstractions. The goal was not to design yet another Arduino platform...
