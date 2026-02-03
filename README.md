# Fret Slot Distance Controller

![Luthier Alfons logo](luthier-alfons-logo-114-50.png)

By Luthier Alfons  
Version 0.2-snapshot

[https://luthieralfons.cat/en/](https://luthieralfons.cat/)

---

## Introduction

This is an Arduino project, created for the Arduino Uno. It can be edited by any IDE, but should be compiled by the Arduino IDE and loaded to the Arduino Uno by that same IDE for testing.

**NOTE: The use of this software is on your own risk and the creator takes no responsibility for potential damage to your hardware!**

A detailed description of the implementation will be given in the book *Electric Guitar Building For Nerds*, which can be expected in 2026 or 2027. The code will be published beforehand, however, and is free to use and customise for anyone, but without any guarantees. But of course, it's recommended and very much appreciated if you'd buy the mentioned book, once becomes available.

---

## Basic functionality and hardware

The software can be divided in two functional parts:
* the calculation of distances between frets for various temperaments and scale lengths;
* controlling a stepper motor with a physical interface.

The controller board as configured in the current version consists of:

* One LED screen (16×2 characters)
* One knob to control the contrast of the LED screen
* Two buttons with LED light that as inputs for the menu

Furthermore, a stepper motor and stepper motor driver should be connected to the Arduino.

How this hardware needs to be connected to the Arduino board cam be derived from the code in [FretSlotDistanceController.ino](FretSlotDistanceController.ino) under `pin assignment`. This can of course be changed by the user, but is only recommended if the user knows Arduino boards and understands what they are doing. For more information I refer to the official Arduino documentation.

---

## The stepper motor

The stepper motor I use is a NEMA 17, and needs to be connected to a compatible stepper motor driver, which is then wired to the correct pins of the Arduino board.

Physically, the stepper motor is connected to a threaded rod that moves a nut (that is kept straight in a slot) back and forward, by turning clockwise (back) or counter-clockwise (forward).

The pitch (increase in mm) of the thread needs to be entered/configured/adjusted in the code in the following line:

```c++
const double MM_INCREASE_PER_REVOLUTION = 1.25;
```
It's currently set to an M8×1.25 rod.

### Test run

It's strongly recommended to do a test run without cutting your fingerboard yet (at the very least the first time) and measure the distances and compare them to what they should be. What works well, is to measure the distance of half the scale length (e.g. fret number 12 in 12-TET) from the starting point, and compare that to the selected scale length.

It should be half, so 317.5 mm in case of a 25" (635 mm) scale length. If there's a deviation, adjust the code accordingly, recompile and upload the updated software to the board. Then test again, until you're satisfied with the configuration.

### Upgrade

Instead of a regular threaded rod and nur in a slot, one can buy a professional lead screw and lead screw nut (anti-backlash), like used in CNC technology.

---

## Tuning and scale length options

Currently, there's an implementation for the following scale lengths:

* 25"
* 25.5"
* 650 mm
* 33.25"
* 34"

And the following temperaments:

* 12-TET (21)
* Meantone
* 19-TET
* 7-TET

This can of course be extended by the user.

---

## Support

Limited support free of charges, for example with installation, configuration, and customisation, I exclusively offer to clients who bought my book or who bought an instrument with me. However, this software is open-source and free to use for anyone outside the mentioned group, but you're on your own, unless you hire me for consultation.

For many years I used to work in IT business as a software programmer. Take into account, that professional software consultancy is usually much more expensive than the price of a book, while the software is not customisable nor open-source.

Since the book isn't available yet, if you need any help you can contact me and pre-order it. That really motivates me to write/finish it as soon as I can.
