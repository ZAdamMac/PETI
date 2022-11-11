# Documentation for Setup of a Development Environment
This guide is current as of **v1.0** of the main project `Makefile`. If you are using a different version of the makefile you may need an older version of the documentation.

## Duplicating the Source
Cloning this repo should be sufficient to duplicate the source. Both the driver library version used in development and the actual project is included under `src/`. This is done because bugs in the driver library required minor modification to the driver source files and it was desirable to make those modified versions available to other contributors.

However, this does not install all build prerequisites.

## Obtaining CL430 and the MSP430 Headers
TI also uses a series of convenience headers alongside their custom C/C++ compiler/assembler/linker toolchain (cl430). In theory, both of these tools are available through your package manager.

However, it is also convenient - and the tested method used by the core development team - to simply install TI's recommended Code Composer Studio version in a convenient location and update the `local_settings.mk` appropriately.

Whichever method you choose, `local_settings.mk` must be modified to correctly specify the appropriate paths.

## Obtaining mspdebug and msp430-gdb
Under Ubuntu (and presumably other debian Linuxes), each of these tools is available from the `apt` package manager under their own names.

Installing these tools is necessary for the makefile to perform the flash without use of CCS itself. This may be desirable if you have another preferred IDE/editor environment. (That is, this is required for the use of the make commands `target-flash and target-debug` along with `target-check`)