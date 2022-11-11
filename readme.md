# PETI Virtual Pet Development Project

## Introduction

PETI is a project to create a bespoke, retro-styled Virtual Pet running on the MSP430FR5994 microcontroller, along with a number of late-project goals to add functionality that is useful for demonstrating the sort of Hardware Hacking concepts useful in (i.e.) DEFCON Badge Challenges.

Full documentation is available as part of the "documentation" directory of this repo. A suite of helper tools is present [here](https://github.com/zadammac/peti-helpers)

This project involves both firmware and hardware development.

## Requirements
This project has a number of requirements that need to be satisfied to build the project successfully:
  - It is advised that the developer install TI's Code Composer Studio and its included compiler, or otherwise source cl430 and the rest.
  - The developer will also need to meet the requirements laid out in `hardware_design/readme.md`, and;
  - The developer will need to provide both the standard MSP430 incudes directory and the "driverlib" from MSP430, both available under license from TI.

Specific cloning/installation instructions are included under `documentation/env_setup.md`
	
## Contribution and Acknowledgement

Building PETI has been and will likely continue to be a time-consuming effort. Even at this early stage, some custom hardware has needed to be developed in order to develop the corresponding firmware.

If you would like to contribute to the development of PETI, feel free to submit a pull request, or perhaps buy me a coffee at [ko-fi.com/arclabs](https://ko-fi.com/arclabs), or consider backing [@ZAdamMac on Github Sponsors](https://github.com/users/ZAdamMac/sponsorship.)

## Make Commands
The current version of the makefile (1.0) accepts the following commands:
- `make` - performs a full rebuild of the entire source tree under `src/` and generates an output binary suitable to run on the MSP430FR5994.
- `make clean` - deletes the `build/` dir. Useful for tidying up pre-commit.
- `make target-check` - Outputs information about the currently-connected MSP430FR5994 and/or PETI Dev Kit. Sanity check to ensure a good connection.
- `make target-flash` - Performs the full rebuild then flashes the output binary to the connected PETI dev kit or other MSP430FR5994 target.
- `make target-debug` - Performs a full rebuild, starts the MSP430FR5994 connected by USB in its debugging mode, then uses msp430-dgb to enter debugging mode.
	
## Current State
PETI is currently in something of a close hold until at least Q1 2021; you can think of this as a code freeze. In order to save time on refactoring and iterative development down the road, the architecture of the main game code and main device loop is being developed on a planning scale prior to implementation.

Refer to the Projects and Wiki tabs for more information.
