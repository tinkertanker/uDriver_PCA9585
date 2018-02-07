/*
 * udriver_pca9685.ts
 * PCA9685 driver for the Microbit
*/

/** 
 * PCA9685 μDriver allows the MicroBit to leverge the PCA9685 pletoras of pins 
 * for applications such controling a muiltitude of LEDs or servos.
*/
//% color=#b58900 icon="\uf2db" block="μDriver PCA9685"
namespace UDriver_PCA9685 
{
    /**
     * Defines the analog pins on the PCA9685 
    */
    export enum Pin
    {
        P0 = 0, /* Pin value autoincrements 0...15*/
        P1,
        P2,
        P3,
        P4,
        P5,
        P6,
        P7,
        P8,
        P9,
        P10,
        P11,
        P12,
        P13,
        P14,
        P15
    }

}
