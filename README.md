# MicroDriver - PCA9685
MicroBit driver for the PCA9685, a 16-channel PWM controller, enabling the the 
microbit to control 16 analog pins with a simple i2c connection to the chip
alowwing you to plug more LEDs, servos or other analog devices to your hearts
content. The driver also comes with built in servo support, allow you to control
servos by specifying the position in degrees.

Driver comes in both C++ library and a makecode package for use on the 
[Makecode Microbit Editor](makecode.microbit.org).

## Setup
1. Library Version
    * Include `udriver_pca9685.h` for the defintions.
    * Compile your program with `udriver_pca9685.c`, ensure you link against it
2. Makecode Version
    * Navigate to **Add Package** and enter this repository's URL.
    * Select this package from the results.

## Usage
First ensure that the PCA9685 is _connected properly_ to the MicroBit's I2c pins.
Or the driver will complain with a Microbit panic.

### 1. Library form 
    * Library provides 2 C++ classes for interacting with the PCA965
        - See `udriver_pca9685.h` for addtional documentation on members
        1. PCA9685 - The root class for interacting with PCA9685
            * Provides the core functionality
        2. PCA9685ServoController - Subclass with addtional support for controlling servos
            * Provides support for controlling servos
---
### 2. Makecode Package 
    * The package would be called "MDriver PCA9685" and have a yellow colorscheme
    * Hover over the respective blocks to display its documentation
    * Advanced functionality is located under "More..."
---

## Troubleshooting
1. MicroBit panics _(displays a frowning face) with an error code of 80.
    * The driver was unable to read from the sensor. Check whether the sensor
        is connected properly.

## License
MIT

## Supported targets
* for PXT/microbit
(The metadata above is needed for package search.)

# Random
Chew(PCA9685)
