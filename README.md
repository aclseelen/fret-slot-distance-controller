# Fret Slot Distance Controller

![Luthier Alfons logo](luthier-alfons-logo-114-50.png)

By Luthier Alfons  
Version 0.2-snapshot

[https://luthieralfons.cat/en/](https://luthieralfons.cat/)

---

## Introduction

This is an Arduino project, created for the Arduino Uno. It can be edited by any IDE, but should be compiled by the Arduino IDE and loaded to the Arduino Uno by that same IDE for testing.

**NOTE: The use of this software is on your own risk and the creator takes no responsibility for potential damage to your hardware!**

A detailed description of the implementation will be given in the book *Electric Guitar Building For Nerds*, which can be expected in 2026 or 2027. The code will be published beforehand, however, and is free to use and customise for anyone, but without any warranties. But of course, it's recommended and very much appreciated if you'd buy the mentioned book, once it becomes available.

---

## Basic functionality and hardware

The software can be divided into two functional parts:
* the calculation of distances between frets for various temperaments and scale lengths;
* controlling a stepper motor with a physical interface.

The controller board as configured in the current version consists of:

* One LED screen (16×2 characters)
* One knob to control the contrast of the LED screen
* Two buttons with LED lights, where the buttons serve as inputs for the menu, and the lights as feedback from the system to the user.

Furthermore, a stepper motor should be connected to the Arduino via and appropriate/compatible stepper motor driver.

How this hardware needs to be connected to the Arduino board can be derived from the code in [FretSlotDistanceController.ino](FretSlotDistanceController.ino) under `pin assignment`. This can of course be changed by the user, but is only recommended if the user knows Arduino boards and understands what they are doing. For more information, I refer to the official Arduino documentation.

---

## The stepper motor

The stepper motor I use is a NEMA 17, and it needs to be connected to a compatible stepper motor driver, which is then wired to the correct pins of the Arduino board.

Physically, the stepper motor is connected to a threaded rod that moves a nut (that is kept straight in a slot) back and forward, by turning clockwise (back) or counter-clockwise (forward), if you look from the perspective of the stepper motor towards the rod end.

The pitch (increase in mm) of the thread needs to be entered/configured/adjusted in the code in the following line:

```c++
const double MM_INCREASE_PER_REVOLUTION = 1.25;
```
It's currently set to an M8×1.25 rod.

### Test run

It's strongly recommended to do a test run without cutting your fingerboard yet (at the very least the first time) and measure the distances and compare them to what they should be. What works well, is to measure the distance of half the scale length (e.g. fret number 12 in 12-TET) from the starting point, and compare that to the selected scale length.

It should be half, so 317.5 mm in case of a 25" (635 mm) scale length. If there's a deviation, adjust the code accordingly, recompile and upload the updated software to the board. Then test again, until you're satisfied with the configuration.

### Upgrade

Instead of a regular threaded rod and nut in a slot, one can buy a professional lead screw and lead screw nut (anti-backlash), as used in CNC technology.

### Possible issues

**_NOTE: There is no feedback from the stepper motor!_**

So, If for some reason, the physical connection is unstable between the stepper motor and its driver, it's possible that the motor will not process certain steps (which you'll notice by the sound of the motor disappearing for a second or so), while the software thinks that they were processed correctly.

If there's a power break or an unstable connection between the power supply and the Arduino and/or the stepper motor, a correct reference will be lost as well. In case the Arduino loses its power supply, the program needs to be restarted, and it won't be aware of the current position. If the stepper motor driver loses its power supply, the motor will miss the steps sent from the Arduino.

There's no programmatic solution to these issues (yet). In case of such an issue, I recommend not continuing the process of cutting slots there, since the distances will be incorrect. What I usually do is one test run, check that the program runs without issues, and draw sharp lines where the slots need to go (you can even check with a ruler whether the distances are as expected). I then go back go to the start to cut the slots and see for each position whether it still aligns with the line. If an issue occurs, there's either the option of starting from scratch and try to start it from the exact same position, or move the fingerboard by hand to the position of the lines.

I will try to figure out solutions, but the best thing to do is to create solid connections between all parts, and don't move the cables and wires while working on a fingerboard.

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

### From me to you

Limited support free of charges, for example with installation, configuration, and customisation, I exclusively offer to clients who bought my book or who bought an instrument with me. However, this software is open-source and free to use for anyone outside the mentioned group, but you're on your own, unless you hire me for consultation.

For many years I used to work in IT business as a software programmer. Take into account, that professional software consultancy is usually much more expensive than the price of a book, while their software is usually not customisable nor open-source. And companies that do create free and open-source software may charge thousands of euros for workshops where they teach you how to use it.

Since the book isn't available yet, if you need any help you can contact me and pre-order it. That really motivates me to write/finish it as soon as I can. I'm really happy to help anyone, but as you might understand, I can't offer any more of my time for free than I hereby did already. I hope and believe that you will understand that.

### From you to me

Check the GitHub project for links to my GitHub sponsors page or BuyMeACoffee, if you'd like to show me your appreciation. Any small amount of financial help is very welcome and motivating.
