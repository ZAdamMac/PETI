# Explaining the PETI Hardware
As you'll notice when reviewing the KiCAD project files included here, at present, only the layout for the Buttons Daughterboard is fully documented at this time. That is because designing the PETI device as a standalone, single-board item is a down-the-road project, relying on meeting a few other hardware prototyping goals described in the Projects and Wiki sections of the PETI github repo.

At time of writing, the PETI hardware demonstrator consists of:
- The Texas Insturments MSP-EXP430FR994, which is the hardware demonstrator for the MSP430FR5994 microcontroller as well as offering their EX-FET board as an integrated component for programming.
- A Texas Instruments BOOSTXL-SHARP128, which is TI's hardware demonstrator for the Sharp Memory Display used in the project, and;
- A buttons daughterboard.

These three components are attached to one another in a stack using the headers provided on each component.

Check on the status of the projects board before investing in the Hardware. It is possible we have completed our side-goal of providing QEMU emulation of the device.
